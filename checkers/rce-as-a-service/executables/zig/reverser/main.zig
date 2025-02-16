const std = @import("std");

fn decodeBase64(allocator: std.mem.Allocator, input: []const u8) ![]u8 {
    var output = std.ArrayList(u8).init(allocator);
    errdefer output.deinit();

    var buf: u32 = 0;
    var bits: u5 = 0;  // Changed to u5 as it's sufficient for our needs (0-31)
    var padding: u8 = 0;

    for (input) |c| {
        if (c == '=') {
            padding += 1;
            continue;
        }

        const val: u32 = switch (c) {
            'A'...'Z' => c - 'A',
            'a'...'z' => c - 'a' + 26,
            '0'...'9' => c - '0' + 52,
            '+' => 62,
            '/' => 63,
            else => continue,
        };

        buf = (buf << 6) | val;
        bits = @truncate(bits + 6);

        if (bits >= 8) {
            bits = @truncate(bits - 8);
            try output.append(@truncate((buf >> @as(u5, bits)) & 0xFF));
        }
    }

    // Handle remaining bits based on padding
    if (padding == 1 and bits >= 2) {
        bits = @truncate(bits - 2);
        try output.append(@truncate((buf >> @as(u5, bits)) & 0xFF));
    } else if (padding == 2 and bits >= 4) {
        bits = @truncate(bits - 4);
        try output.append(@truncate((buf >> @as(u5, bits)) & 0xFF));
    }

    return output.toOwnedSlice();
}

fn reverseString(allocator: std.mem.Allocator, input: []const u8) ![]u8 {
    var output = try allocator.alloc(u8, input.len);
    var i: usize = 0;
    while (i < input.len) : (i += 1) {
        output[input.len - 1 - i] = input[i];
    }
    return output;
}

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    const args = try std.process.argsAlloc(allocator);
    defer std.process.argsFree(allocator, args);

    if (args.len != 2) {
        try std.io.getStdErr().writer().print("Usage: {s} <base64_string>\n", .{args[0]});
        std.process.exit(1);
    }

    const decoded = decodeBase64(allocator, args[1]) catch |err| {
        try std.io.getStdErr().writer().print("Error decoding base64: {}\n", .{err});
        std.process.exit(1);
    };
    defer allocator.free(decoded);

    const reversed = try reverseString(allocator, decoded);
    defer allocator.free(reversed);

    try std.io.getStdOut().writer().writeAll(reversed);
}
