#include "game.h"


bool running = true;
int last_frame_time = 0;
float time_delta;
const unsigned char* keys;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;
SDL_Color text_color = {255, 255, 255, 255};

int score_points = 0;
char* score_str;

int snake_length = SNAKE_STARTINGLENGTH;
Node* snake_node_head = NULL;
Node* snake_node_tail = NULL;
Direction snake_direction_current;
Direction snake_direction_previous;

int apple_x;
int apple_y;


void initialize(void) {
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    puts("Error initializing SDL.");
    exit(1);
  }
  if (TTF_Init() < 0) {
    puts("Error initializing TTF.");
    exit(1);
  }
  
  window = SDL_CreateWindow("Game",
			    SDL_WINDOWPOS_CENTERED,
			    SDL_WINDOWPOS_CENTERED,
			    SCR_WIDTH,
			    SCR_HEIGHT,
			    0);
  if (!window) {
    puts("Error initializing window.");
    exit(1);
  }
  
  renderer = SDL_CreateRenderer(window, -1, 0);
  if (!renderer) {
    puts("Error initializing renderer.");
    exit(1);
  }
  
  font = TTF_OpenFont("/home/cenfra/Desktop/SDL-Snake/Uknumberplate-A4Vx.ttf", 24);
  if (!font) {
    printf("Error loading open font: %s\n", TTF_GetError());
    exit(1);
  }

  score_str = (char*)malloc(sizeof(char)*2); // one digit + null terminator
  if (!score_str){
    puts("Error allocating memory for score string.");
    exit(1);
  }

  srand(time(NULL));
}

void setup(void) {
  // initialize snake nodes
  snake_node_head = snake_create_node(SNAKE_STARTINGPOS_X,
				      SNAKE_STARTINGPOS_Y);
  snake_node_tail = snake_create_node(SNAKE_STARTINGPOS_X - snake_length,
				      SNAKE_STARTINGPOS_Y);
  snake_node_tail->previous = snake_node_head;
  snake_node_tail->next = NULL;
  snake_node_head->previous = NULL;
  snake_node_head->next = snake_node_tail;

  snake_direction_current = RIGHT;
  snake_direction_previous = -1;

  apple_update_coords();
}

void update(void) {
    time_delta = (SDL_GetTicks() - last_frame_time) / 1000.0f;
    last_frame_time = SDL_GetTicks();
    // cap framerate
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
        SDL_Delay(time_to_wait);

    // update snake head
    switch (snake_direction_current) {
    case UP:
        snake_node_head->y -= (int)(SNAKE_SPEED * time_delta);
        break;
    case DOWN:
        snake_node_head->y += (int)(SNAKE_SPEED * time_delta);
        break;
    case RIGHT:
        snake_node_head->x += (int)(SNAKE_SPEED * time_delta);
        break;
    case LEFT:
        snake_node_head->x -= (int)(SNAKE_SPEED * time_delta);
        break;
    }

    // update snake tail
    Node* current = snake_node_tail;
    if (current->x < current->previous->x) {
      current->x += (int)(SNAKE_SPEED * time_delta);
    } else if (current->x > current->previous->x) {
      current->x -= (int)(SNAKE_SPEED * time_delta);
    } else if (current->y < current->previous->y) {
      current->y += (int)(SNAKE_SPEED * time_delta);
    } else if (current->y > current->previous->y) {
      current->y -= (int)(SNAKE_SPEED * time_delta);
    }

    // if tail reaches its previous node, remove node
    if (snake_node_tail->x == snake_node_tail->previous->x && snake_node_tail->y == snake_node_tail->previous->y) {
        Node* temp = snake_node_tail->previous;
        temp->previous->next = snake_node_tail;
        snake_node_tail->previous = temp->previous;
        free(temp);
    }

    // check if collision with apple
    if (check_snake_apple_collision()) {
      apple_update_coords();
      snake_enlarge();
      ++score_points;
    }

    if (check_snake_body_collision() || check_snake_border_collision()) {
      // restart score
      score_points = 0;
      // free snake
      Node* current = snake_node_head;
      while (current != NULL) {
	Node* temp = current->next;
	free(current);
	current = temp;
      }
      // restart
      setup();
    }

}

