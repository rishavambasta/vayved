CC=mipsel-openwrt-linux-gcc
#CC=gcc


all:	vayved

vayved: vayved_fsm.c color.o internetChecker.o vayved.h
	$(CC) vayved_fsm.c color.o internetChecker.o -o vayved  -lpthread -ggdb

color.o:  color.c color.h
	$(CC) -c color.c -o color.o -ggdb

internetChecker.o:	internetChecker.c
	$(CC) -c internetChecker.c -o internetChecker.o -ggdb

clean:	
	rm -f vayved *.o
