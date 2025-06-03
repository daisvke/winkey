#include "Winkey.hpp"

int main() {
    Winkey *w = new Winkey();

    try {
        w->run();
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        delete w;
        return 1;
    }

    delete w;
    return 0;
}
