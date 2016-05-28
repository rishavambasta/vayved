all:	vayved

vayved: vayved.c
	mipsel-openwrt-linux-gcc -g vayved.c -o vayved -lpthread
clean:	
	rm -f vayved
