#include "iface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <net/if.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>



int up(interface *iface) {
    if (iface->state == 1) {
        return 0;
    }
    iface->iface_socket = -1;
    char *cmd = (char*)malloc(13 * sizeof(char));

    sprintf(cmd , "ip link set dev %s up", (iface->name));
    int retCode;
    if (retCode = (system(cmd)) != 0){
        fprintf(stderr, "failed to up interface using ip link");
        return retCode;
    }

    iface->iface_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (iface->iface_socket < 0) {
        fprintf(stderr, "failed to initialize interface %s socket\n", iface->name);
        return iface->iface_socket;
    }

    struct sockaddr_ll socket_address;
    memset(&socket_address, 0, sizeof (socket_address));
    socket_address.sll_family = PF_PACKET;
    socket_address.sll_ifindex = if_nametoindex(iface->name);
    socket_address.sll_protocol = htons(ETH_P_ALL);
    retCode = bind(iface->iface_socket, (struct sockaddr*)&socket_address, sizeof(socket_address));
    if (retCode != 0) {
        fprintf(stderr, "failed to bind socket to interface %s\n", iface->name);
        return retCode;
    }

    return 0;
}

int down(interface *iface) {
    if (iface->state == 0) {
        return 0;
    }
    
    char *cmd = (char*)malloc(30 * sizeof(char));
    sprintf(cmd , "ip link set dev %s down", (iface->name));
    int retCode;
    if (retCode = (system(cmd)) != 0){
        fprintf(stderr, "failed to down interface using ip link");
        return retCode;
    }

    if (retCode = (close(iface->iface_socket)) == 0) {
        iface->state == 0;
        return 0;
    }
    return retCode;
}

int iface_send(interface *iface, char * buffer[], int buffer_size) {
    return send(iface->iface_socket, buffer, buffer_size, 0);
}

int iface_recv(interface *iface, char * buffer[], int buffer_size) {
    return recv(iface->iface_socket, buffer, buffer_size, 0);
}
