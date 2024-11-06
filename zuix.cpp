#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cctype>
#include <algorithm>
#include <vector>
#include <cmath>
#include <random>
#include <fstream>
#include <sstream>
#include <map>

#ifdef _WIN32
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
    
    char _getch() {
        char buf = 0;
        struct termios old = {0};
        if (tcgetattr(0, &old) < 0)
            perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0)
            perror("tcsetattr ICANON");
        if (read(0, &buf, 1) < 0)
            perror("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0)
            perror("tcsetattr ~ICANON");
        return buf;
    }
#endif

struct Variable {
    std::string name;
    double value;
    bool isString;
    std::string strValue;  // For string variables
};

std::vector<Variable> variables;

void clearScreen() {
    std::cout << "\x1B[2J\x1B[H";
}

void showHelp() {
    std::cout << "\nZUIX-DOS Commands:\n";
    std::cout << "HELP    - Show this help\n";
    std::cout << "CLS     - Clear screen\n";
    std::cout << "NAME    - Enter your name\n";
    std::cout << "COUNT   - Count with delay\n";
    std::cout << "VER     - Show version\n";
    std::cout << "LET     - Assign value (LET A=10)\n";
    std::cout << "PRINT   - Print value (PRINT A)\n";
    std::cout << "RND     - Random number (RND 100)\n";
    std::cout << "BEEP    - Make a beep sound\n";
    std::cout << "LOCATE  - Move cursor (LOCATE X,Y)\n";
    std::cout << "LIST    - List variables\n";
    std::cout << "NEW     - Clear all variables\n";
    std::cout << "EXIT    - Exit to system\n";
    std::cout << "GOTO    - Jump to line number\n";
    std::cout << "FOR     - FOR loop (FOR I=1 TO 10)\n";
    std::cout << "NEXT    - End FOR loop\n";
    std::cout << "IF      - Conditional (IF X=10 THEN)\n";
    std::cout << "REM     - Comment line\n";
    std::cout << "INPUT   - Input value\n";
    std::cout << "DATA    - Define data values\n";
    std::cout << "READ    - Read from DATA\n";
    std::cout << "RESTORE - Reset DATA pointer\n";
    std::cout << "END     - End program\n";
    std::cout << "\nFunction Keys:\n";
    std::cout << "F1  - HELP\n";
    std::cout << "F2  - LIST\n";
    std::cout << "F3  - GOTO line\n";
    std::cout << "F4  - RUN\n";
    std::cout << "F5  - CONT\n";
    std::cout << "F6  - LIST.\n";
    std::cout << "F7  - TRON\n";
    std::cout << "F8  - TROFF\n";
    std::cout << "F9  - KEY\n";
    std::cout << "F10 - SCREEN\n";
}

void showVersion() {
    std::cout << "\nZUIX-DOS Version 1.0\n";
    std::cout << "BASIC Mode Enhanced\n";
    std::cout << "Copyright (C) 2024 ZUIX Computer Systems\n";
}

void setVariable(const std::string& name, double value) {
    for (size_t i = 0; i < variables.size(); i++) {
        if (variables[i].name == name) {
            variables[i].value = value;
            return;
        }
    }
    Variable var;
    var.name = name;
    var.value = value;
    variables.push_back(var);
}

double getVariable(const std::string& name) {
    for (size_t i = 0; i < variables.size(); i++) {
        if (variables[i].name == name) {
            return variables[i].value;
        }
    }
    return 0.0;
}

void processLet(const std::string& cmd) {
    size_t pos = cmd.find('=');
    if (pos != std::string::npos) {
        std::string varName = cmd.substr(4, pos-4);
        std::string valueStr = cmd.substr(pos+1);
        try {
            double value = std::stod(valueStr);
            setVariable(varName, value);
            std::cout << "OK\n";
        } catch (...) {
            std::cout << "?SYNTAX ERROR\n";
        }
    }
}

std::string handleFunctionKey() {
    int ch = _getch();
    if (ch == 0 || ch == 224) { // Function key prefix
        ch = _getch();
        switch (ch) {
            case 59: return "HELP";    // F1
            case 60: return "LIST";    // F2
            case 61: return "GOTO";    // F3
            case 62: return "RUN";     // F4
            case 63: return "CONT";    // F5
            case 64: return "LIST.";   // F6
            case 65: return "TRON";    // F7
            case 66: return "TROFF";   // F8
            case 67: return "KEY";     // F9
            case 68: return "SCREEN";  // F10
            default: return "";
        }
    }
    return std::string(1, (char)ch);
}

