#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdlib.h>

#define SCR_WIDTH 800
#define SCR_HEIGHT 600
#define FPS 60
#define FRAME_TARGET_TIME (1000/FPS)

#define SNAKE_SPEED 60
#define SNAKE_STARTINGPOS_X 30
#define SNAKE_STARTINGPOS_Y 30

typedef struct Node {
  int x;
  int y;
  struct Node* next;
} Node;

typedef enum {
  UP,
  DOWN,
  RIGHT,
  LEFT
} Direction;

int initialize(void);
void setup(void);
void process_user_input(void);
void update(void);
void render(void);
void kill(void);
Node* snake_create_node(int x, int y);
void snake_free_links(void);

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern TTF_Font* font;
extern SDL_Color text_color;
extern bool running;
extern const unsigned char* keys;
extern int last_frame_time;
extern float time_delta;

extern int snake_length;
extern Node* snake_link_head;
extern Direction snake_direction_head;

#endif
