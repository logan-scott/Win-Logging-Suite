CC=gcc
CFLAGS=-Wall -Wextra -Werror -Wno-missing-field-initializers

keylogger:
	$(CC) $(CFLAGS) -o keylogger keylogger.c

installer:
	$(CC) $(CFLAGS) -o installer installer.c