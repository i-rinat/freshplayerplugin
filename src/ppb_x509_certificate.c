/*
 * Copyright © 2013-2017  Rinat Ibragimov
 *
 * This file is part of FreshPlayerPlugin.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "pp_interface.h"
#include "pp_resource.h"
#include "ppb_var.h"
#include "ppb_x509_certificate.h"
#include "reverse_constant.h"
#include "static_assert.h"
#include "tables.h"
#include "trace_core.h"
#include "utils.h"
#include <ctype.h>
#include <openssl/x509.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct pp_x509_certificate_s {
    COMMON_STRUCTURE_FIELDS
    X509           *cert;
    char           *raw_data;
    uint32_t        raw_data_length;
};

STATIC_ASSERT(sizeof(struct pp_x509_certificate_s) <= LARGEST_RESOURCE_SIZE);

PP_Resource
ppb_x509_certificate_create(PP_Instance instance)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }
    PP_Resource x509 = pp_resource_allocate(PP_RESOURCE_X509_CERTIFICATE, pp_i);
    return x509;
}

static
void
ppb_x509_certificate_destroy(void *ptr)
{
    struct pp_x509_certificate_s *xc = ptr;

    if (xc->cert) {
        X509_free(xc->cert);
        xc->cert = NULL;
    }

    free(xc->raw_data);
}

PP_Bool
ppb_x509_certificate_is_x509_certificate(PP_Resource resource)
{
    return pp_resource_get_type(resource) == PP_RESOURCE_X509_CERTIFICATE;
}

PP_Bool
ppb_x509_certificate_initialize(PP_Resource resource, const char *bytes, uint32_t length)
{
    const unsigned char *in = (const unsigned char *)bytes;
    struct pp_x509_certificate_s *xc = pp_resource_acquire(resource, PP_RESOURCE_X509_CERTIFICATE);
    if (!xc) {
        return PP_FALSE;
    }

    PP_Bool retval = PP_TRUE;
    if (d2i_X509(&xc->cert, &in, length) == NULL) {
        retval = PP_FALSE;
        goto done;
    }

    free_and_nullify(xc->raw_data);
    xc->raw_data = malloc(length);
    if (xc->raw_data) {
        memcpy(xc->raw_data, bytes, length);
        xc->raw_data_length = length;
    }

done:
    pp_resource_release(resource);
    return retval;
}

static struct PP_Var
get_xname_field_value(X509_NAME *xname, int nid)
{
    ASN1_STRING *asn1_str;
    int          loc;

    loc = X509_NAME_get_index_by_NID(xname, nid, -1);
    if (loc == -1)
        return PP_MakeNull();
    asn1_str = X509_NAME_ENTRY_get_data(X509_NAME_get_entry(xname, loc));
    return ppb_var_var_from_utf8((char *)asn1_str->data, asn1_str->length);
}

static struct PP_Var
get_cert_issuer_field_value(X509 *cert, int nid)
{
    X509_NAME *xname = X509_get_issuer_name(cert);
    if (!xname)
        return PP_MakeNull();
    return get_xname_field_value(xname, nid);
}

static struct PP_Var
get_cert_subject_field_value(X509 *cert, int nid)
{
    X509_NAME *xname = X509_get_subject_name(cert);
    if (!xname)
        return PP_MakeNull();
    return get_xname_field_value(xname, nid);
}

static struct PP_Var
convert_tm_to_pp_var(struct tm *t, double subsec, int hofs, int mofs)
{
    return PP_MakeDouble(timegm(t) - hofs * 3600 - mofs * 60 + subsec);
}

static struct PP_Var
convert_asn1_time_to_pp_var(ASN1_TIME *asn1_time)
{
    struct tm t;
    int len = asn1_time->length;
    unsigned char *s = asn1_time->data;

    memset(&t, 0, sizeof(t));
    if (asn1_time->type == V_ASN1_GENERALIZEDTIME) {
        // "YYYYMMDDHH[MM[SS[.fff]]]", "YYYYMMDDHH[MM[SS[.fff]]]Z",
        // or "YYYYMMDDHH[MM[SS[.fff]]]+-HHMM"
        double subsec = 0;

        // YYYY
        if (len < 4 || !isdigit(s[0]) || !isdigit(s[1]) || !isdigit(s[2]) || !isdigit(s[3]))
            goto parse_error;
        t.tm_year = (s[0] - '0') * 1000 + (s[1] - '0') * 100 + (s[2] - '0') * 10 + (s[3] - '0');
        t.tm_year -= 1900;
        len -= 4; s += 4;

        // MM
        if (len < 2 || !isdigit(s[0]) || !isdigit(s[1]))
            goto parse_error;
        t.tm_mon = (s[0] - '0') * 10 + (s[1] - '0') - 1;
        len -= 2; s += 2;

        // DD
        if (len < 2 || !isdigit(s[0]) || !isdigit(s[1]))
            goto parse_error;
        t.tm_mday = (s[0] - '0') * 10 + (s[1] - '0');
        len -= 2; s += 2;

        // HH
        if (len < 2 || !isdigit(s[0]) || !isdigit(s[1]))
            goto parse_error;
        t.tm_hour = (s[0] - '0') * 10 + (s[1] - '0');
        len -= 2; s += 2;

        if (len >= 2 && isdigit(s[0])) {
            // MM
            if (len < 2 || !isdigit(s[0]) || !isdigit(s[1]))
                goto parse_error;
            t.tm_min = (s[0] - '0') * 10 + (s[1] - '0');
            len -= 2; s += 2;

            if (len >= 2 && isdigit(s[0])) {
                // SS
                if (len < 2 || !isdigit(s[0]) || !isdigit(s[1]))
                    goto parse_error;
                t.tm_sec = (s[0] - '0') * 10 + (s[1] - '0');
                len -= 2; s += 2;

                if (len >= 4 && s[0] == '.' && isdigit(s[1]) && isdigit(s[2]) && isdigit(s[3])) {
                    // .fff
                    subsec = (s[1] - '0') * 0.1 + (s[2] - '0') * 0.01 + (s[3] - '0') * 0.001;
                    len -= 4; s += 4;
                }
            }
        }

        if (len == 0)
            return convert_tm_to_pp_var(&t, subsec, 0, 0);

        if (len == 1) {
            if (s[0] == 'Z')
                return convert_tm_to_pp_var(&t, subsec, 0, 0);
            else
                goto parse_error;
        }

        if (len < 1 || (s[0] != '+' && s[0] != '-'))
            goto parse_error;

        int tz_sign = (s[0] == '+') ? 1 : -1;
        len -= 1; s += 1;

        if (len < 2 || !isdigit(s[0]) || !isdigit(s[1]))
            goto parse_error;
        int tz_hours = (s[0] - '0') * 10 + (s[1] - '0');
        len -= 2; s += 2;

        if (len < 2 || !isdigit(s[0]) || !isdigit(s[1]))
            goto parse_error;
        int tz_minutes = (s[0] - '0') * 10 + (s[1] - '0');
        // keep as reminder: len -= 2; s += 2;

        return convert_tm_to_pp_var(&t, subsec, tz_sign * tz_hours, tz_minutes);
    } else if (asn1_time->type == V_ASN1_UTCTIME) {
        // either "YYMMDDhhmm[ss]Z" or "YYMMDDhhmm[ss](+|-)hhmm"

        // YY
        if (len < 2 || !isdigit(s[0]) || !isdigit(s[1]))
            goto parse_error;
        t.tm_year = (s[0] - '0') * 10 + (s[1] - '0');
        t.tm_year += (t.tm_year < 70) * 100;
        len -= 2; s += 2;

        // MM
        if (len < 2 || !isdigit(s[0]) || !isdigit(s[1]))
            goto parse_error;
        t.tm_mon = (s[0] - '0') * 10 + (s[1] - '0') - 1;
        len -= 2; s += 2;

        // DD
        if (len < 2 || !isdigit(s[0]) || !isdigit(s[1]))
            goto parse_error;
        t.tm_mday = (s[0] - '0') * 10 + (s[1] - '0');
        len -= 2; s += 2;

        // hh
        if (len < 2 || !isdigit(s[0]) || !isdigit(s[1]))
            goto parse_error;
        t.tm_hour = (s[0] - '0') * 10 + (s[1] - '0');
        len -= 2; s += 2;

        // mm
        if (len < 2 || !isdigit(s[0]) || !isdigit(s[1]))
            goto parse_error;
        t.tm_min = (s[0] - '0') * 10 + (s[1] - '0');
        len -= 2; s += 2;

        if (len >= 2 && isdigit(s[0])) {
            // ss
            if (len < 2 || !isdigit(s[0]) || !isdigit(s[1]))
                goto parse_error;
            t.tm_sec = (s[0] - '0') * 10 + (s[1] - '0');
            len -= 2; s += 2;
        }

        if (len == 1 && s[0] == 'Z')
            return convert_tm_to_pp_var(&t, 0, 0, 0);

        if (len < 1 || (s[0] != '+' && s[0] != '-'))
            goto parse_error;

        int tz_sign = (s[0] == '+') ? 1 : -1;
        len -= 1; s += 1;

        if (len < 2 || !isdigit(s[0]) || !isdigit(s[1]))
            goto parse_error;
        int tz_hours = (s[0] - '0') * 10 + (s[1] - '0');
        len -= 2; s += 2;

        if (len < 2 || !isdigit(s[0]) || !isdigit(s[1]))
            goto parse_error;
        int tz_minutes = (s[0] - '0') * 10 + (s[1] - '0');
        // keep as reminder: len -= 2; s += 2;

        return convert_tm_to_pp_var(&t, 0, tz_sign * tz_hours, tz_minutes);
    } else {
        goto parse_error;
    }

parse_error:
    return PP_MakeNull();
}

struct PP_Var
ppb_x509_certificate_get_field(PP_Resource resource, PP_X509Certificate_Private_Field field)
{
    struct PP_Var    var = PP_MakeNull();
    ASN1_INTEGER    *asn1_int;
    ASN1_TIME       *asn1_time;

    struct pp_x509_certificate_s *xc = pp_resource_acquire(resource, PP_RESOURCE_X509_CERTIFICATE);
    if (!xc) {
        trace_error("%s, bad resource\n", __func__);
        return PP_MakeNull();
    }

    switch (field) {
    case PP_X509CERTIFICATE_PRIVATE_ISSUER_COMMON_NAME:
        var = get_cert_issuer_field_value(xc->cert, NID_commonName);
        goto done;

    case PP_X509CERTIFICATE_PRIVATE_ISSUER_LOCALITY_NAME:
        var = get_cert_issuer_field_value(xc->cert, NID_localityName);
        goto done;

    case PP_X509CERTIFICATE_PRIVATE_ISSUER_STATE_OR_PROVINCE_NAME:
        var = get_cert_issuer_field_value(xc->cert, NID_stateOrProvinceName);
        goto done;

    case PP_X509CERTIFICATE_PRIVATE_ISSUER_COUNTRY_NAME:
        var = get_cert_issuer_field_value(xc->cert, NID_countryName);
        goto done;

    case PP_X509CERTIFICATE_PRIVATE_ISSUER_ORGANIZATION_NAME:
        var = get_cert_issuer_field_value(xc->cert, NID_organizationName);
        goto done;

    case PP_X509CERTIFICATE_PRIVATE_ISSUER_ORGANIZATION_UNIT_NAME:
        var = get_cert_issuer_field_value(xc->cert, NID_organizationalUnitName);
        goto done;

    case PP_X509CERTIFICATE_PRIVATE_ISSUER_UNIQUE_ID:
        var = PP_MakeNull(); // see ppapi/c/private/ppb_x509_certificate_private.h for details
        goto done;

    case PP_X509CERTIFICATE_PRIVATE_SUBJECT_COMMON_NAME:
        var = get_cert_subject_field_value(xc->cert, NID_commonName);
        goto done;

    case PP_X509CERTIFICATE_PRIVATE_SUBJECT_LOCALITY_NAME:
        var = get_cert_subject_field_value(xc->cert, NID_localityName);
        goto done;

    case PP_X509CERTIFICATE_PRIVATE_SUBJECT_STATE_OR_PROVINCE_NAME:
        var = get_cert_subject_field_value(xc->cert, NID_stateOrProvinceName);
        goto done;

    case PP_X509CERTIFICATE_PRIVATE_SUBJECT_COUNTRY_NAME:
        var = get_cert_subject_field_value(xc->cert, NID_countryName);
        goto done;

    case PP_X509CERTIFICATE_PRIVATE_SUBJECT_ORGANIZATION_NAME:
        var = get_cert_subject_field_value(xc->cert, NID_organizationName);
        goto done;

    case PP_X509CERTIFICATE_PRIVATE_SUBJECT_ORGANIZATION_UNIT_NAME:
        var = get_cert_subject_field_value(xc->cert, NID_organizationalUnitName);
        goto done;

    case PP_X509CERTIFICATE_PRIVATE_SUBJECT_UNIQUE_ID:
        var = PP_MakeNull(); // see ppapi/c/private/ppb_x509_certificate_private.h for details
        goto done;

    case PP_X509CERTIFICATE_PRIVATE_VERSION:
        var = PP_MakeNull(); // see ppapi/c/private/ppb_x509_certificate_private.h for details
        goto done;

    case PP_X509CERTIFICATE_PRIVATE_SERIAL_NUMBER:
        asn1_int = X509_get_serialNumber(xc->cert);
        if (!asn1_int)
            return PP_MakeNull();
        var = ppb_var_array_buffer_create(asn1_int->length);
        memcpy(ppb_var_array_buffer_map(var), asn1_int->data, asn1_int->length);
        ppb_var_array_buffer_unmap(var);
        goto done;

    case PP_X509CERTIFICATE_PRIVATE_SIGNATURE_ALGORITHM_OID:
        var = PP_MakeNull(); // see ppapi/c/private/ppb_x509_certificate_private.h for details
        goto done;

    case PP_X509CERTIFICATE_PRIVATE_SIGNATURE_ALGORITHM_PARAMATERS_RAW:
        var = PP_MakeNull(); // see ppapi/c/private/ppb_x509_certificate_private.h for details
        goto done;

    case PP_X509CERTIFICATE_PRIVATE_VALIDITY_NOT_BEFORE:
        asn1_time = X509_get_notBefore(xc->cert);
        if (!asn1_time)
            return PP_MakeNull();
        var = convert_asn1_time_to_pp_var(asn1_time);
        goto done;

    case PP_X509CERTIFICATE_PRIVATE_VALIDITY_NOT_AFTER:
        asn1_time = X509_get_notAfter(xc->cert);
        if (!asn1_time)
            return PP_MakeNull();
        var = convert_asn1_time_to_pp_var(asn1_time);
        goto done;

    case PP_X509CERTIFICATE_PRIVATE_SUBJECT_PUBLIC_KEY_ALGORITHM_OID:
        trace_error("%s, not implemented path\n", __func__);
        goto done;

    case PP_X509CERTIFICATE_PRIVATE_SUBJECT_PUBLIC_KEY:
        var = PP_MakeNull(); // see ppapi/c/private/ppb_x509_certificate_private.h for details
        goto done;

    case PP_X509CERTIFICATE_PRIVATE_RAW:
        if (xc->raw_data) {
            var = ppb_var_array_buffer_create(xc->raw_data_length);
            memcpy(ppb_var_array_buffer_map(var), xc->raw_data, xc->raw_data_length);
            ppb_var_array_buffer_unmap(var);
        }
        goto done;

    case PP_X509CERTIFICATE_PRIVATE_ISSUER_DISTINGUISHED_NAME:
        var = get_cert_issuer_field_value(xc->cert, NID_distinguishedName);
        goto done;

    case PP_X509CERTIFICATE_PRIVATE_SUBJECT_DISTINGUISHED_NAME:
        var = get_cert_subject_field_value(xc->cert, NID_distinguishedName);
        goto done;

    default:
        var = PP_MakeNull(); // unknown
        goto done;
    }

done:
    pp_resource_release(resource);
    return var;
}


// trace wrappers
TRACE_WRAPPER
PP_Resource
trace_ppb_x509_certificate_create(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_x509_certificate_create(instance);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_x509_certificate_is_x509_certificate(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_x509_certificate_is_x509_certificate(resource);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_x509_certificate_initialize(PP_Resource resource, const char *bytes, uint32_t length)
{
    trace_info("[PPB] {full} %s resource=%d, bytes=%p, length=%u\n", __func__+6, resource, bytes,
               length);
    return ppb_x509_certificate_initialize(resource, bytes, length);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_x509_certificate_get_field(PP_Resource resource, PP_X509Certificate_Private_Field field)
{
    trace_info("[PPB] {full} %s resource=%d, field=%s(%u)\n", __func__+6, resource,
               reverse_x509_certificate_field(field), field);
    return ppb_x509_certificate_get_field(resource, field);
}

const struct PPB_X509Certificate_Private_0_1 ppb_x509_certificate_interface_0_1 = {
    .Create =                   TWRAPF(ppb_x509_certificate_create),
    .IsX509CertificatePrivate = TWRAPF(ppb_x509_certificate_is_x509_certificate),
    .Initialize =               TWRAPF(ppb_x509_certificate_initialize),
    .GetField =                 TWRAPF(ppb_x509_certificate_get_field),
};

static
void
__attribute__((constructor))
constructor_ppb_x509_certificate(void)
{
    register_interface(PPB_X509CERTIFICATE_PRIVATE_INTERFACE_0_1,
                       &ppb_x509_certificate_interface_0_1);
    register_resource(PP_RESOURCE_X509_CERTIFICATE, ppb_x509_certificate_destroy);
}
