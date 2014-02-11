CFLAG=-Wall
	PROG = simulator
	OBJS = graph.c simulator.c terminal.c
	CC = gcc

all: $(OBJS)
	gcc -o $(PROG) $(OBJS)

clean:
	@rm -r $(PROG)
