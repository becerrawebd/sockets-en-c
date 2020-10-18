BINARIO = cliente

all: cliente

cliente: cliente.o
	cc -o $(BINARIO) cliente.o

cliente.o: cliente.c
	cc -c cliente.c

clean:
	rm -f *.o $(BINARIO)