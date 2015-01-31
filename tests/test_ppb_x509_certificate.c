#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <src/ppb_x509_certificate.h>
#include <src/ppb_core.h>
#include <src/pp_resource.h>
#include <tests/common.h>
#include <glib.h>


int
main(void)
{
    PP_Instance instance = create_instance();

    PP_Resource x509 = ppb_x509_certificate_create(instance);
    assert(x509);

    gchar *buf;
    gsize len;
    gboolean ret1 = g_file_get_contents(TEST_DATA_DIR "/wikipedia.org.der", &buf, &len, NULL);
    assert(ret1 == TRUE);

    PP_Bool ret2 = ppb_x509_certificate_initialize(x509, buf, len);
    assert(ret2 == PP_TRUE);
    g_free(buf);

    ppb_core_release_resource(x509);
    destroy_instance(instance);
    printf("pass\n");
    return 0;
}
