#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        fprintf(stderr, "Error: Could not open file '%s'\n", argv[1]);
        return 1;
    }

    unsigned char buffer[BUFFER_SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        if (fwrite(buffer, 1, bytes_read, stdout) != bytes_read) {
            fprintf(stderr, "Error: Failed to write to stdout\n");
            fclose(file);
            return 1;
        }
    }

    if (ferror(file)) {
        fprintf(stderr, "Error: Failed to read from file '%s'\n", argv[1]);
        fclose(file);
        return 1;
    }

    fclose(file);
    return 0;
}