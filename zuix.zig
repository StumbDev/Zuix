const std = @import("std");

const Variable = struct {
    name: []const u8,
    value: f64,
};

var gpa = std.heap.GeneralPurposeAllocator(.{}){};
const allocator = gpa.allocator();
var variables = std.ArrayList(Variable).init(allocator);

const FUNCTION_KEYS = struct {
    const F1 = 59;
    const F2 = 60;
    const F3 = 61;
    const F4 = 62;
    const F5 = 63;
    const F6 = 64;
    const F7 = 65;
    const F8 = 66;
    const F9 = 67;
    const F10 = 68;
};

fn clearScreen(writer: anytype) !void {
    try writer.writeAll("\x1B[2J\x1B[H");
}

fn showHelp(writer: anytype) !void {
    try writer.writeAll("\nZUIX-DOS Commands:\n");
    try writer.writeAll("HELP    - Show this help\n");
    try writer.writeAll("CLS     - Clear screen\n");
    try writer.writeAll("NAME    - Enter your name\n");
    try writer.writeAll("COUNT   - Count with delay\n");
    try writer.writeAll("VER     - Show version\n");
    try writer.writeAll("LET     - Assign value (LET A=10)\n");
    try writer.writeAll("PRINT   - Print value (PRINT A)\n");
    try writer.writeAll("RND     - Random number (RND 100)\n");
    try writer.writeAll("BEEP    - Make a beep sound\n");
    try writer.writeAll("LIST    - List variables\n");
    try writer.writeAll("NEW     - Clear all variables\n");
    try writer.writeAll("EXIT    - Exit to system\n");
    try writer.writeAll("\nFunction Keys:\n");
    try writer.writeAll("F1  - HELP\n");
    try writer.writeAll("F2  - LIST\n");
    try writer.writeAll("F3  - GOTO line\n");
    try writer.writeAll("F4  - RUN\n");
    try writer.writeAll("F5  - CONT\n");
    try writer.writeAll("F6  - LIST.\n");
    try writer.writeAll("F7  - TRON\n");
    try writer.writeAll("F8  - TROFF\n");
    try writer.writeAll("F9  - KEY\n");
    try writer.writeAll("F10 - SCREEN\n");
}

fn showVersion(writer: anytype) !void {
    try writer.writeAll("\nZUIX-DOS Version 1.0\n");
    try writer.writeAll("BASIC Mode Enhanced\n");
    try writer.writeAll("Copyright (C) 2024 ZUIX Computer Systems\n");
}

fn setVariable(name: []const u8, value: f64) !void {
    for (variables.items) |*var| {
        if (std.mem.eql(u8, var.name, name)) {
            var.value = value;
            return;
        }
    }
    try variables.append(.{ .name = name, .value = value });
}

fn getVariable(name: []const u8) f64 {
    for (variables.items) |var| {
        if (std.mem.eql(u8, var.name, name)) {
            return var.value;
        }
    }
    return 0;
}

fn handleFunctionKey(key: u8) []const u8 {
    return switch (key) {
        FUNCTION_KEYS.F1 => "HELP",
        FUNCTION_KEYS.F2 => "LIST",
        FUNCTION_KEYS.F3 => "GOTO",
        FUNCTION_KEYS.F4 => "RUN",
        FUNCTION_KEYS.F5 => "CONT",
        FUNCTION_KEYS.F6 => "LIST.",
        FUNCTION_KEYS.F7 => "TRON",
        FUNCTION_KEYS.F8 => "TROFF",
        FUNCTION_KEYS.F9 => "KEY",
        FUNCTION_KEYS.F10 => "SCREEN",
        else => "",
    };
}

pub fn main() !void {
    defer _ = gpa.deinit();
    const stdout = std.io.getStdOut().writer();
    const stdin = std.io.getStdIn().reader();
    var buffer: [100]u8 = undefined;
    var name_buffer: [100]u8 = undefined;
    var has_name = false;
    var prng = std.rand.DefaultPrng.init(blk: {
        var seed: u64 = undefined;
        try std.os.getrandom(std.mem.asBytes(&seed));
        break :blk seed;
    });
    var rand = prng.random();
    
    try clearScreen(stdout);
    try stdout.writeAll("ZUIX-DOS Version 1.0 - BASIC Mode\n");
    try stdout.writeAll("Memory Size: 64K\n");
    try stdout.writeAll("Enter HELP for commands\n\n");
    
    while (true) {
        try stdout.writeAll("READY.\nA> ");
        
        var command = std.ArrayList(u8).init(allocator);
        defer command.deinit();
        
        while (true) {
            const ch = try stdin.readByte();
            if (ch == '\n') break;
            
            if (ch == 0 or ch == 224) {
                const func_key = try stdin.readByte();
                const func_cmd = handleFunctionKey(func_key);
                if (func_cmd.len > 0) {
                    try command.appendSlice(func_cmd);
                    try stdout.writeAll(func_cmd);
                    break;
                }
            } else {
                try command.append(ch);
                try stdout.writeByte(ch);
            }
        }
        
        const cmd = command.items[0..command.items.len];
        
        if (std.mem.startsWith(u8, cmd, "LET ")) {
            // Process LET command
            if (std.mem.indexOf(u8, cmd, "=")) |pos| {
                const var_name = std.mem.trim(u8, cmd[4..pos], " ");
                const value_str = std.mem.trim(u8, cmd[pos+1..], " ");
                const value = std.fmt.parseFloat(f64, value_str) catch {
                    try stdout.writeAll("?SYNTAX ERROR\n");
                    continue;
                };
                try setVariable(var_name, value);
                try stdout.writeAll("OK\n");
            }
        } else if (std.mem.startsWith(u8, cmd, "PRINT ")) {
            const var_name = std.mem.trim(u8, cmd[6..], " ");
            const value = getVariable(var_name);
            try stdout.print("{d}\n", .{value});
        } else if (std.mem.startsWith(u8, cmd, "RND ")) {
            const max_str = std.mem.trim(u8, cmd[4..], " ");
            const max = std.fmt.parseInt(u32, max_str, 10) catch {
                try stdout.writeAll("?SYNTAX ERROR\n");
                continue;
            };
            const random_num = rand.intRangeAtMost(u32, 1, max);
            try stdout.print("{d}\n", .{random_num});
        }
        // ... (previous commands remain the same)
        else if (std.mem.eql(u8, cmd, "HELP")) {
            try showHelp(stdout);
        } else if (std.mem.eql(u8, cmd, "CLS")) {
            try clearScreen(stdout);
        } else if (std.mem.eql(u8, cmd, "VER")) {
            try showVersion(stdout);
        } else if (std.mem.eql(u8, cmd, "BEEP")) {
            try stdout.writeAll("\x07");
        } else if (std.mem.eql(u8, cmd, "LIST")) {
            if (variables.items.len == 0) {
                try stdout.writeAll("NO VARIABLES DEFINED\n");
            } else {
                for (variables.items) |var| {
                    try stdout.print("{s} = {d}\n", .{ var.name, var.value });
                }
            }
        } else if (std.mem.eql(u8, cmd, "NEW")) {
            variables.clearAndFree();
            try stdout.writeAll("OK\n");
        } else if (std.mem.eql(u8, cmd, "EXIT")) {
            try stdout.writeAll("SYSTEM HALTED\n");
            break;
        } else if (command.items.len > 0) {
            try stdout.writeAll("?SYNTAX ERROR\n");
        }
    }
    variables.deinit();
} 