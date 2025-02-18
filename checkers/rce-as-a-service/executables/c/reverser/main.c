#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const unsigned char base64_table[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const unsigned char base64_reverse_table[256] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0x3e, 0xff, 0xff, 0xff, 0x3f,
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b,
    0x3c, 0x3d, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
    0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
    0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
    0x17, 0x18, 0x19, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20,
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30,
    0x31, 0x32, 0x33, 0xff, 0xff, 0xff, 0xff, 0xff,
};

size_t base64_decode(const char *src, size_t src_len, unsigned char *dst) {
    size_t j = 0;
    int group[4], count = 0, pad = 0;

    // Process input in groups of 4 characters.
    for (size_t i = 0; i < src_len; i++) {
        char ch = src[i];

        // Skip whitespace characters.
        if (ch == '\n' || ch == '\r' || ch == ' ')
            continue;

        if (ch == '=') {
            // For padding, store 0 and count the padding.
            group[count++] = 0;
            pad++;
        } else {
            // If padding has already been encountered, further non-padding char is invalid.
            if (pad > 0) {
                // You may choose to handle this as an error;
                // here we simply break out.
                break;
            }
            unsigned char val = base64_reverse_table[(unsigned char) ch];
            if (val == 0xff) {
                // Unknown character; skip it or you could treat it as an error.
                continue;
            }
            group[count++] = val;
        }

        // Process a complete group of 4.
        if (count == 4) {
            dst[j++] = (group[0] << 2) | (group[1] >> 4);
            if (pad < 2) {
                dst[j++] = (group[1] << 4) | (group[2] >> 2);
            }
            if (pad == 0) {
                dst[j++] = (group[2] << 6) | group[3];
            }
            // Reset for the next group.
            count = 0;
            pad = 0;
        }
    }
    return j;
}

void reverse_string(char *str, size_t len) {
    size_t i;
    char temp;
    for (i = 0; i < len / 2; i++) {
        temp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = temp;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <base64_string>\n", argv[0]);
        return 1;
    }

    size_t input_len = strlen(argv[1]);
    unsigned char *decoded = malloc(input_len);
    if (!decoded) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    size_t decoded_len = base64_decode(argv[1], input_len, decoded);
    reverse_string((char *)decoded, decoded_len);

    fwrite(decoded, 1, decoded_len, stdout);
    free(decoded);
    return 0;
}