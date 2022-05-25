#include <iostream>

#include "minerva/version.hpp"

using namespace minerva;

int main() {
    //minerva info
    std::cout << "Minerva v" << VERSION_MAJOR << '.' << VERSION_MINOR << '.' << VERSION_PATCH << '\n';
}
