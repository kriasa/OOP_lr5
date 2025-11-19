#include <iostream>
#include <string>
#include <limits>
#include "pmr_stack.h"

void print_help() {

    std::cout << "push\n";
    std::cout << "pop\n";
    std::cout << "top\n";
    std::cout << "all\n";
    std::cout << "clear\n";
    std::cout << "exit\n";
}

int main() {

    size_t buffer_size = 1024;

    FixedBlockVectorResource res(buffer_size);
    PmrStack<int> stack(&res);
    print_help();

    std::string command;

    while (true) {
        std::cout << "\n> ";
        std::cin >> command;

        if (command == "exit") {
            break;
        }
        else if (command == "push") {
            int val;
            if (std::cin >> val) {
                try {
                    stack.push(val);
                } catch (const std::bad_alloc&) {
                    std::cerr << "ERROR - not enough memory" << std::endl;
                }
            } else {
                std::cerr << "ERROR - invalid number input" << std::endl;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
        else if (command == "pop") {
            if (!stack.empty()) {
                stack.pop();
            } else {
                std::cout << "stack is empty" << std::endl;
            }
        }
        else if (command == "top") {
            if (!stack.empty()) {
                std::cout << "top element:" << stack.top() << std::endl;
            } else {
                std::cout << "stack is empty." << std::endl;
            }
        }
        else if (command == "all") {
            if (stack.empty()) {
                std::cout << "stack is empty" << std::endl;
            } else {
                std::cout << "stack content:";
                for (int val : stack) {
                    std::cout << val << " ";
                }
                std::cout << std::endl;
            }
        }
        else if (command == "clear") {
            stack.clear();
            std::cout << "stack cleared" << std::endl;
        }
        else {
            std::cout << "ERROR" << std::endl;
        }
    }

    return 0;
}