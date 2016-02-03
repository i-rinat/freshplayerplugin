#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <src/ppb_net_address.c>
#include "src/ppb_var.h"
#include "common.h"
#include "src/ppb_core.h"


static PP_Instance instance;


static
void
test_ppb_net_address_private(void)
{
    printf("%s\n", __func__);

    printf("  ipv4, compose/decompose\n");
    {
        const uint8_t ip[4] = {192, 168, 1, 2};
        const uint16_t port = 1234;
        uint8_t ip_out[sizeof(ip)] = {};
        struct PP_NetAddress_Private addr;

        ppb_net_address_private_create_from_ipv4_address(ip, port, &addr);

        ppb_net_address_private_get_address(&addr, ip_out, sizeof(ip_out));
        assert(memcmp(ip, ip_out, sizeof(ip)) == 0);

        assert(ppb_net_address_private_get_port(&addr) == port);
        assert(ppb_net_address_private_get_family(&addr) == PP_NETADDRESSFAMILY_PRIVATE_IPV4);
        assert(ppb_net_address_private_get_scope_id(&addr) == 0);
    }

    printf("  ipv4, equality\n");
    {
        const uint8_t ip[4] = {192, 168, 1, 2};
        const uint16_t port1 = 1234;
        const uint16_t port2 = 5678;

        struct PP_NetAddress_Private addr1;
        struct PP_NetAddress_Private addr2;

        ppb_net_address_private_create_from_ipv4_address(ip, port1, &addr1);
        ppb_net_address_private_create_from_ipv4_address(ip, port2, &addr2);

        // different ports, hence are different
        assert(ppb_net_address_private_are_equal(&addr1, &addr2) == PP_FALSE);
        assert(ppb_net_address_private_are_hosts_equal(&addr1, &addr2) == PP_TRUE);

        ppb_net_address_private_create_from_ipv4_address(ip, port2, &addr1);
        ppb_net_address_private_create_from_ipv4_address(ip, port2, &addr2);

        // should be equal
        assert(ppb_net_address_private_are_equal(&addr1, &addr2) == PP_TRUE);
        assert(ppb_net_address_private_are_hosts_equal(&addr1, &addr2) == PP_TRUE);
    }

    printf("  ipv4, replace port\n");
    {
        const uint8_t ip[4] = {192, 168, 1, 2};
        const uint16_t port1 = 1234;
        const uint16_t port2 = 1234;

        struct PP_NetAddress_Private addr1;
        struct PP_NetAddress_Private addr2;

        ppb_net_address_private_create_from_ipv4_address(ip, port1, &addr1);
        assert(ppb_net_address_private_get_port(&addr1) == port1);

        ppb_net_address_private_replace_port(&addr1, port2, &addr2);
        assert(ppb_net_address_private_get_port(&addr2) == port2);
    }

    printf("  ipv4, describe\n");
    {
        const uint8_t ip[4] = {192, 168, 1, 2};
        const uint16_t port = 1234;

        struct PP_NetAddress_Private addr;
        struct PP_Var s;

        ppb_net_address_private_create_from_ipv4_address(ip, port, &addr);
        s = ppb_net_address_private_describe(0, &addr, PP_TRUE);
        assert(strcmp(ppb_var_var_to_utf8(s, NULL), "192.168.1.2:1234") == 0);
        ppb_var_release(s);

        s = ppb_net_address_private_describe(0, &addr, PP_FALSE);
        assert(strcmp(ppb_var_var_to_utf8(s, NULL), "192.168.1.2") == 0);
        ppb_var_release(s);
    }

    printf("  ipv6, compose/decompose\n");
    {
        const uint8_t ip[16] = { 0x01, 0x23, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde,
                                 0xef, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 };
        const uint16_t port = 3333;
        const uint32_t scope_id = 555;

        struct PP_NetAddress_Private addr;
        uint8_t ip_out[sizeof(ip)] = {};

        ppb_net_address_private_create_from_ipv6_address(ip, scope_id, port, &addr);

        ppb_net_address_private_get_address(&addr, ip_out, sizeof(ip_out));
        assert(memcmp(ip, ip_out, sizeof(ip)) == 0);

        assert(ppb_net_address_private_get_port(&addr) == port);
        assert(ppb_net_address_private_get_family(&addr) == PP_NETADDRESSFAMILY_PRIVATE_IPV6);
        assert(ppb_net_address_private_get_scope_id(&addr) == scope_id);
    }

    printf("  ipv6, equality\n");
    {
        const uint8_t ip[16] = { 0x01, 0x23, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde,
                                 0xef, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 };
        const uint16_t port1 = 3333;
        const uint16_t port2 = 5555;

        struct PP_NetAddress_Private addr1;
        struct PP_NetAddress_Private addr2;

        // different ports
        ppb_net_address_private_create_from_ipv6_address(ip, 0, port1, &addr1);
        ppb_net_address_private_create_from_ipv6_address(ip, 0, port2, &addr2);

        // different, but hosts are the same
        assert(ppb_net_address_private_are_equal(&addr1, &addr2) == PP_FALSE);
        assert(ppb_net_address_private_are_hosts_equal(&addr1, &addr2) == PP_TRUE);

        // same ports
        ppb_net_address_private_create_from_ipv6_address(ip, 0, port2, &addr1);
        ppb_net_address_private_create_from_ipv6_address(ip, 0, port2, &addr2);

        // should be equal
        assert(ppb_net_address_private_are_equal(&addr1, &addr2) == PP_TRUE);
        assert(ppb_net_address_private_are_hosts_equal(&addr1, &addr2) == PP_TRUE);
    }

    printf("  ipv6, replace port\n");
    {
        const uint8_t ip[16] = { 0x01, 0x23, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde,
                                 0xef, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 };
        const uint16_t port1 = 1234;
        const uint16_t port2 = 1234;

        struct PP_NetAddress_Private addr1;
        struct PP_NetAddress_Private addr2;

        ppb_net_address_private_create_from_ipv6_address(ip, 0, port1, &addr1);
        assert(ppb_net_address_private_get_port(&addr1) == port1);

        ppb_net_address_private_replace_port(&addr1, port2, &addr2);
        assert(ppb_net_address_private_get_port(&addr2) == port2);
    }

    printf("  ipv6, describe\n");
    {
        const uint8_t ip[16] = { 0x01, 0x23, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde,
                                 0xef, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 };
        const uint16_t port = 1234;

        struct PP_NetAddress_Private addr;
        struct PP_Var s;

        ppb_net_address_private_create_from_ipv6_address(ip, 0, port, &addr);
        s = ppb_net_address_private_describe(0, &addr, PP_TRUE);
        assert(strcmp(ppb_var_var_to_utf8(s, NULL),
                      "[123:3456:789a:bcde:ef11:2233:4455:6677]:1234") == 0);
        ppb_var_release(s);

        s = ppb_net_address_private_describe(0, &addr, PP_FALSE);
        assert(strcmp(ppb_var_var_to_utf8(s, NULL),
                      "123:3456:789a:bcde:ef11:2233:4455:6677") == 0);
        ppb_var_release(s);
    }

    printf("  ill-formed PP_NetAddress_Private\n");
    {
        const uint8_t ip[4] = {192, 168, 1, 2};
        const uint16_t port = 1234;

        struct PP_NetAddress_Private addr1;
        struct PP_NetAddress_Private addr2 = {};
        struct PP_NetAddress_Private addr3 = {};
        struct PP_Var s;
        uint8_t ip_out[4];

        ppb_net_address_private_create_from_ipv4_address(ip, port, &addr1);

        assert(ppb_net_address_private_are_equal(&addr1, &addr2) == PP_FALSE);
        assert(ppb_net_address_private_are_hosts_equal(&addr1, &addr2) == PP_FALSE);
        assert(ppb_net_address_private_get_family(&addr2)==PP_NETADDRESSFAMILY_PRIVATE_UNSPECIFIED);
        assert(ppb_net_address_private_get_port(&addr2) == 0);

        assert(ppb_net_address_private_get_address(&addr2, ip_out, sizeof(ip_out)) == PP_FALSE);
        assert(ppb_net_address_private_replace_port(&addr2, port, &addr3) == PP_FALSE);

        s = ppb_net_address_private_describe(0, &addr2, PP_TRUE);
        assert(s.type == PP_VARTYPE_UNDEFINED);
    }

    printf("  ipv4, get any address\n");
    {
        const uint8_t all_zeroes[4] = {};
        uint8_t ip[4];
        struct PP_NetAddress_Private addr;

        memset(&addr, 5, sizeof(addr)); // overwrite with something
        ppb_net_address_private_get_any_address(PP_FALSE, &addr);
        assert(ppb_net_address_private_get_family(&addr) == PP_NETADDRESSFAMILY_PRIVATE_IPV4);

        ppb_net_address_private_get_address(&addr, ip, sizeof(ip));
        assert(memcmp(ip, all_zeroes, sizeof(ip)) == 0);
    }

    printf("  ipv6, get any address\n");
    {
        const uint8_t all_zeroes[16] = {};
        uint8_t ip[16];
        struct PP_NetAddress_Private addr;

        memset(&addr, 5, sizeof(addr)); // overwrite with something
        ppb_net_address_private_get_any_address(PP_TRUE, &addr);
        assert(ppb_net_address_private_get_family(&addr) == PP_NETADDRESSFAMILY_PRIVATE_IPV6);

        ppb_net_address_private_get_address(&addr, ip, sizeof(ip));
        assert(memcmp(ip, all_zeroes, sizeof(ip)) == 0);
    }
}

