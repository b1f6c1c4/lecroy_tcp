CFLAGS=-g

lecroy_example:		lecroy_example.o lecroy_tcp.o
	g++ $(CFLAGS) lecroy_example.o lecroy_tcp.o -o lecroy_example

lecroy_example.o:	lecroy_example.c
	g++ $(CFLAGS) -c lecroy_example.c lecroy_tcp.c

lecroy_tcp.o:	lecroy_tcp.c lecroy_tcp.h
	g++ $(CFLAGS) -c lecroy_tcp.c
clean:	
	rm *.o
install:
	echo "not going to install anything"
