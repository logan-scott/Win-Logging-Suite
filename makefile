CC=gcc
CFLAGS=-Wall -Wextra -Werror

cache:
	$(CC) $(CFLAGS) -o keylogger keylogger.c