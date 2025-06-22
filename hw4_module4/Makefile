CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

all: fork outredir allredir argredir piperedir pip2redir

fork: fork.c
	$(CC) $(CFLAGS) -o fork fork.c

outredir: outredir.c
	$(CC) $(CFLAGS) -o outredir outredir.c

allredir: allredir.c
	$(CC) $(CFLAGS) -o allredir allredir.c

argredir: argredir.c
	$(CC) $(CFLAGS) -o argredir argredir.c

piperedir: piperedir.c
	$(CC) $(CFLAGS) -o piperedir piperedir.c

pip2redir: pip2redir.c
	$(CC) $(CFLAGS) -o pip2redir pip2redir.c

clean:
	rm -f fork outredir allredir argredir piperedir pip2redir

.PHONY: all clean 