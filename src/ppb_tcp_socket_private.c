#include <ppapi/c/private/ppb_tcp_socket_private.h>
#include <stddef.h>


const struct PPB_TCPSocket_Private_0_4 ppb_tcp_socket_private_interface_0_4 = {
    .Create = (void *)41,
    .IsTCPSocket = (void *)42,
    .Connect = (void *)43,
    .ConnectWithNetAddress = (void *)44,
    .GetLocalAddress = (void *)45,
    .GetRemoteAddress = (void *)46,
    .SSLHandshake = (void *)47,
    .GetServerCertificate = (void *)48,
    .AddChainBuildingCertificate = (void *)49,
    .Read = (void *)50,
    .Write = (void *)51,
    .Disconnect = (void *)52,
};
