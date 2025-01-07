#include "game.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;
SDL_Color text_color = {255, 255, 255, 255};
bool running = true;
const unsigned char* keys;
int last_frame_time = 0;
float time_delta;

int snake_length = 40;
Node* snake_link_head = NULL;
Direction snake_direction_head = RIGHT;

int initialize(void) {
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    puts("Error initializing SDL.");
    return 1;
  }
  if (TTF_Init() < 0) {
    puts("Error initializing TTF.");
    return 1;
  }
  
  window = SDL_CreateWindow("Game",
			    SDL_WINDOWPOS_CENTERED,
			    SDL_WINDOWPOS_CENTERED,
			    SCR_WIDTH,
			    SCR_HEIGHT,
			    0);
  if (!window) {
    puts("Error initializing window.");
    return 1;
  }
  
  renderer = SDL_CreateRenderer(window, -1, 0);
  if (!renderer) {
    puts("Error initializing renderer.");
    return 1;
  }
  
  font = TTF_OpenFont("/home/cenfra/Desktop/SDL-Snake/Uknumberplate-A4Vx.ttf", 24);
  if (!font) {
    printf("Error loading open font: %s\n", TTF_GetError());
    return 1;
  }

  // initialize snake
  snake_link_head = snake_create_node(SNAKE_STARTINGPOS_X,
				      SNAKE_STARTINGPOS_Y);
  snake_link_head->next = snake_create_node(SNAKE_STARTINGPOS_X,
					    SNAKE_STARTINGPOS_Y + snake_length);

  return 0;
}

void setup(void) {

}

void update(void) {
  time_delta = (SDL_GetTicks() - last_frame_time) / 1000.0f;
  last_frame_time = SDL_GetTicks();
  // cap framerate
  int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);
  if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
    SDL_Delay(time_to_wait);

  // update snake head
  switch (snake_direction_head) {
  case RIGHT:
    snake_link_head->x += SNAKE_SPEED * time_delta;
    break;
  }

  // update head and tail
  //Node* temp = snake_link_head;

}

void render(void) {
  // create rectangles/lines/textures and display
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  // set snake color
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  // draw snake colors
  Node* temp = snake_link_head;
  while (temp != NULL && temp->next != NULL) {
    SDL_RenderDrawLine(renderer,
		       temp->x,
		       temp->y,
		       temp->next->x,
		       temp->next->y);
    temp = temp->next; 
  }

  SDL_RenderPresent(renderer);
}

void kill(void) {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void process_user_input(void) {

  SDL_Event e;

  while (SDL_PollEvent(&e)) {

    switch (e.type) {
    case SDL_QUIT:
      running = false;
      break;
    case SDL_KEYDOWN:
      switch (e.key.keysym.sym) {
      case SDLK_q:
	running = false;
	break;
      }
      break;
    case SDL_KEYUP:
      switch (e.key.keysym.sym) {
      case SDLK_w:
	break;
      }
      break;
    }
  }
}

Node* snake_create_node(int x, int y) {
  Node* new_node = (Node*)malloc(sizeof(Node));
  if (!new_node) {
    puts("Error allocating memory for new node.");
    exit(1);
  }
  new_node->x = x;
  new_node->y = y;
  new_node->next = NULL;
  return new_node;
}

void snake_free_links(void) {
  Node* temp;
  while (snake_link_head != NULL) {
    temp = snake_link_head;
    snake_link_head = snake_link_head->next;
    free(temp);
  }
}
