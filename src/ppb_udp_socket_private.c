#include <ppapi/c/private/ppb_udp_socket_private.h>
#include <stddef.h>


const struct PPB_UDPSocket_Private_0_4 ppb_udp_socket_private_interface_0_4 = {
    .Create = (void *)27,
    .IsUDPSocket = (void *)28,
    .SetSocketFeature = (void *)29,
    .Bind = (void *)30,
    .GetBoundAddress = (void *)31,
    .RecvFrom = (void *)32,
    .GetRecvFromAddress = (void *)33,
    .SendTo = (void *)34,
    .Close = (void *)35,
};
