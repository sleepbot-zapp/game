CC = gcc
CFLAGS = -Wall -O2 `pkg-config --cflags raylib`
LDFLAGS = `pkg-config --libs raylib`
SRC = main.c game.c
OUT = flappy

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

clean:
	rm -f $(OUT)