void render(void) {
  // create rectangles/lines/textures and display
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  // set snake color
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  // draw snake colors
  Node* temp = snake_node_head;
  while (temp != NULL && temp->next != NULL) {
    SDL_RenderDrawLine(renderer,
		       temp->x,
		       temp->y,
		       temp->next->x,
		       temp->next->y);
    temp = temp->next; 
  }

  SDL_RenderSetScale(renderer, SCALE, SCALE);

  // snake head point
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderDrawPoint(renderer, snake_node_head->x, snake_node_head->y);

  // render apple
  SDL_Rect rect_apple = {
    .x = apple_x,
    .y = apple_y,
    .w = APPLE_WIDTH,
    .h = APPLE_HEIGHT,
  };
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderFillRect(renderer, &rect_apple);

  // render points
  int num_digits = get_digits_amount(score_points);
  SDL_Rect rect_score = {
    .x = 0,
    .y = 0,
    .w = SCORE_WIDTH * num_digits,
    .h = SCORE_HEIGHT
  };
  score_str = (char*)realloc(score_str, sizeof(char) * num_digits);
  if (score_str == NULL) {
    puts("Error reallocating memory for score string");
    exit(1);
  }
  sprintf(score_str, "%d", score_points);
  SDL_Surface* surface_score;
  surface_score = TTF_RenderText_Solid(font, score_str, text_color);
  SDL_Texture* texture_score_player = SDL_CreateTextureFromSurface(renderer, surface_score);
  SDL_FreeSurface(surface_score);
  SDL_RenderCopy(renderer, texture_score_player, NULL, &rect_score);

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
    case SDL_KEYDOWN: {
      switch (e.key.keysym.sym) {
      case SDLK_w:
	if (snake_direction_current != DOWN)
	  snake_direction_current = UP;
	if (snake_direction_previous != UP && snake_direction_previous != DOWN) {
	  snake_direction_previous = snake_direction_current;
	  snake_insert_node_change_direction();
	}
	break;
      case SDLK_a:
	if (snake_direction_current != RIGHT)
	  snake_direction_current = LEFT;
	if (snake_direction_previous != LEFT && snake_direction_previous != RIGHT) {
	  snake_direction_previous = snake_direction_current;
	  snake_insert_node_change_direction();
	}
	break;
      case SDLK_s:
	if (snake_direction_current != UP)
	  snake_direction_current = DOWN;
	if (snake_direction_previous != UP && snake_direction_previous != DOWN) {
	  snake_direction_previous = snake_direction_current;
	  snake_insert_node_change_direction();
	}
	break;
      case SDLK_d:
	if (snake_direction_current != LEFT)
	  snake_direction_current = RIGHT;
	if (snake_direction_previous != LEFT && snake_direction_previous != RIGHT) {
	  snake_direction_previous = snake_direction_current;
	  snake_insert_node_change_direction();
	}
	break;
      }
      break;
    }
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

// this function inserts a node when the snake changes direction,
// therefore inserting it directly after the snake's head.
void snake_insert_node_change_direction(void) {
  Node* new_node = snake_create_node(snake_node_head->x, snake_node_head->y);
  new_node->next = snake_node_head->next;
  new_node->previous = snake_node_head;
  snake_node_head->next->previous = new_node;
  snake_node_head->next = new_node;
}

void snake_enlarge(void) {
  // to enlarge the snake we will add the snake increment value to the
  // tail coordinates. we first need to check in which direction we
  // need to add the increment length (horizontal or vertical).
  int x = snake_node_tail->x;
  int y = snake_node_tail->y;
  int x_previous = snake_node_tail->previous->x;
  int y_previous = snake_node_tail->previous->y;
  if (x == x_previous) {
    if (y > y_previous) {
      snake_node_tail->y += SNAKE_INCREMENT;
    } else {
      snake_node_tail->y -= SNAKE_INCREMENT;
    }
  } else if (y == y_previous) {
    if (x > x_previous) {
      snake_node_tail->x += SNAKE_INCREMENT;
    } else {
      snake_node_tail->x -= SNAKE_INCREMENT;
    }
  }
}

// returns 1 if snake head collided with apple, 0 otherwise
int check_snake_apple_collision(void) {
  int x = snake_node_head->x;
  int y = snake_node_head->y;
  if (apple_x <= x && x <= apple_x + APPLE_WIDTH && apple_y <= y && y <= apple_y + APPLE_HEIGHT)
    return 1;
  return 0;
}

// returns 1 if the snake's head touches its own body.
int check_snake_body_collision(void) {
  Node* current = snake_node_head -> next;
  int x_head = snake_node_head->x;
  int y_head = snake_node_head->y;
  int x, y, x_next, y_next, coord_min, coord_max;
  while (current->next != NULL) {
    x = current->x;
    y = current->y;
    x_next = current->next->x;
    y_next = current->next->y;

    // if vertical line
    if (x == x_next) {
      coord_min = get_min(y, y_next);
      coord_max = get_max(y, y_next);
      // if snake head is touching line
      if (x_head == x && y_head >= coord_min && y_head <= coord_max) {
	return 1;
      }
    } else if (y == y_next) { // if horizontal line
      coord_min = get_min(x, x_next);
      coord_max = get_max(x, x_next);
      // if snake head is touching line
      if (y_head == y && x_head >= coord_min && x_head <= coord_max) {
	return 1;
      }
    }
    current = current->next;
  }
  return 0;
}

// returns 1 if snake touches any border from the screen.
int check_snake_border_collision(void) {
  int x_head = snake_node_head->x;
  int y_head = snake_node_head->y;
  if (0 > x_head || x_head > SCR_WIDTH/SCALE ||
      0 > y_head || y_head > SCR_HEIGHT/SCALE) {
    return 1;
  }
  return 0;
}

// randomly updates the apple's coordinate.
void apple_update_coords(void) {
  apple_x = rand() % SCR_WIDTH - APPLE_WIDTH;
  apple_y = rand() % SCR_HEIGHT- APPLE_HEIGHT;
  apple_x /= SCALE;
  apple_y /= SCALE;
}

// this function returns the number of digits needed to write a
// number. 314 -> three digits
int get_digits_amount(int number) {
    int digits = 0;
    if (number == 0)
        return 1;
    while (number != 0) {
        number /= 10;
        digits++;
    }
    return digits;
}

int get_min(int num1, int num2) {
  return (num1 > num2) ? num2 : num1;
}
int get_max(int num1, int num2) {
  return (num1 > num2) ? num1 : num2;
}
