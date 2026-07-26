PREFIX=/usr/local
INSTALL_DIR=$(PREFIX)/bin

CC = clang
CFLAGS = -std=c99 -Wall -Wextra -Wpedantic -Wunused-value
SOURCES = dtube.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = dtube

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS)

install:
	install -m 0775 $(TARGET) $(INSTALL_DIR)

uninstall:
	rm $(INSTALL_DIR)/$(TARGET)

clean:
	rm -f $(TARGET) $(OBJECTS)
