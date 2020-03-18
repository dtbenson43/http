both: server.x client.x

server.x : fileio.o server.o
	gcc server.o fileio.o -o server.x

client.x : client.o
	gcc client.o -o client.x

client.o : client.c
	gcc -Wall -c -g client.c

server.o : server.c fileio.h
	gcc -Wall -c -g server.c

fileio.o : fileio.h fileio.c
	gcc -Wall -c -g fileio.c

clean :
	rm *.x *.o