struct Line {
    int number;
    std::string content;
};

struct ForLoop {
    std::string variable;
    double start;
    double end;
    double step;
    size_t returnLine;
};

std::vector<Line> program;
std::vector<std::string> dataValues;
size_t dataPointer = 0;
std::vector<ForLoop> forLoops;

void processInput(const std::string& cmd) {
    std::string varName = cmd.substr(6);
    std::cout << "? ";
    std::string value;
    std::getline(std::cin, value);
    try {
        double numValue = std::stod(value);
        setVariable(varName, numValue);
    } catch (...) {
        std::cout << "?REDO FROM START\n";
    }
}

void processData(const std::string& cmd) {
    std::string data = cmd.substr(5);
    std::string value;
    size_t pos = 0;
    while ((pos = data.find(',')) != std::string::npos) {
        value = data.substr(0, pos);
        dataValues.push_back(value);
        data = data.substr(pos + 1);
    }
    dataValues.push_back(data);
}

void processRead(const std::string& cmd) {
    std::string varName = cmd.substr(5);
    if (dataPointer >= dataValues.size()) {
        std::cout << "?OUT OF DATA\n";
        return;
    }
    try {
        double value = std::stod(dataValues[dataPointer++]);
        setVariable(varName, value);
    } catch (...) {
        std::cout << "?TYPE MISMATCH\n";
    }
}

void addProgramLine(const std::string& line) {
    try {
        // Find the first space to separate line number from content
        size_t spacePos = line.find(' ');
        if (spacePos == std::string::npos) return;
        
        int lineNum = std::stoi(line.substr(0, spacePos));
        std::string content = line.substr(spacePos + 1);
        
        // Store or replace the line
        bool found = false;
        for (auto& pLine : program) {
            if (pLine.number == lineNum) {
                pLine.content = content;
                found = true;
                break;
            }
        }
        if (!found) {
            program.push_back({lineNum, content});
            // Sort program by line numbers
            std::sort(program.begin(), program.end(), 
                     [](const Line& a, const Line& b) { return a.number < b.number; });
        }
        std::cout << "OK\n";
    } catch (...) {
        std::cout << "?SYNTAX ERROR\n";
    }
}

void processPrint(const std::string& cmd) {
    std::string content = cmd.substr(6);
    if (content.empty()) {
        std::cout << "\n";
        return;
    }
    
    if (content[0] == '"') {
        // Print string literal
        size_t endQuote = content.find('"', 1);
        if (endQuote != std::string::npos) {
            std::cout << content.substr(1, endQuote - 1) << "\n";
        }
    } else {
        // Check if it's a variable
        try {
            double value = getVariable(content);
            std::cout << value << "\n";
        } catch (...) {
            // If not a variable, print as literal text
            std::cout << content << "\n";
        }
    }
}

// Add these to the existing global variables
int currentLine = 0;
bool isRunning = false;

// Add this function for GOTO handling
void gotoLine(int lineNumber) {
    for (size_t i = 0; i < program.size(); i++) {
        if (program[i].number == lineNumber) {
            currentLine = i;
            return;
        }
    }
    std::cout << "?UNDEFINED LINE NUMBER\n";
}

// Add FOR loop handling
void processFor(const std::string& cmd) {
    // Format: FOR I = 1 TO 10 [STEP 2]
    size_t eqPos = cmd.find('=');
    size_t toPos = cmd.find("TO");
    if (eqPos == std::string::npos || toPos == std::string::npos) {
        std::cout << "?SYNTAX ERROR\n";
        return;
    }

    std::string var = cmd.substr(4, eqPos - 4);
    var = std::string(var.begin(), std::remove(var.begin(), var.end(), ' '));
    
    try {
        double start = std::stod(cmd.substr(eqPos + 1, toPos - eqPos - 1));
        size_t stepPos = cmd.find("STEP");
        double end, step = 1;
        
        if (stepPos != std::string::npos) {
            end = std::stod(cmd.substr(toPos + 2, stepPos - toPos - 2));
            step = std::stod(cmd.substr(stepPos + 4));
        } else {
            end = std::stod(cmd.substr(toPos + 2));
        }

        setVariable(var, start);
        ForLoop loop;
        loop.variable = var;
        loop.start = start;
        loop.end = end;
        loop.step = step;
        loop.returnLine = static_cast<size_t>(currentLine);
        forLoops.push_back(loop);
        
    } catch (...) {
        std::cout << "?SYNTAX ERROR\n";
    }
}

