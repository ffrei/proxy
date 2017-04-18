all: client server main
	gcc server.o main.o client.o -o main

client: client.c
		gcc -Wall -c -o client.o client.c

server: server.c
	gcc -Wall -c -o server.o server.c

main: main.c
	gcc -Wall -c -o main.o main.c

clear:
	rm main *.o
