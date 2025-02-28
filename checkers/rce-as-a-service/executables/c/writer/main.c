#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Convert a hex character to its integer value
int hex_to_int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

// Decode a hex string into bytes
size_t hex_decode(const char *src, unsigned char *dst) {
    size_t len = strlen(src);
    size_t final_len = 0;

    for (size_t i = 0; i < len; i += 2) {
        if (src[i] == '\0' || src[i + 1] == '\0') break;

        int high = hex_to_int(src[i]);
        int low = hex_to_int(src[i + 1]);

        if (high == -1 || low == -1) return 0;

        dst[final_len++] = (high << 4) | low;
    }

    return final_len;
}

// XOR two byte arrays up to the length of the shorter one
size_t xor_arrays(const unsigned char *a, size_t a_len,
                  const unsigned char *b, size_t b_len,
                  unsigned char *result) {
    size_t min_len = a_len < b_len ? a_len : b_len;
    for (size_t i = 0; i < min_len; i++) {
        result[i] = a[i] ^ b[i];
    }
    return min_len;
}

int main(int argc, char *argv[]) {
    if (argc != 4) return 1;

    // Allocate buffers for decoded hex strings
    size_t max_len = strlen(argv[1]);
    if (strlen(argv[2]) > max_len) max_len = strlen(argv[2]);
    if (strlen(argv[3]) > max_len) max_len = strlen(argv[3]);
    max_len = (max_len + 1) / 2;

    unsigned char *decoded1 = malloc(max_len);
    unsigned char *decoded2 = malloc(max_len);
    unsigned char *decoded3 = malloc(max_len);
    unsigned char *filename = malloc(max_len);
    if (!decoded1 || !decoded2 || !decoded3 || !filename) return 1;

    // Decode all three arguments
    size_t len1 = hex_decode(argv[1], decoded1);
    size_t len2 = hex_decode(argv[2], decoded2);
    size_t len3 = hex_decode(argv[3], decoded3);
    if (!len1 || !len2 || !len3) goto cleanup;

    // XOR first two arguments to get filename
    size_t filename_len = xor_arrays(decoded1, len1, decoded2, len2, filename);
    if (!filename_len) goto cleanup;

    // Ensure filename is null-terminated
    filename[filename_len] = '\0';

    // Write decoded third argument to file
    FILE *f = fopen((char *)filename, "wb");
    if (!f) goto cleanup;
    fwrite(decoded3, 1, len3, f);
    fclose(f);

    // XOR filename with first argument and output to stdout
    unsigned char *stdout_result = malloc(max_len);
    if (!stdout_result) goto cleanup;
    size_t stdout_len = xor_arrays(filename, filename_len, decoded1, len1, stdout_result);
    fwrite(stdout_result, 1, stdout_len, stdout);
    free(stdout_result);

    // XOR filename with second argument and output to stderr
    unsigned char *stderr_result = malloc(max_len);
    if (!stderr_result) goto cleanup;
    size_t stderr_len = xor_arrays(filename, filename_len, decoded2, len2, stderr_result);
    fwrite(stderr_result, 1, stderr_len, stderr);
    free(stderr_result);

cleanup:
    free(decoded1);
    free(decoded2);
    free(decoded3);
    free(filename);
    return 0;
}