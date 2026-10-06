CC = gcc
CFLAGS = -O2 -Wall -Wextra
TARGET = lines
SRC = lines.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean
