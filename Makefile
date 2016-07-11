OBJS = aulasm2.o asmkernel.o
CC = gcc
#CFLAGS = -O2 -DSIMPLE_FPOS_T
CFLAGS = -O2
BIN=aulasm.bin

$(BIN): $(OBJS)
	$(CC) $^ -o $@

aulasm2.o : aulasm2.c aulasm2.h
	$(CC) $(CFLAGS) -c $< -o $@

asmkernel.o : asmkernel.c asmkernel.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BIN) $(OBJS)