// Add NEXT handling
void processNext(const std::string& cmd) {
    if (forLoops.empty()) {
        std::cout << "?NEXT WITHOUT FOR\n";
        return;
    }

    ForLoop& loop = forLoops.back();
    double currentVal = getVariable(loop.variable);
    currentVal += loop.step;
    
    if ((loop.step > 0 && currentVal <= loop.end) || 
        (loop.step < 0 && currentVal >= loop.end)) {
        setVariable(loop.variable, currentVal);
        currentLine = loop.returnLine;
    } else {
        forLoops.pop_back();
    }
}

// Add these new structures and globals
struct StringVariable {
    std::string name;
    std::string value;
};

struct Array {
    std::string name;
    std::vector<double> values;
    int dimensions[3];  // Support up to 3D arrays
};

std::vector<StringVariable> stringVars;
std::vector<Array> arrays;
std::vector<size_t> gosubStack;  // For GOSUB/RETURN

// Add these new functions
void setStringVariable(const std::string& name, const std::string& value) {
    for (auto& var : stringVars) {
        if (var.name == name) {
            var.value = value;
            return;
        }
    }
    stringVars.push_back({name, value});
}

std::string getStringVariable(const std::string& name) {
    for (const auto& var : stringVars) {
        if (var.name == name) {
            return var.value;
        }
    }
    return "";
}

// Add array handling
void dimArray(const std::string& cmd) {
    // Format: DIM A(10) or DIM B(5,5)
    size_t start = cmd.find('(');
    size_t end = cmd.find(')');
    if (start == std::string::npos || end == std::string::npos) {
        std::cout << "?SYNTAX ERROR\n";
        return;
    }
    
    std::string name = cmd.substr(4, start-4);
    std::string dims = cmd.substr(start+1, end-start-1);
    
    Array arr;
    arr.name = name;
    
    // Parse dimensions
    int dimCount = 0;
    size_t pos = 0;
    while ((pos = dims.find(',')) != std::string::npos) {
        arr.dimensions[dimCount++] = std::stoi(dims.substr(0, pos));
        dims = dims.substr(pos + 1);
    }
    arr.dimensions[dimCount++] = std::stoi(dims);
    
    // Calculate total size
    int totalSize = 1;
    for (int i = 0; i < dimCount; i++) {
        totalSize *= arr.dimensions[i];
    }
    
    arr.values.resize(totalSize);
    arrays.push_back(arr);
}

// Add math functions
double evalMathFunction(const std::string& cmd) {
    if (cmd.substr(0, 4) == "SIN(") {
        return sin(std::stod(cmd.substr(4, cmd.length()-5)));
    }
    else if (cmd.substr(0, 4) == "COS(") {
        return cos(std::stod(cmd.substr(4, cmd.length()-5)));
    }
    else if (cmd.substr(0, 4) == "TAN(") {
        return tan(std::stod(cmd.substr(4, cmd.length()-5)));
    }
    else if (cmd.substr(0, 5) == "SQRT(") {
        return sqrt(std::stod(cmd.substr(5, cmd.length()-6)));
    }
    return 0.0;
}

// Add these string function declarations
std::string leftStr(const std::string& str, int len) {
    return str.substr(0, len);
}

std::string rightStr(const std::string& str, int len) {
    return str.substr(str.length() - len);
}

std::string midStr(const std::string& str, int start, int len) {
    return str.substr(start - 1, len);
}

// Add file operations
void saveProgram(const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        std::cout << "?CANNOT OPEN FILE\n";
        return;
    }
    for (const auto& line : program) {
        file << line.number << " " << line.content << "\n";
    }
    std::cout << "OK\n";
}

