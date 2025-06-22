CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lrt

TARGETS = endless proc killn catchsig childctl

all: $(TARGETS)

endless: endless.c
	$(CC) $(CFLAGS) -o endless endless.c

proc: proc.c
	$(CC) $(CFLAGS) -o proc proc.c

killn: killn.c
	$(CC) $(CFLAGS) -o killn killn.c

catchsig: catchsig.c
	$(CC) $(CFLAGS) -o catchsig catchsig.c

childctl: childctl.c
	$(CC) $(CFLAGS) -o childctl childctl.c

clean:
	rm -f $(TARGETS)

.PHONY: all clean 