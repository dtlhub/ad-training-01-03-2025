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
    size_t i = 0, j = 0;
    unsigned char a, b, c, d;
    unsigned char tmp[4];
    int padding = 0;

    while (i < src_len) {
        // Skip whitespace
        if (src[i] == '\n' || src[i] == '\r' || src[i] == ' ') {
            i++;
            continue;
        }

        // Count padding
        if (src[i] == '=') {
            padding++;
            i++;
            continue;
        }

        // Get first byte
        if (i >= src_len) break;
        tmp[0] = base64_reverse_table[(unsigned char)src[i++]];
        if (tmp[0] == 0xff) continue;

        // Get second byte
        if (i >= src_len) break;
        tmp[1] = base64_reverse_table[(unsigned char)src[i++]];
        if (tmp[1] == 0xff) continue;

        // Get third byte
        if (i >= src_len) break;
        tmp[2] = base64_reverse_table[(unsigned char)src[i++]];
        if (tmp[2] == 0xff) continue;

        // Get fourth byte
        if (i >= src_len) break;
        tmp[3] = base64_reverse_table[(unsigned char)src[i++]];
        if (tmp[3] == 0xff) continue;

        a = tmp[0];
        b = tmp[1];
        c = tmp[2];
        d = tmp[3];

        // Write output bytes
        dst[j++] = (a << 2) | (b >> 4);
        if (padding < 2) {
            dst[j++] = (b << 4) | (c >> 2);
            if (padding < 1) {
                dst[j++] = (c << 6) | d;
            }
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