CC=gcc

WIN_CDEFINES=-D__MSVCRT_VERSION__=__MSVCR80_DLL
CFLAGS=-Wall -Wextra -Ofast $(WIN_CDEFINES)
LFLAGS=-s

OBJS=calendar.o event.o eventmanager.o linkedlist.o main.o stringarray.o strings.o userui.o
DEPS=calendar.h event.h eventmanager.h linkedlist.h stringarray.h strings.h userui.h
LIBS=

BIN=calendar

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BIN): $(OBJS)
	$(CC) -o $@ $^ $(LFLAGS) $(LIBS)

clean:
	rm -f $(OBJS) $(BIN)