static
void
test_ppb_net_address(void)
{
    printf("%s\n", __func__);

    printf("  ipv4, compose/decompose\n");
    {
        const struct PP_NetAddress_IPv4 ipv4 = { .addr = {192, 168, 1, 2},
                                                 .port = htons(3456) };
        struct PP_NetAddress_IPv4 ipv4_a = {};
        struct PP_NetAddress_IPv6 ipv6 = {};

        PP_Resource addr = ppb_net_address_create_from_ipv4_address(instance, &ipv4);
        assert(ppb_net_address_is_net_address(addr) == PP_TRUE);
        assert(ppb_net_address_get_family(addr) == PP_NETADDRESS_FAMILY_IPV4);
        assert(ppb_net_address_describe_as_ipv6_address(addr, &ipv6) == PP_FALSE);
        assert(ppb_net_address_describe_as_ipv4_address(addr, &ipv4_a) == PP_TRUE);

        assert(memcmp(&ipv4, &ipv4_a, sizeof(ipv4_a)) == 0);

        struct PP_Var s = ppb_net_address_describe_as_string(addr, PP_TRUE);
        assert(strcmp(ppb_var_var_to_utf8(s, NULL), "192.168.1.2:3456") == 0);
        ppb_var_release(s);

        ppb_core_release_resource(addr);
    }

    printf("  ipv6, compose/decompose\n");
    {
        const struct PP_NetAddress_IPv6 ipv6 = { .addr = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                                          12, 13, 14, 15, 16},
                                                 .port = htons(3456) };
        struct PP_NetAddress_IPv6 ipv6_a = {};
        struct PP_NetAddress_IPv4 ipv4 = {};

        PP_Resource addr = ppb_net_address_create_from_ipv6_address(instance, &ipv6);
        assert(ppb_net_address_is_net_address(addr) == PP_TRUE);
        assert(ppb_net_address_get_family(addr) == PP_NETADDRESS_FAMILY_IPV6);
        assert(ppb_net_address_describe_as_ipv4_address(addr, &ipv4) == PP_FALSE);

        assert(ppb_net_address_describe_as_ipv6_address(addr, &ipv6_a) == PP_TRUE);
        assert(memcmp(&ipv6, &ipv6_a, sizeof(ipv6_a)) == 0);

        struct PP_Var s = ppb_net_address_describe_as_string(addr, PP_TRUE);
        assert(strcmp(ppb_var_var_to_utf8(s, NULL), "[102:304:506:708:90a:b0c:d0e:f10]:3456") == 0);
        ppb_var_release(s);

        ppb_core_release_resource(addr);
    }
}

int
main(void)
{
    instance = create_instance();

    test_ppb_net_address_private();
    test_ppb_net_address();

    destroy_instance(instance);

    printf("pass\n");
    return 0;
}
