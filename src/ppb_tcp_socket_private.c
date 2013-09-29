#include <ppapi/c/private/ppb_tcp_socket_private.h>
#include <stddef.h>


const struct PPB_TCPSocket_Private_0_4 ppb_tcp_socket_private_interface_0_4 = {
    .Create = NULL,
    .IsTCPSocket = NULL,
    .Connect = NULL,
    .ConnectWithNetAddress = NULL,
    .GetLocalAddress = NULL,
    .GetRemoteAddress = NULL,
    .SSLHandshake = NULL,
    .GetServerCertificate = NULL,
    .AddChainBuildingCertificate = NULL,
    .Read = NULL,
    .Write = NULL,
    .Disconnect = NULL
};
