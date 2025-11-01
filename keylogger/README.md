# Winkey Keylogger

A Windows keylogger in C++ using WinAPI, with Unicode support, active window tracking, etc.

## TODO
- ^^e

## Features

- Logs all keystrokes globally.
- Detects and logs active window title changes.
- Handles uppercase and shifted input (e.g., `A` vs `a`, `!` vs `1`).
- Prevents multiple instances via named mutex.
- Writes logs to a UTF-16LE encoded `.log` file.

---

## How It Works

### **Preventing Duplicate Instances**
On startup:
```cpp
CreateMutex(NULL, TRUE, TEXT(TW_MUTEX_NAME));
````
If a previous instance exists, it throws `InstanceAlreadyRunnningException()`.

---

### **Hook Setup**

```cpp
SetWinEventHook(EVENT_SYSTEM_FOREGROUND, ...); // For window title tracking
SetWindowsHookEx(WH_KEYBOARD_LL, ...);         // For global keystroke logging
```

---

### **Keystroke Logging Logic**

![Windows Keystroke Scheme](screenshots/keystroke-scheme.png)
[Source](https://www.synacktiv.com/publications/writing-a-decent-win32-keylogger-23)

```cpp
ToUnicodeEx(...)  // Converts VK code + scan code to readable character
```

#### Modifier Keys (Handled Manually)

* `VK_SHIFT` (for uppercase & symbols)
* `VK_CAPITAL` (Caps Lock toggle)
* `VK_CONTROL` / `VK_MENU` (optional for context-aware logging)

The logger uses:

```cpp
GetAsyncKeyState(VK_SHIFT) & 0x8000
GetKeyState(VK_CAPITAL) & 0x0001
```

This ensures uppercase characters are detected **only** when:

* Shift is held OR
* Caps Lock is toggled (for alphabetic keys)

#### Unicode Characters
* explain about wchar_t
* Japanese
    // Tell std::wofstream to use UTF-8 encoding when writing wide
    //  characters (wchar_t) to the file.
    _logFile.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));

#### How to confirm it's really UTF-8

If you're getting **mojibake** in the window title like:

```
[03.06.2025 03:18:28] - 'ã‚ã‚‰^ã€œ (ã‚ã‚‰ãƒ¼)...'
```

The program is opening the log file with UTF-8 encoding, but you're later reading it in an **ANSI or misconfigured text viewer** (like Notepad).

The program is correctly doing:

```cpp
_logFile.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
```

Which means: the program **writes UTF-8** to disk.

But then, if you open the file in a non-UTF-8 compatible editor like:

* Windows Notepad (older versions)
* `type logfile.txt` in a Windows console with non-Unicode codepage
* Any tool that assumes ANSI encoding

You’ll see mojibake like `ã‚` instead of the intended Japanese kana.

### **How to confirm it's really UTF-8**

1. Open the log file in a proper UTF-8-aware viewer:

   * **Notepad++** (select `Encoding → UTF-8`)
   * **Visual Studio Code**
   * **`more logfile.txt`** from `cmd` after running `chcp 65001`

---

### Why we won’t get Japanese characters from the keyboard hook

When a user types Japanese, input is usually processed through the Windows IME (Input Method Editor). Unlike direct keyboard input, the IME composes text in stages and sends it through different message types such as:
- WM_IME_STARTCOMPOSITION
- WM_IME_COMPOSITION
- WM_IME_ENDCOMPOSITION<br />

Those are sent to the focused window, not to the low-level keyboard hook we’re using.<br />
That means our hook never sees the resulting composed Japanese text — it only sees raw key codes before conversion.

---

### **Window Change Detection**

Whenever a foreground window changes:

```cpp
winEventProc(...) ➜ GetWindowTextW(...) ➜ _windowTitle
```

Logged with timestamp in the following format:

```
[DD.MM.YYYY HH:MM:SS] - 'Window Title'
```

---

### **File Output**

* The log file (`TW_LOGFILE`) is opened in append mode.
* Each keystroke is immediately flushed to disk using `_logFile.flush()`.

---

## Output Example

```
[01.06.2025 10:15:42] - 'Visual Studio Code'

```

---

## Build Instructions

```powershell
nmake
```

---

## Disclaimer

This software is for **educational purposes only**. Unauthorized use of keyloggers may violate privacy laws. Use responsibly.

## Documentation

[Keylogger Tutorial (Synacktiv)](https://www.synacktiv.com/publications/writing-a-decent-win32-keylogger-13)
[Virtual Key Codes (Windows Learn)](https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes)