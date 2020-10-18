BINARIO = servidor

all: servidor

servidor: servidor.o
	cc -o $(BINARIO) servidor.o

servidor.o: servidor.c
	cc -c servidor.c

clean: 
	rm -f servidor.o $(BINARIO)