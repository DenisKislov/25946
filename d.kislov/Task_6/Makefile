CC = gcc
CFLAGS = -O2 -Wall -Wextra
TARGET = task6
SRC = task6.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean
