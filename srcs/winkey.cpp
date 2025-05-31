#include "winkey.hpp"

std::wofstream  logFile;
HWND            lastWindow = nullptr;
HANDLE          singleInstanceMutex = nullptr;

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
    logFile << L"[" << std::setfill(L'0')
            << std::setw(2) << localTime.tm_mday << L"."
            << std::setw(2) << (localTime.tm_mon + 1) << L"."
            << (localTime.tm_year + 1900) << L" "
            << std::setw(2) << localTime.tm_hour << L":"
            << std::setw(2) << localTime.tm_min << L":"
            << std::setw(2) << localTime.tm_sec << L"] - '"
            << windowTitle << L"'" << std::endl;
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

    while (1) {
        LogForegroundWindow();
        // Check every 500ms
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    // Cleanup
    CloseHandle(singleInstanceMutex);
    logFile.close();

    return 0;
}
