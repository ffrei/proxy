all: main

main: main.c
	gcc -Wall -o main main.c

clear:
	rm main
