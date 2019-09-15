typedef struct interface {
    char * name;
    int state;
    int iface_socket;
} interface;

int up(interface *iface);
int down(interface *iface);
int iface_send(interface *iface, char * buffer[], int buffer_size);
int iface_recv(interface *iface, char * buffer[], int buffer_size);