void loadProgram(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cout << "?FILE NOT FOUND\n";
        return;
    }
    program.clear();
    std::string line;
    while (std::getline(file, line)) {
        addProgramLine(line);
    }
    std::cout << "OK\n";
}

// Add graphics commands (using ASCII art for now)
void drawLine(int x1, int y1, int x2, int y2) {
    // Simple ASCII line drawing
    std::cout << "Drawing line from (" << x1 << "," << y1 
              << ") to (" << x2 << "," << y2 << ")\n";
    std::cout << "*****\n";
}

void drawCircle(int x, int y, int radius) {
    std::cout << "Drawing circle at (" << x << "," << y 
              << ") with radius " << radius << "\n";
    std::cout << " *** \n";
    std::cout << "*   *\n";
    std::cout << " *** \n";
}

// Add sound functions
void playSound(int frequency, int duration) {
    std::cout << "\a"; // Simple beep for now
    std::this_thread::sleep_for(std::chrono::milliseconds(duration));
}

void playNote(const std::string& note) {
    // Basic musical note mapping
    std::map<std::string, int> notes = {
        {"C", 262}, {"D", 294}, {"E", 330},
        {"F", 349}, {"G", 392}, {"A", 440}, {"B", 494}
    };
    if (notes.find(note) != notes.end()) {
        playSound(notes[note], 500);
    }
}

// Add more math functions
double evalAdvancedMath(const std::string& cmd) {
    if (cmd.substr(0, 4) == "LOG(") {
        return log(std::stod(cmd.substr(4, cmd.length()-5)));
    }
    else if (cmd.substr(0, 4) == "EXP(") {
        return exp(std::stod(cmd.substr(4, cmd.length()-5)));
    }
    else if (cmd.substr(0, 4) == "ABS(") {
        return abs(std::stod(cmd.substr(4, cmd.length()-5)));
    }
    else if (cmd.substr(0, 4) == "INT(") {
        return floor(std::stod(cmd.substr(4, cmd.length()-5)));
    }
    return evalMathFunction(cmd); // Call original math functions
}

// Modify runProgram to include new commands
void runProgram() {
    if (program.empty()) {
        std::cout << "NO PROGRAM\n";
        return;
    }

    isRunning = true;
    currentLine = 0;
    forLoops.clear();

    while (currentLine < program.size() && isRunning) {
        std::string cmd = program[currentLine].content;
        
        if (cmd.substr(0, 6) == "PRINT ") {
            processPrint(cmd);
        }
        else if (cmd.substr(0, 4) == "LET ") {
            processLet(cmd);
        }
        else if (cmd.substr(0, 5) == "GOTO ") {
            try {
                int lineNum = std::stoi(cmd.substr(5));
                gotoLine(lineNum);
                continue;
            } catch (...) {
                std::cout << "?SYNTAX ERROR\n";
                break;
            }
        }
        else if (cmd.substr(0, 4) == "FOR ") {
            processFor(cmd);
        }
        else if (cmd.substr(0, 5) == "NEXT ") {
            processNext(cmd);
            continue;  // Skip currentLine increment if loop continues
        }
        else if (cmd.substr(0, 6) == "INPUT ") {
            processInput(cmd);
        }
        else if (cmd.substr(0, 6) == "GOSUB ") {
            try {
                int lineNum = std::stoi(cmd.substr(6));
                gosubStack.push_back(currentLine);
                gotoLine(lineNum);
                continue;
            } catch (...) {
                std::cout << "?SYNTAX ERROR\n";
            }
        }
        else if (cmd == "RETURN") {
            if (gosubStack.empty()) {
                std::cout << "?RETURN WITHOUT GOSUB\n";
                return;
            }
            currentLine = gosubStack.back();
            gosubStack.pop_back();
        }
        else if (cmd.substr(0, 4) == "DIM ") {
            dimArray(cmd);
        }
        else if (cmd.substr(0, 5) == "SAVE ") {
            saveProgram(cmd.substr(5));
        }
        else if (cmd.substr(0, 5) == "LOAD ") {
            loadProgram(cmd.substr(5));
        }
        else if (cmd.substr(0, 5) == "LINE ") {
            // Parse coordinates and call drawLine
            // Format: LINE x1,y1,x2,y2
            std::stringstream ss(cmd.substr(5));
            int x1, y1, x2, y2;
            char comma;
            ss >> x1 >> comma >> y1 >> comma >> x2 >> comma >> y2;
            drawLine(x1, y1, x2, y2);
        }
        else if (cmd.substr(0, 7) == "CIRCLE ") {
            // Parse parameters and call drawCircle
            // Format: CIRCLE x,y,radius
            std::stringstream ss(cmd.substr(7));
            int x, y, radius;
            char comma;
            ss >> x >> comma >> y >> comma >> radius;
            drawCircle(x, y, radius);
        }
        else if (cmd.substr(0, 5) == "PLAY ") {
            // Format: PLAY "CDEFGAB"
            std::string notes = cmd.substr(5);
            for (char note : notes) {
                if (note != ' ') playNote(std::string(1, note));
            }
        }
        
        currentLine++;
    }
    
    isRunning = false;
}

