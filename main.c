#include "dataplane/iface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>


interface *iface_h1_sw, *iface_h2_sw;

interface * initialize_interface(char *name) {
    interface *iface = malloc(sizeof(interface));
    iface->name = strdup(name);
    iface->iface_socket = -1;
    iface->state = 0;
    return iface;
}


interface * get_out_port(char *buf, int buf_size, interface *in_port) {
    // TODO
    printf("DST MAC: %X:%X:%X:%X:%X:%X\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
    printf("SRC MAC: %X:%X:%X:%X:%X:%X\n", buf[6], buf[7], buf[8], buf[9], buf[10], buf[11]);
    
    if (in_port == NULL) {
        return NULL;
    }

    if (in_port->name == "h1-sw") {
        return iface_h2_sw;
    } else if (in_port->name == "h2-sw") {
        return iface_h1_sw;
    }

    printf("no matching interface for out_port.\n");
    return NULL;
}


int main(int argc, char *argv[]) {
    interface *iface_h1_sw = initialize_interface("h1-sw");
    interface *iface_h2_sw = initialize_interface("h2-sw");
    int retCode;
    retCode = up(iface_h1_sw);
    printf("\nresult of interface h1-sw up is %d\n", retCode);
    retCode = up(iface_h2_sw);
    printf("\nresult of interface h2-sw up is %d\n", retCode);
    

    fd_set read_fds;
    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 500000;

    

    __time_t delay = 60;
    __time_t start_time = time(NULL);

    while (time(NULL) - start_time < delay) {
        FD_ZERO(&read_fds);
        FD_SET(iface_h1_sw->iface_socket, &read_fds);
        FD_SET(iface_h2_sw->iface_socket, &read_fds);
        retCode = select(FD_SETSIZE, &read_fds, NULL, NULL, &tv);
        if (retCode <= 0) {
            printf("Failed to select socket descriptor");
            goto end;
        }
        printf("select is ok\n");

        ssize_t recv_size = -1;
        ssize_t send_size = -1;

        if (FD_ISSET(iface_h1_sw->iface_socket, &read_fds)) {
            // h1-sw forward to h2-sw
            printf("recieve from h1-sw is ready. receiving now...");
            char buf[1500];
            memset(&buf, 0, sizeof(buf));
            recv_size = iface_recv(iface_h1_sw, &buf, sizeof(buf));
            if (recv_size == -1)
            {
                perror("error in Socket receive\n");
                exit(0);
            }
            printf("recieved %d bytes\n", recv_size);

            get_out_port(buf, recv_size, NULL);

            send_size = iface_send(iface_h2_sw, &buf, recv_size);
            if (send_size == -1) {
                perror("error in Socket send\n");
                exit(0);
            }
            printf("sent %d bytes\n", sizeof(buf));
        } else {
            printf("h1-sw is not ready for recieve.\n");
        }

        if (FD_ISSET(iface_h2_sw->iface_socket, &read_fds)) {
            // h2-sw forward to h1-sw
            printf("recieve from h2-sw is ready. receiving now...");
            char buf[1500];
            memset(&buf, 0, sizeof(buf));
            recv_size = iface_recv(iface_h2_sw, &buf, recv_size);
            if (recv_size == -1)
            {
                perror("error in Socket receive\n");
                exit(0);
            }
            printf("recieved %d bytes\n", recv_size);

            get_out_port(buf, recv_size, NULL);

            send_size = iface_send(iface_h1_sw, &buf, sizeof(buf));
            if (send_size == -1) {
                perror("error in Socket send\n");
                exit(0);
            }
            printf("sent %d bytes\n", sizeof(buf));
        } else {
            printf("h2-sw is not ready for recieve.\n");
        }
    }

end:
    retCode = down(iface_h1_sw);
    retCode = down(iface_h2_sw);
    printf("\nresult of interface down is %d\n", retCode);
    return 0;
}