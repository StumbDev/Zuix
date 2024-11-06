#include <iostream>
#include <string>
#include <thread>
#include <chrono>

int main() {
    std::string name;
    
    // Print welcome message (like BASIC's PRINT)
    std::cout << "BASIC-LIKE PROGRAM IN C++\n";
    
    // Input name (like BASIC's INPUT)
    std::cout << "WHAT IS YOUR NAME? ";
    std::getline(std::cin, name);
    
    // FOR loop (like BASIC's FOR/NEXT)
    std::cout << "\nCOUNTING FOR " << name << ":\n";
    for (int i = 1; i <= 5; i++) {
        std::cout << i << "\n";
        // Simulate BASIC's delay
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    // IF/THEN style condition
    if (name.length() > 5) {
        std::cout << "\n" << name << " IS A LONG NAME!\n";
    } else {
        std::cout << "\n" << name << " IS A SHORT NAME!\n";
    }
    
    return 0;
} 