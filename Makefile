OBJECTS = cli.c srv.c 
CC = gcc

all: $(OBJECTS)
	$(CC) -o cli cli.c 
	$(CC) -o srv srv.c

clean: 
	rm -rf cli
	rm -rf srv






