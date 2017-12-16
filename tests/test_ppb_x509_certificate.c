#include "nih_test.h"
#include <glib.h>
#include <math.h>
#include <src/pp_resource.h>
#include <src/ppb_core.h>
#include <src/ppb_var.h>
#include <src/ppb_x509_certificate.c>
#include <stdio.h>
#include <tests/common.h>

static
void
test_hlp(const char *s, int type, double target, double eps)
{
    ASN1_TIME at = { .data = (unsigned char *)s, .length = strlen(s), .type = type };
    struct PP_Var var = convert_asn1_time_to_pp_var(&at);

    if (target > 0) {
        // expecting success
        ASSERT_EQ(var.type, PP_VARTYPE_DOUBLE);
        ASSERT_TRUE(fabs(var.value.as_double - target) < eps);
    } else {
        // expecting failure
        ASSERT_EQ(var.type, PP_VARTYPE_NULL);
    }
}

TEST(ppb_x509_certificate, convert_asn1_time_to_pp_var)
{
    printf("V_ASN1_UTCTIME\n");
    test_hlp("9912312359Z",       V_ASN1_UTCTIME, 946684740,            1e-3);
    test_hlp("991231235959+0200", V_ASN1_UTCTIME, 946684799 - 3600 * 2, 1e-3);

    printf("V_ASN1_GENERALIZEDTIME\n");
    test_hlp("20001231235959.999",      V_ASN1_GENERALIZEDTIME, 978307199.999, 1e-3);
    test_hlp("20001231235959.999+0300", V_ASN1_GENERALIZEDTIME, 978307199.999 - 3600 * 3, 1e-3);
}

TEST(ppb_x509_certificate, main)
{
    struct PP_Var var;
    const char *s;
    PP_Resource x509;
    PP_Instance instance = create_instance();

    x509 = ppb_x509_certificate_create(instance);
    ASSERT_TRUE(x509);

    gchar *buf;
    gsize len;
    gboolean ret1 = g_file_get_contents(TEST_DATA_DIR "/wikipedia.org.der", &buf, &len, NULL);
    ASSERT_TRUE(ret1);

    PP_Bool ret2 = ppb_x509_certificate_initialize(x509, buf, len);
    ASSERT_EQ(ret2, PP_TRUE);
    g_free(buf);

#define TST_GET_FIELD(fname)                            \
    printf(#fname "\n");                                \
    var = ppb_x509_certificate_get_field(x509, fname);

    // ========
    TST_GET_FIELD(PP_X509CERTIFICATE_PRIVATE_ISSUER_COMMON_NAME);
    s = ppb_var_var_to_utf8(var, NULL);
    ASSERT_TRUE(strcmp(s, "GlobalSign Organization Validation CA - SHA256 - G2") == 0);
    ppb_var_release(var);

    // ========
    TST_GET_FIELD(PP_X509CERTIFICATE_PRIVATE_ISSUER_ORGANIZATION_NAME);
    s = ppb_var_var_to_utf8(var, NULL);
    ASSERT_TRUE(strcmp(s, "GlobalSign nv-sa") == 0);
    ppb_var_release(var);

    // ========
    TST_GET_FIELD(PP_X509CERTIFICATE_PRIVATE_SUBJECT_COMMON_NAME);
    s = ppb_var_var_to_utf8(var, NULL);
    ASSERT_TRUE(strcmp(s, "*.wikipedia.org") == 0);
    ppb_var_release(var);

    // ========
    TST_GET_FIELD(PP_X509CERTIFICATE_PRIVATE_SUBJECT_ORGANIZATION_NAME);
    s = ppb_var_var_to_utf8(var, NULL);
    ASSERT_TRUE(strcmp(s, "Wikimedia Foundation, Inc.") == 0);
    ppb_var_release(var);

    // ========
    TST_GET_FIELD(PP_X509CERTIFICATE_PRIVATE_SUBJECT_LOCALITY_NAME);
    s = ppb_var_var_to_utf8(var, NULL);
    ASSERT_TRUE(strcmp(s, "San Francisco") == 0);
    ppb_var_release(var);

    // ========
    TST_GET_FIELD(PP_X509CERTIFICATE_PRIVATE_SUBJECT_STATE_OR_PROVINCE_NAME);
    s = ppb_var_var_to_utf8(var, NULL);
    ASSERT_TRUE(strcmp(s, "California") == 0);
    ppb_var_release(var);

    // ========
    TST_GET_FIELD(PP_X509CERTIFICATE_PRIVATE_SUBJECT_COUNTRY_NAME);
    s = ppb_var_var_to_utf8(var, NULL);
    ASSERT_TRUE(strcmp(s, "US") == 0);
    ppb_var_release(var);

    ppb_core_release_resource(x509);

    destroy_instance(instance);
}
