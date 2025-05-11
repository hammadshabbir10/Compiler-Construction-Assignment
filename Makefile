CC = gcc
CFLAGS = -Wall -Wextra -g
LEX = flex
YACC = bison

TARGET = json2relcsv

OBJS = parser.o scanner.o ast.o tables.o main.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

parser.c parser.h: parser.y
	$(YACC) -d -o parser.c parser.y

scanner.c: scanner.l parser.h
	$(LEX) -o scanner.c scanner.l

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(TARGET) $(OBJS) parser.c parser.h scanner.c

.PHONY: all clean
