CC = gcc
CFLAGS = -O2 -Wall -Wextra
TARGET = task5
SRC = task5.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean
