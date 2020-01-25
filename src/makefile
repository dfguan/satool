CC      =  gcc
CFLAGS  =  -g -Wall -D VERBOSE -D PRINT_COVERAGE #-O2  
LDFLAGS = -lz -lm

PROG = satool 

.SUFFIXS:.c .o

all:$(PROG)
	mkdir -p ../bin && cp $(PROG) ../bin

satool: satool.o graph.o sdict.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

.c .o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(PROG) ../bin

	


