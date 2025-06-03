#include "Winkey.hpp"

int main() {
    try {
        Winkey w;
        w.run();
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
