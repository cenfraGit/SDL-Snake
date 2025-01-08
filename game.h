#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdlib.h>

#define SCR_WIDTH 800
#define SCR_HEIGHT 600
#define FPS 60
#define FRAME_TARGET_TIME (1000/FPS)

#define SCALE 4

#define SNAKE_SPEED 60
#define SNAKE_STARTINGLENGTH 10
#define SNAKE_INCREMENT 5
#define SNAKE_STARTINGPOS_X 30
#define SNAKE_STARTINGPOS_Y 30

#define SCORE_WIDTH 10
#define SCORE_HEIGHT 10

#define APPLE_WIDTH 10
#define APPLE_HEIGHT 10

typedef struct Node {
  int x;
  int y;
  struct Node* next;
  struct Node* previous;
} Node;

typedef enum {
  UP,
  DOWN,
  RIGHT,
  LEFT
} Direction;

void initialize(void);
void setup(void);
void update(void);
void render(void);
void kill(void);
void process_user_input(void);
Node* snake_create_node(int x, int y);
void snake_insert_node_change_direction(void);
void snake_enlarge(void);
int check_snake_apple_collision(void);
int check_snake_body_collision(void);
int check_snake_border_collision(void);
void apple_update_coords(void);
int get_digits_amount(int number);
int get_min(int num1, int num2);
int get_max(int num1, int num2);

extern bool running;
extern int last_frame_time;
extern float time_delta;
extern const unsigned char* keys;
extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern TTF_Font* font;
extern SDL_Color text_color;

extern int score_points;
extern char* score_str;

extern int snake_length;
extern Node* snake_node_head;
extern Node* snake_node_tail;
extern Direction snake_direction_current;
extern Direction snake_direction_previous;

extern int apple_x;
extern int apple_y;

#endif
