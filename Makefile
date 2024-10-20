CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=gnu18
CDBGFLAGS = -Wall -Wextra -Werror -pedantic -std=gnu18 -g -fsanitize=address

.PHONY: all clean barber barber-dbg

all: barber barber-dbg

barber: barber.c barber.h
	$(CC) $(CFLAGS) -O2 $< -o $@

barber-dbg: barber.c barber.h
	$(CC) $(CDBGFLAGS) -Og -ggdb $< -o $@

clean:
	rm -f barber barber-dbg

