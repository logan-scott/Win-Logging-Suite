CC=gcc
CFLAGS=-Wall -Wextra -Werror -Wno-missing-field-initializers

cache:
	$(CC) $(CFLAGS) -o keylogger keylogger.c