CC := clang

CFLAGS := -std=c17 -g -Wall -Wextra
LIBS := -lbsd

pl0c: main.c
	$(CC) main.c $(CFLAGS) $(LIBS) -o pl0c

.PHONY: clean

clean:
	rm -f pl0c
