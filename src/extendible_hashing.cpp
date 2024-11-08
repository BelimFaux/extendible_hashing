#include "EH_set.h"
#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include <sstream>
#include <string>

// These options should be defined by the cmake file
#ifndef PROG_NAME
#define PROG_NAME "<undefined>"
#endif

#ifndef PROG_VERSION
#define PROG_VERSION "<undefined>"
#endif

#ifndef PROG_DESC
#define PROG_DESC "<undefined>"
#endif

using set = EH_set<unsigned>;

void print_version() { std::cout << PROG_NAME << " " << PROG_VERSION << "\n"; }

void print_cli_help() {
    print_version();
    std::cout << PROG_DESC << "\n\nUSAGE:\n"
              << PROG_NAME << " [OPTIONS]\n"
              << "\nOPTIONS:\n"
              << "   -h, --help      Print this help\n"
              << "   -v, --verbose   Toggle verbose output. (default: true)\n"
              << "   -V, --version   Print the program version\n\n"
              << "After Startup, you can enter commands, to manipulate the "
                 "datastructure.\n"
              << "For more Information on the available commands run the "
                 "command 'h'.\n";
}

void print_help() {
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
}

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

void run(bool verbose) {
    set set{};
    bool changed{false};
    std::string line{""};

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
            print_help();
            break;
        case 'q':
            return;
        default:
            std::cout << "unknown command\n";
        }

        if (verbose && changed)
            set.dump();
    }
}

int main(int argc, char **argv) {
    int verbose;
    static const struct option long_options[] = {
        {"verbose", no_argument, nullptr, 'v'},
        {"version", no_argument, nullptr, 'V'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr, 0}};

    int opt{'?'};
    while ((opt = getopt_long(argc, argv, "vhV", long_options, nullptr)) !=
           -1) {
        switch (opt) {
        case 0:
            break;
        case 'v':
            verbose = false;
            break;
        case 'V':
            print_version();
            return EXIT_SUCCESS;
        case 'h':
            print_cli_help();
            return EXIT_SUCCESS;
        case '?':
            std::cerr << "Try '" PROG_NAME " --help' for more information.\n";
            return EXIT_FAILURE;
        default:
            std::cerr << "Error while parsing Arguments\n";
            return EXIT_FAILURE;
        }
    }

    run(verbose);

    return EXIT_SUCCESS;
}
