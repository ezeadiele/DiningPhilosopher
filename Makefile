CC      ?= cc
CFLAGS  ?= -std=c11 -Wall -Wextra -Wpedantic -O2
LDFLAGS ?=
LDLIBS  := -pthread

TARGET  := dining_philosophers
SRC     := dining_philosophers.c

.PHONY: all run clean tsan

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -pthread $(LDFLAGS) $< -o $@ $(LDLIBS)

run: $(TARGET)
	./$(TARGET) 5 3

# Build with ThreadSanitizer to check for data races.
tsan: CFLAGS += -g -fsanitize=thread
tsan: LDFLAGS += -fsanitize=thread
tsan: clean $(TARGET)

clean:
	rm -f $(TARGET)
