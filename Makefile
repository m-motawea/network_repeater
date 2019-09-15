main: main.o iface.o
	gcc -o main main.o dataplane/iface.o

main.o: main.c dataplane/iface.h
	gcc -c main.c

iface.o: dataplane/iface.c dataplane/iface.h
	gcc -c dataplane/iface.c -o dataplane/iface.o


