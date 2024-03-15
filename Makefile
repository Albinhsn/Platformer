ifdef WSL_DISTRO_NAME
CC := x86_64-w64-mingw32-gcc
CFLAGS := -O2 -g -std=c11 -Wall -Wno-missing-braces -Wno-stringop-truncation -D_DEFAULT_SOURCE -I./src/common -I/usr/local/x86_64-w64-mingw32/include 
LDFLAGS := -L/usr/local/x86_64-w64-mingw32/lib -lm -lopengl32 -lmingw32   -lSDL2main -lSDL2_test -lSDL2
TARGET = main.exe
TARGET_EDITOR = editor.exe
else
CC := gcc
CFLAGS := -O2 -g -std=c11 -Wall -Wno-missing-braces -Wno-stringop-truncation -D_DEFAULT_SOURCE -I./src/common
LDFLAGS := -lm -lGL -lSDL2
TARGET = main
TARGET_EDITOR = editor 
endif


COMMON_SRCS = $(wildcard src/common/*.c)
GAME_SRCS = $(wildcard src/game/*.c)
EDITOR_SRCS = $(wildcard src/editor/*.c)

COMMON_OBJS = $(patsubst src/common/%.c,obj/game/%.o,$(COMMON_SRCS))
GAME_OBJS = $(patsubst src/game/%.c,obj/game/%.o,$(GAME_SRCS))
EDITOR_OBJS = $(patsubst src/editor/%.c,obj/editor/%.o,$(EDITOR_SRCS))


g: $(TARGET)
$(TARGET): $(COMMON_OBJS) $(GAME_OBJS)
	$(CC)  -o $@ $^ $(LDFLAGS)

obj/game/%.o: src/common/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

obj/game/%.o: src/game/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

e: $(TARGET_EDITOR)
$(TARGET_EDITOR): $(COMMON_OBJS) $(EDITOR_OBJS)
	$(CC)  -o $@ $^ $(LDFLAGS)

obj/editor/%.o: src/common/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

obj/editor/%.o: src/editor/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf obj/ $(TARGET)

.PHONY: all clean

len:
	find . -name '*.c' | xargs wc -l
