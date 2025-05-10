OBJS = spchk.o
SOURCE = spchk.c
HEADER = 
OUT = spchk
CC = gcc
CFLAGS = -g -c -Wall
LFLAGS = 

# Target to build the executable
all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

# Compile the source file into an object file
spchk.o: spchk.c $(HEADER)
	$(CC) $(CFLAGS) spchk.c

# Clean target to remove object files and the executable
clean:
	rm -f $(OBJS) $(OUT)