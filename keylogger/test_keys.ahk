#Requires AutoHotkey v2.0

; Wait 2 seconds to focus Winkey logger window
Sleep 2000

; -----------------------------
; Normal sentence
; -----------------------------
Send("The quick brown fox jumps over the lazy dog.")
Sleep 100

; -----------------------------
; Numbers 0-9 and Shifted symbols
; -----------------------------
symbols := ")!@#$%^&*("
Loop 10
{
    Send(Chr(48 + A_Index - 1))       ; number key 0-9
    Sleep 50
    Send(SubStr(symbols, A_Index, 1)) ; Shifted symbol
    Sleep 50
}

; -----------------------------
; Ctrl combo (representative)
; -----------------------------
Send("^s")  ; Ctrl+S
Sleep 50

; -----------------------------
; Shift combos (representative letters)
; -----------------------------
Send("+a")  ; Shift+A
Sleep 50
Send("+b")  ; Shift+B
Sleep 50

; -----------------------------
; Left Alt combos (representative)
; -----------------------------
Send("!f")  ; Alt+F
Sleep 50
Send("!e")  ; Alt+E
Sleep 50

; -----------------------------
; AltGr / RightAlt combos (representative)
; -----------------------------
Send("{RAlt down}q{RAlt up}") ; AltGr+Q (DE layout → @)
Sleep 50
Send("{RAlt down}e{RAlt up}") ; AltGr+E (FR layout → €)
Sleep 50

; -----------------------------
; Function key F3 only
; -----------------------------
Send("{F7}")
Sleep 50

; -----------------------------
; Common special keys
; -----------------------------
Send("{Tab}")
Sleep 50
Send("{Backspace}")
Sleep 50
Send("{Esc}")
Sleep 50
Send("{Space}")
Sleep 50
Send("{Up}")
Sleep 50
Send("{Down}")
Sleep 50
Send("{Left}")
Sleep 50
Send("{Right}")
Sleep 50
Send("{Home}")
Sleep 50
Send("{End}")
Sleep 50
Send("{Insert}")
Sleep 50
Send("{Delete}")
Sleep 50
Send("{Enter}")

MsgBox("Test script finished sending keys to Winkey logger!")





; Path to the log file
logFile := "ks.log"

; Path to the answer file
answerFile := "answer.txt"


; Read the answer file
expected := FileRead(answerFile)

; Read the log file
content := FileRead(logFile)

; Remove newlines
expected := StrReplace(expected, "`r")
expected := StrReplace(expected, "`n")
content := StrReplace(content, "`r")
content := StrReplace(content, "`n")

; Compare lengths first
minLen := (StrLen(content) < StrLen(expected)) ? StrLen(content) : StrLen(expected)
diffCount := 0
diffPositions := []

Loop minLen
{
    if SubStr(content, A_Index, 1) != SubStr(expected, A_Index, 1)
    {
        diffCount++
        diffPositions.Push(A_Index)
    }
}

; If strings differ in length, record the extra characters
if StrLen(content) != StrLen(expected)
{
    diffCount += Abs(StrLen(content) - StrLen(expected))
    Loop Abs(StrLen(content) - StrLen(expected))
        diffPositions.Push(minLen + A_Index)
}

; Report
if diffCount = 0
{
    MsgBox("Log matches expected string!")
}
else
{
    msg := "Log differs from expected string!`n"
    msg .= "Total differences: " diffCount "`n"

	; Join diffPositions manually
	posStr := ""  ; initialize to avoid #Warn warning
	for i, v in diffPositions
		posStr .= (i = 1 ? "" : ", ") v

    msg .= "Expected: " expected "`n"
    msg .= "Actual:   " content
    MsgBox(msg)
}
