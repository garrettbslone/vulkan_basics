#include <iostream>
#include <cstdlib>
#include <stdexcept>

#include "app.hpp"

using namespace std;

int main()
{
    try {
        app a{};
        a.run();
    } catch (const exception &e) {
        cerr << e.what() << endl;
        return -1;
    }

    return 0;
}
