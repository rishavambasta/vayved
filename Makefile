CC=mipsel-openwrt-linux-gcc
#CC=gcc
GDB_FLAGS=-ggdb


all:	vayved

vayved:	vayved_fsm.o color.o internetChecker.o
	$(CC) vayved_fsm.o color.o internetChecker.o -o vayved -lpthread $(GDB_FLAGS)

vayved_fsm.o: vayved_fsm.c vayved.h
	$(CC) -c vayved_fsm.c -o vayved_fsm.o $(GDB_FLAGS)

color.o:  color.c color.h
	$(CC) -c color.c -o color.o $(GDB_FLAGS)

internetChecker.o:	internetChecker.c
	$(CC) -c internetChecker.c -o internetChecker.o $(GDB_FLAGS)

clean:	
	rm -f vayved *.o
