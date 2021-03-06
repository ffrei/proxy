all: utils adBlock client server main
	gcc utils.o server.o main.o adBlock.o client.o -o main -Lmyhtml-4.0.0/lib -lmyhtml-4.0.0

utils: utils.c
	gcc -Wall -c -o utils.o utils.c

adBlock: adBlock.c
	gcc -Wall -c -o adBlock.o adBlock.c

client: client.c
	gcc -Wall -c -o client.o client.c

server: server.c
	gcc -Wall -c -o server.o server.c

main: main.c
	gcc -Wall -c -o main.o main.c

install:
	cd myhtml-4.0.0 && make install

clear:
	rm main *.o
