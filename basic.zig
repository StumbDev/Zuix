const std = @import("std");

pub fn main() !void {
    const stdout = std.io.getStdOut().writer();
    var buffer: [100]u8 = undefined;
    
    // Print welcome message (like BASIC's PRINT)
    try stdout.print("BASIC-LIKE PROGRAM IN ZIG\n", .{});
    
    // Input name (like BASIC's INPUT)
    try stdout.print("WHAT IS YOUR NAME? ", .{});
    const stdin = std.io.getStdIn().reader();
    if (try stdin.readUntilDelimiter(&buffer, '\n')) |name| {
        // FOR loop (like BASIC's FOR/NEXT)
        try stdout.print("\nCOUNTING FOR {s}:\n", .{name});
        var i: i32 = 1;
        while (i <= 5) : (i += 1) {
            try stdout.print("{d}\n", .{i});
            // Simulate BASIC's delay
            std.time.sleep(500 * 1000 * 1000);
        }
        
        // IF/THEN style condition
        if (name.len > 5) {
            try stdout.print("\n{s} IS A LONG NAME!\n", .{name});
        } else {
            try stdout.print("\n{s} IS A SHORT NAME!\n", .{name});
        }
    }
} 