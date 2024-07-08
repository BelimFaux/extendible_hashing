#include "EH_set.h"
#include <sstream>
#include <string>
#include <iostream>

#ifdef SIZE
using set = EH_set<unsigned, SIZE>;
#else
using set = EH_set<unsigned>;
#endif

std::istringstream get_input() {
    std::string inp;
    std::getline(std::cin, inp);
    return std::istringstream(inp);
}

void input(set *set) {
    std::cout << "input> ";
    std::istringstream inp_stream{get_input()};
    unsigned i;
    while (inp_stream >> i)
        set->insert(i);
}

void remove(set *set) {
    std::cout << "remove> ";
    std::istringstream inp_stream{get_input()};
    unsigned i;
    while (inp_stream >> i)
        set->erase(i);
}

void find(set *set) {
    std::cout << "find> ";
    std::istringstream inp_stream{get_input()};
    unsigned i;
    while (inp_stream >> i) {
        set::iterator it{set->find(i)};
        if (it == set->end())
            std::cout << "Element not found";
        else {
            auto pos{it.get_pos()};
            std::cout << "bucket: " << pos.first << ", index: " << pos.second;
        }
        std::cout << '\n';
    }
}


int main() {
    set set;
    bool changed;

    std::string line;
    while (std::cout << "cmd> ", std::getline(std::cin, line)) {
        changed = false;
        std::istringstream line_stream(line);
        char cmd;
        line_stream >> cmd;

        switch (cmd) {
            case 'i':
                input(&set);
                changed = true;
                break;
            case 'r':
                remove(&set);
                changed = true;
                break;
            case 'f':
                find(&set);
                break;
            case 'l':
                for (unsigned i: set)
                    std::cout << i << " ";
                std::cout << '\n';
                break;
            case 's':
                std::cout << set.size() << '\n';
                break;
            case 'c':
                set.clear();
                changed = true;
                break;
            case 'p':
                set.dump();
                break;
            case 'h':
                std::cout << "Help\n"
                          << "  i - insert elements in set\n"
                          << "  r - remove elements from set\n"
                          << "  f - find elements in set\n"
                          << "  l - list all elements in set (using foreach loop)\n"
                          << "  s - show size of set\n"
                          << "  c - clear set\n"
                          << "  p - print current set\n"
                          << "  h - show help page (this screen)\n"
                          << "  q (or EOF) - quit\n";
                break;
            case 'q':
                return 0;
            default:
                std::cout << "unknown command\n";
        }
    #ifndef NVERB
        if (changed)
            set.dump();
    #endif
    }

    return 0;
}
