#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <stdlib.h>


static
void
replace_dots(char *s)
{
    while (*s) {
        if (*s == '.')
            *s = '_';
        s++;
    }
}


int
main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("not enough arguments\n");
        return 1;
    }

    FILE *fp_h = fopen(argv[1], "w");
    if (!fp_h) {
        printf("can't open %s\n", argv[1]);
        return 2;
    }

    FILE *fp_c = fopen(argv[2], "w");
    if (!fp_c) {
        fclose(fp_h);
        printf("can't open %s\n", argv[2]);
        return 2;
    }

    fprintf(fp_h,
        "// generated file, all changes will be lost\n"
        "#ifndef FPP_GENERATED_TXT_RESOURCES_H\n"
        "#define FPP_GENERATED_TXT_RESOURCES_H\n"
        "\n"
        "\n");
    for (int k = 3; k < argc; k ++) {
        char *fname = strdup(argv[k]);
        char *bname = basename(fname);

        replace_dots(bname);
        fprintf(fp_h, "extern const char *resource_text_%s;\n", bname);
        free(fname);
    }
    fprintf(fp_h, "\n\n#endif\n");
    fclose(fp_h);

    fprintf(fp_c,
        "// generated file, all changes will be lost\n"
        "#include \"%s\"\n"
        "\n", argv[1]);

    for (int k = 3; k < argc; k ++) {
        unsigned char buf[4096];
        char *fname = strdup(argv[k]);
        char *bname = basename(fname);

        replace_dots(bname);
        fprintf(fp_c, "const char *resource_text_%s = \"", bname);

        size_t fsize = 0;
        FILE *tmp = fopen(argv[k], "rb");
        if (!tmp) {
            printf("can't open %s\n", argv[k]);
            free(fname);
            goto err_1;
        }
        while (!feof(tmp)) {
            size_t read_bytes = fread(buf, 1, sizeof(buf), tmp);
            for (size_t j = 0; j < read_bytes; j ++)
                fprintf(fp_c, "\\x%02x", buf[j]);
            fsize += read_bytes;
        }
        fclose(tmp);
        fprintf(fp_c, "\";\n");
        free(fname);
    }

    fclose(fp_c);
    return 0;

err_1:
    fclose(fp_c);
    return 3;
}
