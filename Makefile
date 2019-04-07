CFLAGS=-g -Wall -Wextra
LIB=-L. -pthread -rdynamic
INC=-I.
SRC=main.c vdata.c vcom.c vnet.c sample.c

all:
	gcc -o exe $(CFLAGS) $(INC) $(SRC) $(LIB) 
clean:
	rm -rf exe

