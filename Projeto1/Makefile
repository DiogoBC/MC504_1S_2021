CFLAGS = -std=c99 -Wall -Werror -Werror=vla -g
LDFLAGS = -lm -lfreeimage -lpthread

.PHONY: all clean

all: projeto1

projeto1: *.c
	gcc $(CFLAGS) *.c -o projeto1 $(LDFLAGS)

clean:
	rm -f projeto1s