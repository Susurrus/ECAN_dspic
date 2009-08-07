# Makefile for Circular Buffer
#
# make             makes ecanTest
# make clean       removes object files
#

ecanTest: ecanTest.o circBuffer.o
	gcc -o ecanTest circBuffer.o ecanTest.o 

ecanTest.o: circBuffer.h ecanTest.c
	gcc -c  -Wall ecanTest.c

circBuffer.o: circBuffer.h circBuffer.c
	gcc -c  -Wall circBuffer.c


clean:
	rm -f ecanTest ecanTest.o circBuffer.o 