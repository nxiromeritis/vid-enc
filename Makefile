CC = gcc
CFLAGS = -Wall -g
OBJ1 = luts.o lutbuilder.o

all: luts

luts: $(OBJ1)
	$(CC) $(OBJ1) -o luts -lm

%.o: %.c lutbuilder.h
	$(CC) $(CFLAGS) -c $< -lm


.PHONY: clean
clean:
	@echo Deletting .o files...
	rm -f *.o
