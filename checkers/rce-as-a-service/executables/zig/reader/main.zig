const std = @import("std");

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    const args = try std.process.argsAlloc(allocator);
    defer std.process.argsFree(allocator, args);

    if (args.len != 2) {
        try std.io.getStdErr().writer().print("Usage: {s} <filename>\n", .{args[0]});
        std.process.exit(1);
    }

    const file = std.fs.cwd().openFile(args[1], .{}) catch |err| {
        try std.io.getStdErr().writer().print("Error: Could not open file '{s}': {}\n", .{ args[1], err });
        std.process.exit(1);
    };
    defer file.close();

    const contents = file.readToEndAlloc(allocator, std.math.maxInt(usize)) catch |err| {
        try std.io.getStdErr().writer().print("Error: Failed to read from file '{s}': {}\n", .{ args[1], err });
        std.process.exit(1);
    };
    defer allocator.free(contents);

    try std.io.getStdOut().writer().writeAll(contents);
}
