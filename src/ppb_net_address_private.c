#include <ppapi/c/private/ppb_net_address_private.h>
#include <stddef.h>


const PPB_NetAddress_Private ppb_net_address_private = {
    .AreEqual = NULL,
    .AreHostsEqual = NULL,
    .Describe = NULL,
    .ReplacePort = NULL,
    .GetAnyAddress = NULL,
    .GetFamily = NULL,
    .GetPort = NULL,
    .GetAddress = NULL,
    .GetScopeID = NULL,
    .CreateFromIPv4Address = NULL,
    .CreateFromIPv6Address = NULL
};
