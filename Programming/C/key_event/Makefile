CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c11
LDFLAGS=

SRCS=my_test.c
OBJS=$(SRCS:.c=.o)
EXEC=my_test

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)
