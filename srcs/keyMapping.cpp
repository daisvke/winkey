#include <windows.h>
#include <iostream>

// Get the key name to output from the given VK code
std::wstring getKeyName(const UINT vkCode) {
    switch (vkCode) {
        case VK_BACK:         return L"[Backspace]";
        case VK_TAB:          return L"[Tab]";
        case VK_CLEAR:        return L"[Clear]";
        case VK_RETURN:       return L"[Enter]";
        case VK_SHIFT:        return L"[Shift]";
        case VK_CONTROL:      return L"[Ctrl]";
        case VK_MENU:         return L"[Alt]";
        case VK_PAUSE:        return L"[Pause]";
        case VK_CAPITAL:      return L"[CapsLock]";
        case VK_KANA:         return L"[Kana]";
        case VK_JUNJA:        return L"[Junja]";
        case VK_FINAL:        return L"[Final]";
        case VK_HANJA:        return L"[Hanja]";
        case VK_ESCAPE:       return L"[Esc]";
        case VK_CONVERT:      return L"[Convert]";
        case VK_NONCONVERT:   return L"[NoConvert]";
        case VK_ACCEPT:       return L"[Accept]";
        case VK_MODECHANGE:   return L"[ModeChange]";
        case VK_SPACE:        return L"[Space]";
        case VK_PRIOR:        return L"[PageUp]";
        case VK_NEXT:         return L"[PageDown]";
        case VK_END:          return L"[End]";
        case VK_HOME:         return L"[Home]";
        case VK_LEFT:         return L"[Left]";
        case VK_UP:           return L"[Up]";
        case VK_RIGHT:        return L"[Right]";
        case VK_DOWN:         return L"[Down]";
        case VK_SELECT:       return L"[Select]";
        case VK_PRINT:        return L"[Print]";
        case VK_EXECUTE:      return L"[Execute]";
        case VK_SNAPSHOT:     return L"[PrintScreen]";
        case VK_INSERT:       return L"[Insert]";
        case VK_DELETE:       return L"[Delete]";
        case VK_HELP:         return L"[Help]";
        
        // Function keys
        case VK_F1:           return L"[F1]";
        case VK_F2:           return L"[F2]";
        case VK_F3:           return L"[F3]";
        case VK_F4:           return L"[F4]";
        case VK_F5:           return L"[F5]";
        case VK_F6:           return L"[F6]";
        case VK_F7:           return L"[F7]";
        case VK_F8:           return L"[F8]";
        case VK_F9:           return L"[F9]";
        case VK_F10:          return L"[F10]";
        case VK_F11:          return L"[F11]";
        case VK_F12:          return L"[F12]";
        
        case VK_NUMLOCK:      return L"[NumLock]";
        case VK_SCROLL:       return L"[ScrollLock]";
        case VK_LSHIFT:       return L"[LeftShift]";
        case VK_RSHIFT:       return L"[RightShift]";
        case VK_LCONTROL:     return L"[LeftCtrl]";
        case VK_RCONTROL:     return L"[RightCtrl]";
        case VK_LMENU:        return L"[LeftAlt]";
        case VK_RMENU:        return L"[RightAlt]";

        default:
            // Unknown key: just print the hex code
            wchar_t	buf[16];
            swprintf(buf, sizeof(buf) / sizeof(wchar_t), L"[VK_%02X]", vkCode);
            return std::wstring(buf);
    }
}

// Check if the character is a printable unicode character
bool isPrintable(wchar_t c) {
    return (c >= 0x20 && (c < 0x7F || c >= 0xA0));
}
