#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define OPEN_FLAGS_READ_BINARY "rb"
#else
    #define OPEN_FLAGS_READ_BINARY "r"
#endif

char *file_name(char *path) {
    char *p = path;
    char *name = p;
    while (*p != '\0') {
        if (*p == '/')
            name = p + 1;
        p++;
    }
    return name;
}

char *do_file(FILE *outf, char *symbol_prefix, char *path)
{
    FILE *f = fopen(path, OPEN_FLAGS_READ_BINARY);
    if (f == NULL) {
        printf("fopen() failed for \"%s\"", path);
        return NULL;
    }

    char *p = path;
    char *name = p;
    while (*p != '\0') {
        if (*p == '/')
            name = p + 1;
        else if (*p == '.')
            *p = '\0';
        p++;
    }

    size_t len = 0;
    uint8_t b;
    fprintf(outf, "\nuint8_t %s_%s[] = {", symbol_prefix, name);
    while (fread(&b, sizeof(b), 1, f)) {
        fprintf(outf, "0x%x,", b);
        len++;
    }
    fprintf(outf, "};");
    fclose(f);
    return name;
}

int main(int argc, char *argv[])
{
    if (argc < 4) {
        printf("USAGE: resgen OUT_H OUT_C SYMBOL_PREFIX FILES ...");
        exit(1);
    }
    char *out_h = argv[1];
    char *out_c = argv[2];
    char *symbol_prefix = argv[3];

    char *names[argc - 4];

    FILE *out_c_file = fopen(out_c, "w");
    if (out_c_file == NULL) {
        printf("fopen() failed for \"%s\"", out_c);
        exit(1);
    }
    for (int i = 4; i < argc; i++) {
        char *path = argv[i];
        names[i - 4] = do_file(out_c_file, symbol_prefix, path);
    }
    fclose(out_c_file);

    FILE *out_h_file = fopen(out_h, "w");
    if (out_h_file == NULL) {
        printf("fopen() failed for \"%s\"", out_h);
        exit(1);
    }
    fprintf(out_h_file, "#ifndef %s_H\n", symbol_prefix);
    fprintf(out_h_file, "#define %s_H\n", symbol_prefix);
    fprintf(out_h_file, "#define FOR_EACH_%s(m)", symbol_prefix);
    for (int i = 0; i < argc - 4; i++) {
        if (names[i] != NULL)
            fprintf(out_h_file, "\\\nm(%s)", names[i]);
    }
    fprintf(out_h_file, "\n#endif\n");
    fclose(out_h_file);

    return 0;
}