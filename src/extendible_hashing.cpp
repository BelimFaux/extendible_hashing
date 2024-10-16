#include "EH_set.h"
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#ifndef PROG_NAME
#define PROG_NAME "<undefined>"
#endif

#ifndef PROG_VERSION
#define PROG_VERSION "<undefined>"
#endif

using set = EH_set<unsigned>;

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
            std::cout << i << ": element not found";
        else {
            auto pos{it.get_pos()};
            std::cout << i << ": bucket: " << pos.first
                      << ", index: " << pos.second;
        }
        std::cout << '\n';
    }
}

bool cmd_option_exists(char **begin, char **end, const std::string &option) {
    return std::find(begin, end, option) != end;
}

void print_version() { std::cout << PROG_NAME << " " << PROG_VERSION << "\n"; }

void print_usage() {
    print_version();
    std::cout << "\nUsage:\n"
              << PROG_NAME << " [options]\n"
              << "\nOptions:\n"
              << "   -h     Print this help\n"
              << "   -v     Toggle verbose output. (default: true)\n"
              << "   -V     Print the program version\n";
}

int main(int argc, char **argv) {
    set set;
    bool changed;
    bool verbose{!cmd_option_exists(argv, argv + argc, "-v")};

    if (cmd_option_exists(argv, argv + argc, "-h")) {
        print_usage();
        return EXIT_SUCCESS;
    }
    if (cmd_option_exists(argv, argv + argc, "-V")) {
        print_version();
        return EXIT_SUCCESS;
    }

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
            for (unsigned i : set)
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

        if (verbose && changed)
            set.dump();
    }

    return EXIT_SUCCESS;
}
