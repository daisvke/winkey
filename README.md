# tinky-winkey

## Setting up Windows

### Install Build Tools (CL, NMAKE)

1. **Download the Build Tools for Visual Studio**:

   * Go to the official Microsoft download page:
    [https://visualstudio.microsoft.com/downloads/](https://visualstudio.microsoft.com/downloads/)
   * Scroll all the way down to **“Tools for Visual Studio”**.
   * Click **“Build Tools for Visual Studio”** and download it.

2. **Install Only Required Components**:

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

4. **Verify Installation**:

   ```cmd
   nmake /?
   ```

   You should see the help menu for NMAKE.
