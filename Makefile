CC = gcc
CFLAGS = -Wall -Wextra -g

shell: shell.c
	$(CC) $(CFLAGS) -o shell shell.c

clean:
	rm -f shell

.PHONY: clean
