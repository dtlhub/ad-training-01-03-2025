const std = @import("std");

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    const args = try std.process.argsAlloc(allocator);
    defer std.process.argsFree(allocator, args);

    if (args.len > 1) {
        for (args[1..], 0..) |arg, i| {
            try std.io.getStdOut().writer().print("{s}", .{arg});
            if (i < args.len - 2) {
                try std.io.getStdOut().writer().print(" ", .{});
            }
        }
    }
}