#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <stdlib.h>


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
        "#ifndef FPP_GENERATED_IMG_RESOURCES_H\n"
        "#define FPP_GENERATED_IMG_RESOURCES_H\n"
        "\n"
        "#include <stddef.h>\n"
        "\n"
        "struct resource_image_s {\n"
        "    const char *name;\n"
        "    size_t      len;\n"
        "    const char *data;\n"
        "};\n"
        "\n"
        "extern struct resource_image_s resource_image[];\n"
        "extern size_t resource_image_count;\n"
        "\n"
        "#endif\n");
    fclose(fp_h);

    fprintf(fp_c,
        "// generated file, all changes will be lost\n"
        "#include \"%s\"\n"
        "\n", argv[1]);

    fprintf(fp_c, "size_t resource_image_count = %d;\n", argc - 3);
    fprintf(fp_c, "struct resource_image_s resource_image[] = {\n");
    for (int k = 3; k < argc; k ++) {
        unsigned char buf[4096];
        char *fname = strdup(argv[k]);
        char *bname = basename(fname);

        fprintf(fp_c, "{   .name = \"%s\",\n", bname);
        fprintf(fp_c, "    .data = \"");

        size_t fsize = 0;
        FILE *tmp = fopen(fname, "rb");
        if (!tmp) {
            printf("can't open %s\n", fname);
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
        fprintf(fp_c, "\",\n");
        fprintf(fp_c, "    .len = %u\n", (unsigned int)fsize);
        fprintf(fp_c, "},\n");
        free(fname);
    }
    fprintf(fp_c, "};\n");

    fclose(fp_c);
    return 0;

err_1:
    fclose(fp_c);
    return 3;
}
