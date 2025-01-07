CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lSDL2 -lSDL2_ttf

TARGET = game
SRCS = main.c game.c
OBJS = $(SRCS:.c=.o)

.PHONY = all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
