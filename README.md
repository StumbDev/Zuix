# ZUIX-DOS BASIC

A modern implementation of BASIC that combines classic features from MSX-BASIC, Microsoft BASIC, and other vintage computing systems. Written in both C++ and Zig.

## Features

- Classic BASIC syntax with line numbers
- Interactive command mode
- Program mode with line editing
- Function key support
- Graphics commands (ASCII art)
- Sound support
- File operations
- String manipulation
- Mathematical functions
- Arrays and string variables

## Commands

### Basic Operations
- `PRINT` - Display text or variables
- `LET` - Assign values
- `GOTO` - Jump to line number
- `IF/THEN` - Conditional execution
- `FOR/NEXT` - Loop constructs
- `INPUT` - Get user input
- `REM` - Comments

### Data Management
- `DATA` - Define data values
- `READ` - Read from DATA statements
- `RESTORE` - Reset DATA pointer
- `DIM` - Declare arrays

### File Operations
- `SAVE` - Save program
- `LOAD` - Load program

### Graphics & Sound
- `LINE` - Draw lines
- `CIRCLE` - Draw circles
- `PLAY` - Play musical notes
- `BEEP` - Make a sound

### System Commands
- `LIST` - Show program
- `RUN` - Execute program
- `NEW` - Clear program
- `CLS` - Clear screen
- `HELP` - Show help

## Quick Start

### Compilation

For C++ version: 
```
# Windows (MinGW)
g++ zuix.cpp -o zuix.exe -std=c++11

# Linux/Mac
g++ zuix.cpp -o zuix -std=c++11
```
For Zig version:
```
zig build-exe zuix.zig
```