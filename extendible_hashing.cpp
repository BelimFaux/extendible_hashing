#include "EH_set.h"
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

#ifndef SIZE
#define SIZE 16
#endif

using set = EH_set<unsigned, SIZE>;

std::vector<unsigned> stov(std::string inp) {
    std::vector<unsigned> v;
    std::string s;

    for (char c: inp) {
        if (c == ' ') {
            if (!s.empty())
                v.push_back(std::stoi(s));
            continue;
        }
        if (c > 47 && c < 58)
            s.push_back(c);
        else throw std::runtime_error("input contains nondigit characters");
    }

    return v;
}

void input(set *set) {
    std::string inp;
    std::cout << "input > ";
    std::cin >> inp;

    std::vector<unsigned> ins_v;
    try {
        ins_v = stov(inp);
    } catch(std::runtime_error &e) {
        std::cout << e.what() << '\n';
        return;
    }
    set->insert(ins_v.begin(), ins_v.end());
#ifndef NVERB
    set->dump();
#endif
}

int main() {
    set set;

    while(true) {
        char i;
        std::cout << "cmd > ";
        std::cin >> i;

        switch (i) {
            case 'i':
                input(&set);
                break;
            case 'h':
                std::cout << "Help\n  i - insert elements in container\n  h - show help page (this)\n  q - quit\n";
                break;
            case 'q':
                return 0;
            default:
                std::cout << "unknown command\n";
        }
    }

    return 0;
}
