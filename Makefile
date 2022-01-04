CFLAGS=-g

lecroy_example:		lecroy_example.o lecroy_tcp.o

lecroy_example.o:	lecroy_example.c

lecroy_tcp.o:	lecroy_tcp.c lecroy_tcp.h

clean:
	rm -f *.o lecroy_example
install:
	echo "not going to install anything"
