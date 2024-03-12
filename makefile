TARGET := b6502s
DEBUG := b6502s_dbg

TEST := test
TEST_DBG := test_dbg

CC := gcc

DEBUG_FLAGS := -g -fsanitize=address -fsanitize=object-size -fno-optimize-sibling-calls -fsanitize=undefined -fsanitize=leak -fsanitize=alignment
CFLAGS := -O1 -g -std=gnu17 -Wall -Wextra -Wpedantic -Wno-unused-parameter 
LDFLAGS := -lcurses

SRCS := src/*.c

all: $(TARGET)

$(TARGET):  $(SRCS) main.c
	$(CC) $(CFLAGS) $^ -o $(TARGET) $(LDFLAGS)
$(DEBUG) :   $(SRCS) main.c
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $^ -o $(DEBUG) $(LDFLAGS)


$(TEST): $(SRCS)  test.c
	$(CC) $(CFLAGS) $^ -o $(TEST) $(LDFLAGS)
$(TEST_DBG): $(SRCS) test.c
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $^ -o $(TEST) $(LDFLAGS)


clean : 
	rm -f $(TARGET) $(DEBUG) $(TEST) $(TEST_DBG)