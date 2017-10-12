CC=gcc

CFLAGS=-Wall -Wextra -Ofast
LFLAGS=-s

OBJS=calendar.o linkedlist.o main.o stringarray.o strings.o userui.o
DEPS=calendar.h linkedlist.h stringarray.h strings.h userui.h
LIBS=

BIN=calendar

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BIN): $(OBJS)
	$(CC) -o $@ $^ $(LFLAGS) $(LIBS)

clean:
	rm -f $(OBJS) $(BIN)