int main() {
    std::string command;
    std::string name;
    bool running = true;
    std::random_device rd;
    std::mt19937 gen(rd());
    
    clearScreen();
    std::cout << "ZUIX-DOS Version 1.0 - BASIC Mode\n";
    std::cout << "Memory Size: 64K\n";
    std::cout << "Enter HELP for commands\n\n";
    
    while (running) {
        std::cout << "READY.\nA> ";
        
        std::getline(std::cin, command);
        
        // Convert command to uppercase before processing
        std::transform(command.begin(), command.end(), command.begin(), ::toupper);
        
        if (!command.empty() && std::isdigit(command[0])) {
            addProgramLine(command);
        }
        else if (command == "HELP") {
            showHelp();
        }
        else if (command == "RUN") {
            runProgram();
        }
        else if (command == "LIST") {
            for (const auto& line : program) {
                std::cout << line.number << " " << line.content << "\n";
            }
        }
        else if (command.substr(0, 6) == "PRINT ") {
            processPrint(command);
        }
        else if (command.substr(0, 4) == "RND ") {
            try {
                int max = std::stoi(command.substr(4));
                std::uniform_int_distribution<> dis(1, max);
                std::cout << dis(gen) << "\n";
            } catch (...) {
                std::cout << "?SYNTAX ERROR\n";
            }
        }
        else if (command == "BEEP") {
            std::cout << "\a";
        }
        else if (command == "NAME") {
            std::cout << "ENTER YOUR NAME: ";
            std::getline(std::cin, name);
            std::cout << "\nHELLO, " << name << "!\n";
        }
        else if (command == "COUNT") {
            if (name.empty()) {
                std::cout << "ERROR: PLEASE USE NAME COMMAND FIRST\n";
                continue;
            }
            std::cout << "\nCOUNTING FOR " << name << ":\n";
            for (int i = 1; i <= 5; i++) {
                std::cout << i << "\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }
        else if (command == "EXIT") {
            std::cout << "SYSTEM HALTED\n";
            running = false;
        }
        else if (command.substr(0, 6) == "INPUT ") {
            processInput(command);
        }
        else if (command.substr(0, 5) == "DATA ") {
            processData(command);
        }
        else if (command.substr(0, 5) == "READ ") {
            processRead(command);
        }
        else if (command == "RESTORE") {
            dataPointer = 0;
            std::cout << "OK\n";
        }
        else if (command.substr(0, 4) == "REM ") {
            // Just ignore the rest of the line
            std::cout << "OK\n";
        }
        else if (command.substr(0, 3) == "IF ") {
            // Basic IF statement processing
            size_t thenPos = command.find("THEN");
            if (thenPos != std::string::npos) {
                std::string condition = command.substr(3, thenPos - 3);
                // Very basic condition processing (just equals for now)
                size_t eqPos = condition.find('=');
                if (eqPos != std::string::npos) {
                    std::string var = condition.substr(0, eqPos);
                    double value = std::stod(condition.substr(eqPos + 1));
                    if (getVariable(var) == value) {
                        std::string thenCmd = command.substr(thenPos + 4);
                        // Process the THEN command
                        // (This is a simplified version - you might want to expand it)
                        std::cout << "CONDITION TRUE\n";
                    }
                }
            }
        }
        else if (!command.empty()) {
            std::cout << "?SYNTAX ERROR\n";
        }
    }
    return 0;
} 