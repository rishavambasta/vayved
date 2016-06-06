all:	vayved

vayved: vayved_fsm.c
	mipsel-openwrt-linux-gcc -g vayved_fsm.c -o vayved -lpthread
clean:	
	rm -f vayved
