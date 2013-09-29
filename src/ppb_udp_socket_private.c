#include <ppapi/c/private/ppb_udp_socket_private.h>
#include <stddef.h>


const PPB_UDPSocket_Private ppb_udp_socket_private = {
    .Create = NULL,
    .IsUDPSocket = NULL,
    .SetSocketFeature = NULL,
    .Bind = NULL,
    .GetBoundAddress = NULL,
    .RecvFrom = NULL,
    .GetRecvFromAddress = NULL,
    .SendTo = NULL,
    .Close = NULL
};
