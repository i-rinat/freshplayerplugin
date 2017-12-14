#include "nih_test.h"
#include <math.h>
#include <src/config_parser/config_parser.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TEST(config_parser, test_1)
{
    ASSERT_TRUE((void*)1);

    const char *s =
        "param1= value\n"                   // simple key-value
        "param2 =  123\r\n"                 // number value; different line ending
        "unknownkey = unknownvalue\n"       // unknown key
        "param3   =qwe";                    // no newline character at the end of a file

    char *value1 = strdup("");
    int   value2 = 0;
    char *value3 = strdup("");
    char *value4 = strdup("defaultvalue");

    cfg_opt_t opts[] = {
        CFG_SIMPLE_STR("param1", &value1),
        CFG_SIMPLE_INT("param2", &value2),
        CFG_SIMPLE_STR("param3", &value3),
        CFG_SIMPLE_STR("param4", &value4),
        CFG_END()
    };

    cfg_t *cfg = cfg_init(opts, 0);
    ASSERT_NE(cfg, NULL);

    cfg_parse_string(cfg, s);

    ASSERT_TRUE(strcmp(value1, "value") == 0);
    ASSERT_TRUE(value2 == 123);
    ASSERT_TRUE(strcmp(value3, "qwe") == 0);
    ASSERT_TRUE(strcmp(value4, "defaultvalue") == 0);

    cfg_free(cfg);

    free(value1);
    free(value3);
    free(value4);
}

TEST(config_parser, test_2)
{
    const char *s =
        "param1_float = qwe\n"          // invalid float value
        "  param2_float = 3.45  \n"     // float value with leading and trailing spaces
        "param3_str = value3\n";

    double value1 = 11.0;
    double value2 = 11.0;
    char  *value3 = strdup("defaultvalue");

    cfg_opt_t opts[] = {
        CFG_SIMPLE_FLOAT("param1_float", &value1),
        CFG_SIMPLE_FLOAT("param2_float", &value2),
        CFG_SIMPLE_STR("param3_str", &value3),
        CFG_END()
    };

    cfg_t *cfg = cfg_init(opts, 0);
    ASSERT_NE(cfg, NULL);

    cfg_parse_string(cfg, s);
    ASSERT_TRUE(fabs(value1 - 0) < 1e-5);
    ASSERT_TRUE(fabs(value2 - 3.45) < 1e-5);
    ASSERT_TRUE(strcmp(value3, "value3") == 0);

    cfg_free(cfg);

    free(value3);
}

TEST(config_parser, test_3)
{
    const char *s =
        "# commented line\n"            // comment
        "\n"                            // empty line by itself
        "param1 = var1\n"               // multiple definitions
        "param1 = var2\n"               // of the same parameter
        "# param1 = var3\n";            // and one of them commented out

    char *value1 = strdup("");

    cfg_opt_t opts[] = {
        CFG_SIMPLE_STR("param1", &value1),
        CFG_END()
    };

    cfg_t *cfg = cfg_init(opts, 0);
    ASSERT_NE(cfg, NULL);

    cfg_parse_string(cfg, s);
    ASSERT_TRUE(strcmp(value1, "var2") == 0);

    cfg_free(cfg);
    free(value1);
}
