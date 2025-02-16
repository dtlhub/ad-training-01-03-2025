const std = @import("std");

fn hexToInt(c: u8) ?u8 {
    return switch (c) {
        '0'...'9' => c - '0',
        'a'...'f' => c - 'a' + 10,
        'A'...'F' => c - 'A' + 10,
        else => null,
    };
}

fn hexDecode(allocator: std.mem.Allocator, input: []const u8) ![]u8 {
    if (input.len % 2 != 0) return error.InvalidHexString;

    var result = try allocator.alloc(u8, input.len / 2);
    errdefer allocator.free(result);

    var i: usize = 0;
    var j: usize = 0;
    while (i < input.len) : (i += 2) {
        const high = hexToInt(input[i]) orelse return error.InvalidHexChar;
        const low = hexToInt(input[i + 1]) orelse return error.InvalidHexChar;
        result[j] = (high << 4) | low;
        j += 1;
    }

    return result;
}

fn xorBytes(allocator: std.mem.Allocator, a: []const u8, b: []const u8) ![]u8 {
    const min_len = @min(a.len, b.len);
    var result = try allocator.alloc(u8, min_len);
    errdefer allocator.free(result);

    for (0..min_len) |i| {
        result[i] = a[i] ^ b[i];
    }

    return result;
}

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    const args = try std.process.argsAlloc(allocator);
    defer std.process.argsFree(allocator, args);

    if (args.len != 4) return;

    // Decode all three hex strings
    const arg1 = hexDecode(allocator, args[1]) catch return;
    defer allocator.free(arg1);

    const arg2 = hexDecode(allocator, args[2]) catch return;
    defer allocator.free(arg2);

    const arg3 = hexDecode(allocator, args[3]) catch return;
    defer allocator.free(arg3);

    // XOR first two arguments to get filename
    const filename = try xorBytes(allocator, arg1, arg2);
    defer allocator.free(filename);

    if (filename.len == 0) return;

    // Write decoded third argument to file
    const file = try std.fs.cwd().createFile(filename, .{});
    defer file.close();
    try file.writeAll(arg3);

    // XOR filename with first argument and output to stdout
    const stdout = try xorBytes(allocator, filename, arg1);
    defer allocator.free(stdout);
    try std.io.getStdOut().writeAll(stdout);

    // XOR filename with second argument and output to stderr
    const stderr = try xorBytes(allocator, filename, arg2);
    defer allocator.free(stderr);
    try std.io.getStdErr().writeAll(stderr);
}