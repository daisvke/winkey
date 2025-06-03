# Winkey Keylogger

## TODO
- ^^e

A minimal Windows keylogger in C++ using WinAPI, with Unicode support, active window tracking, and intelligent uppercase handling.

## Features

- Logs all keystrokes globally.
- Detects and logs active window title changes.
- Handles uppercase and shifted input (e.g., `A` vs `a`, `!` vs `1`).
- Prevents multiple instances via named mutex.
- Writes logs to a UTF-16LE encoded `.log` file.

---

## 🔧 How It Works

### 1. **Preventing Duplicate Instances**
On startup:
```cpp
CreateMutex(NULL, TRUE, TEXT(TW_MUTEX_NAME));
````

If a previous instance exists, it throws `InstanceAlreadyRunnningException()`.

---

### 2. **Hook Setup**

```cpp
SetWinEventHook(EVENT_SYSTEM_FOREGROUND, ...); // For window title tracking
SetWindowsHookEx(WH_KEYBOARD_LL, ...);         // For global keystroke logging
```

---

### 3. **Keystroke Logging Logic**

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
How to confirm it's really UTF-8

    Open the log file in a proper UTF-8-aware viewer:

        Notepad++ (select Encoding → UTF-8)

        Visual Studio Code

        more logfile.txt from cmd after running chcp 65001
---

### 4. **Window Change Detection**

Whenever a foreground window changes:

```cpp
winEventProc(...) ➜ GetWindowTextW(...) ➜ _windowTitle
```

Logged with timestamp in the following format:

```
[DD.MM.YYYY HH:MM:SS] - 'Window Title'
```

---

### 5. **File Output**

* The log file (`TW_LOGFILE`) is opened in append mode.
* Each keystroke is immediately flushed to disk using `_logFile.flush()`.

---

## 📦 Output Example

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
