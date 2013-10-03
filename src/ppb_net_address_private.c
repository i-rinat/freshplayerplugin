#include <ppapi/c/private/ppb_net_address_private.h>
#include <stddef.h>


const struct PPB_NetAddress_Private_1_1 ppb_net_address_private_interface_1_1 = {
    .AreEqual = (void *)199,
    .AreHostsEqual = (void *)200,
    .Describe = (void *)201,
    .ReplacePort = (void *)202,
    .GetAnyAddress = (void *)203,
    .GetFamily = (void *)204,
    .GetPort = (void *)205,
    .GetAddress = (void *)206,
    .GetScopeID = (void *)207,
    .CreateFromIPv4Address = (void *)208,
    .CreateFromIPv6Address = (void *)209,
};
