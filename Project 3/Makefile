CFLAGS= -g -fpermissive
LDFLAGS= -lpthread -lrt
LDFLAGS1= -lpthread -fpermissive
CC=g++ -pthread

all: BoundedBuffer Bridge profstud

# To make an executable
BoundedBuffer: BoundedBuffer.o 
	$(CC) $(LDFLAGS) -o BoundedBuffer BoundedBuffer.o

bridge: bridge.o
	$(CC) $(LDFLAGS1) -o Bridge Bridge.o

profstud: profstud.o
	$(CC) $(LDFLAGS) -o profstud profstud.o

# To make an object from source
.c.o:
	$(CC) $(CFLAGS) -c $*.c

# clean out the dross
clean:
	-rm boundedBuffer bridge profstud *.o

