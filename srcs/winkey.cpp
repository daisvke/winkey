#include "winkey.hpp"

std::ofstream   logFile;

void LogForegroundWindow(void) {
    // Get a handle to the currently focused window
    HWND    hwnd = GetForegroundWindow();

    char    windowTitle[256];
    GetWindowTextA(hwnd, windowTitle, sizeof(windowTitle) / sizeof(char));

    time_t      now = time(nullptr);
    struct tm   localTime {};
    localtime_s(&localTime, &now);

    logFile << "[" << std::setfill('0')
            << std::setw(2) << localTime.tm_mday << "."
            << std::setw(2) << (localTime.tm_mon + 1) << "."
            << (localTime.tm_year + 1900) << " "
            << std::setw(2) << localTime.tm_hour << ":"
            << std::setw(2) << localTime.tm_min << ":"
            << std::setw(2) << localTime.tm_sec << "] - '"
            << windowTitle << "'" << std::endl;
}

int main(void) {
    try {
        // Open the outfile in appending mode
        logFile.open("winkey.log", std::ios::app);
        if (!logFile.is_open()) {
            std::cerr << "Failed to open log file." << std::endl;
            return 1;
        }

        LogForegroundWindow();

        logFile.close();
    } catch (const std::exception &e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }

    return 0;
}