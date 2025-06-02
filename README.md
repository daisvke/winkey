# tinky-winkey

## TODO
- doesn't work on open tab on firefox

## Documentation
- [SetWindowsHookExA function (Microsoft Learn)](https://learn.microsoft.com/fr-fr/windows/win32/api/winuser/nf-winuser-setwindowshookexa)
- [TranslateMessage function (Microsoft Learn)](https://learn.microsoft.com/en-gb/windows/win32/api/winuser/nf-winuser-translatemessage)
- [Using Messages and Message Queues (Microsoft Learn)](https://learn.microsoft.com/en-gb/windows/win32/winmsg/using-messages-and-message-queues)

## Setting up Windows

### Install Build Tools (CL, NMAKE)

1. **Download the Build Tools for Visual Studio**:

   * Go to the official Microsoft download page:
    [https://visualstudio.microsoft.com/downloads/](https://visualstudio.microsoft.com/downloads/)
   * Scroll all the way down to **“Tools for Visual Studio”**.
   * Click **“Build Tools for Visual Studio”** and download it.

2. **Install only required components**:

   * Run the installer.
   * Select the **“desktop development with C++”** workload.
   * Ensure these components are checked:

     * MSVC v142 or later (C++ build tools)
     * Windows 10 SDK
     * **C++ CMake tools for Windows**
     * **Windows SDK for Windows 10**
   * Click Install.

  If Microsoft.VisualCpp.Redist.14 installation fails, follow the steps on:
    https://developercommunity.visualstudio.com/t/PackageId:MicrosoftVisualCppRedist14;/10902964

3. **Launch Developer Command Prompt**:

   * After installation, search for **“x64 Native Tools Command Prompt for VS”** in Start Menu.
   * This sets all environment variables, including for `nmake`.

4. **Verify installation**:

   ```cmd
   nmake /?
   ```

   You should see the help menu for NMAKE.

---

### **Add right-click “Open VS Build Tools here”**

Here’s how to make it easier to open the proper command line from any folder:

1. Open Notepad
2. Paste the following (adjust path if needed):

```reg
Windows Registry Editor Version 5.00

[HKEY_CLASSES_ROOT\Directory\Background\shell\VS2022_x64_Tools]
@="Open VS 2022 x64 Tools Here"

[HKEY_CLASSES_ROOT\Directory\Background\shell\VS2022_x64_Tools\command]
@="\"C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\VC\\Auxiliary\\Build\\vcvars64.bat\" && cmd.exe"
```

3. Save it as `vs-tools-here.reg`
4. Double-click to add it to your registry.

Now right-click any folder background → “Open VS 2022 x64 Tools Here”

---

### **How to use `nmake` in VS Code**

To use `nmake` from within **VS Code**, you need to launch VS Code **with the environment set up by the "x64 Native Tools Command Prompt"**.

1. Open: **"x64 Native Tools Command Prompt for VS 2022"**
2. Navigate to your project folder:

   ```cmd
   cd path\to\your_project
   ```
3. Launch VS Code:

   ```cmd
   code .
   ```

Now, any **terminal inside VS Code** will inherit the environment (`cl`, `nmake`, etc. will work).

### **`nmake` and cmd.exe
nmake uses cmd.exe under the hood — not PowerShell
   * The syntax you use in the Makefile (like `del`) is interpreted by Command Prompt, not PowerShell.
   * For instance, `Remove-Item` is a PowerShell command — nmake won't understand it and will throw an error.

---

### **Setting up PowerShell**

#### Adding aliases

1. Open the PowerShell profile:

```powershell
notepad $PROFILE
```

2. Add the aliases as functions:

```powershell
# Git
function gst { git status }
function gph { git push }  # gp is already taken 
function ga { git add $args }
function gcmsg { git commit -m $args }
```

Save and quit.

3. Save and restart PowerShell or run:

```powershell
. $PROFILE
```
