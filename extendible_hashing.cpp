#include "EH_set.h"
#include <iostream>

int main() {
    EH_set<unsigned> set{1, 4, 0, 5, 2, 10, 30, 40, 23, 42, 45, 3};

    set.dump();

    return 0;
}
