all: client server adBlock main
	gcc server.o main.o adBlock.o client.o -o main

client: client.c
	gcc -Wall -c -o client.o client.c

server: server.c
	gcc -Wall -c -o server.o server.c

adBlock: adBlock.c
	gcc -Wall -o adBlock.o adBlock.c -Lmyhtml-4.0.0/lib -lmyhtml-4.0.0

main: main.c
	gcc -Wall -c -o main.o main.c

install:
	cd myhtml-4.0.0 && make install

clear:
	rm main *.o
