# Winkey Keylogger
A Windows keylogger in C++ using WinAPI, with Unicode support, active window tracking, etc.

## Functionality
- When the maximum key repetition count is reached, we stop printing it.

---

## TODO
- check dead keys behavior on < w10

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
* If you open the file in a non-UTF-8 compatible editor like:
    - Windows Notepad (older versions)
    - `type logfile.txt` in a Windows console with non-Unicode codepage
    - Any tool that assumes ANSI encoding<br />
You’ll see mojibake like `ã‚` instead of the intended Japanese kana.<br /><br />

=> Open the log file in a proper UTF-8-aware viewer:<br />
   - **Notepad++** (select `Encoding → UTF-8`)
   - **Visual Studio Code**
   - **`more logfile.txt`** from `cmd` after running `chcp 65001`, etc...

#### Why we won’t get Japanese characters from the keyboard hook

When a user types Japanese, input is usually processed through the Windows IME (Input Method Editor). Unlike direct keyboard input, the IME composes text in stages and sends it through different message types such as:
- WM_IME_STARTCOMPOSITION
- WM_IME_COMPOSITION
- WM_IME_ENDCOMPOSITION<br />

Those are sent to the focused window, not to the low-level keyboard hook we’re using.<br />
That means our hook never sees the resulting composed Japanese text — it only sees raw key codes before conversion.

---
Here’s a polished and technically precise version of your README section — keeping your tone but improving clarity, grammar, and formatting:

---

### Dead Keys Problem

#### The Problem

* We use the `ToUnicodeEx` function to translate virtual-key codes into their corresponding Unicode characters.

* However, as stated in the [Microsoft Learn documentation](https://learn.microsoft.com/en-gb/windows/win32/api/winuser/nf-winuser-tounicodeex):

  > As ToUnicodeEx translates the virtual-key code, it also changes the state of the kernel-mode keyboard buffer.
  > This state-change affects dead keys, ligatures, Alt+Numeric keypad key entry, and so on.

* Because of this behavior, `ToUnicodeEx` modifies the system keyboard buffer used by the active application.

* As a result, when our hook calls `ToUnicodeEx` for logging purposes, it can interfere with the user’s actual input — producing odd results such as `^^e` instead of `ê`.

#### The Solution

* The same documentation notes that the `wFlags` parameter of `ToUnicodeEx` controls its behavior. Specifically:

  > If bit 2 is set, the keyboard state is not changed (Windows 10, version 1607 and newer).

* Therefore, by setting **bit 2** in `wFlags`, we can safely translate the key without altering the system keyboard state.

```cpp
int result = ToUnicodeEx(
    p->vkCode,
    p->scanCode,
    keyboardState,
    buffer,
    TW_KEYSTROKE_MAX,
    0x0004, // Set bit 2 → do not change keyboard state
    layout
);
```

* This prevents `ToUnicodeEx` from consuming or modifying dead keys, allowing composed characters like “ê” to appear correctly while still logging them accurately.

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
- [SetWindowsHookExA function (Microsoft Learn)](https://learn.microsoft.com/fr-fr/windows/win32/api/winuser/nf-winuser-setwindowshookexa)
- [TranslateMessage function (Microsoft Learn)](https://learn.microsoft.com/en-gb/windows/win32/api/winuser/nf-winuser-translatemessage)
- [Using Messages and Message Queues (Microsoft Learn)](https://learn.microsoft.com/en-gb/windows/win32/winmsg/using-messages-and-message-queues)
- [Keylogger Tutorial (Synacktiv)](https://www.synacktiv.com/publications/writing-a-decent-win32-keylogger-13)
- [Virtual Key Codes (Windows Learn)](https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes)
- [Keyboard input on Windows, Part II: The semantic of ToUnicode()](https://metacpan.org/dist/UI-KeyboardLayout/view/lib/UI/KeyboardLayout.pm#Keyboard-input-on-Windows,-Part-II:-The-semantic-of-ToUnicode())