CC := gcc
CFLAGS := -O2 -g -std=c11 -Wall -Wno-missing-braces -Wno-stringop-truncation -D_DEFAULT_SOURCE
TARGET := main 
LDFLAGS := -ldl -lSDL2 -lm -lz -lGL

SRCS := $(wildcard ./src/*.c)
OBJS := $(patsubst ./src/%.c,./obj/%.o,$(SRCS)) $(patsubst ./lib/%.c,./obj/%.o,$(LIBS))

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBRARIES)
./obj/%.o: ./src/%.c
	$(CC) $(CFLAGS) $(LIBRARIES)  -c $<  && mv *.o ./obj/
./obj/%.o: ./lib/%.c
	$(CC) $(CFLAGS) $(LIBRARIES)  -c $<  && mv *.o ./obj/
clean:
	rm -rf $(TARGET) ./obj/*.o

len:
	find . -name '*.c' | xargs wc -l


.PHONY: all clean

