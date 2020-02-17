both: server.x client.x


server.x : main.o
        gcc main.o -o server.x

client.x : client.o
        gcc client.o -o client.x

client.o : client.c
        gcc -g -Wall -c client.c

main.o : main.c
        gcc -g -Wall -c main.c

clean : 
        rm *.x *.o