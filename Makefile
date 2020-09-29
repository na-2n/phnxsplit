#
# Main Makefile
# (c) 2002
#

CC = gcc

phnxsplit: lzint_decode.o phnxfunc.o
	${CC} phnxsplit.c lzint_decode.o phnxfunc.o -s -fpack-struct -o phnxsplit

lzint_decode:
	${CC} lzint_decode.c -c -o lzint_decode.o -fpack-struct

phnxfunc:
	${CC} phnxfunc.c -c -o phnxfunc.o -fpack-struct

clean: 
	rm -f *.o
	rm -f phnxsplit
	rm -f phnxsplit.exe

rebuild:
	make clean
	make lzint_decode
	make phnxfunc
	make phnxsplit

