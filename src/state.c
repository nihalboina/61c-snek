#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Task 1 */
game_state_t* create_default_state() {
  game_state_t *nstate = malloc(sizeof(game_state_t));
  nstate->num_rows = 18;
  nstate->snakes = malloc(sizeof(snake_t) * 1);
  nstate->snakes->head_row = 2;
  nstate->snakes->head_col = 4;
  nstate->snakes->live = true;
  nstate->snakes->tail_row = 2;
  nstate->snakes->tail_col = 2;

  nstate->board = malloc(sizeof(char*) * nstate->num_rows);
  //nstate->board[x] = (char*)(malloc(sizeof(char*) * 21));

  for(int x = 0; x < 18; x++){
    nstate->board[x] = (char*)(malloc(sizeof(char) * 21));
    if (x >= 1 && x <= nstate->num_rows - 2) {
      strcpy(nstate->board[x], "#                  #\0");
    } else {
      strcpy(nstate->board[x], "####################\0");
    }
   // printf("%s\n", nstate->board[x]);
  }
  nstate->num_snakes = 1;
  set_board_at(nstate, 2, 4, 'D');
  set_board_at(nstate, 2, 3, '>');
  set_board_at(nstate, 2, 2, 'd');
  nstate->board[2][9] = '*';

  //for (int x = 0; x < 18; x++) {
    //printf("%s\n", nstate->board[x]);
  //}

  return nstate;
}


/* Task 2 */
void free_state(game_state_t* state) {
  // TODO: Implement this function
    if (state == NULL) {
        return;
    }

    free(state->snakes);

    if (state->board != NULL) {
        for (unsigned int i = 0; i < state->num_rows; ++i) {
            free(state->board[i]);
        }
        free(state->board);
    }

    free(state);
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
  // TODO: Implement this function.
	for(int row = 0; row < state->num_rows; row++){
		fprintf(fp,"%s\n", state->board[row]);
	}
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
	return;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
	if(c == 'w' || c == 'a' || c == 's' || c == 'd'){
		return true;
	}
  return false;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
        if(c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x'){
                return true;
        }
  return false;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
        if(c == '^' || c == '<' || c == 'v' || c == '>' || is_tail(c) || is_head(c)){
                return true;
        }
  return false;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
	if(c=='^'){
		return 'w';
	}
	if(c=='<'){
		return 'a';
	}
	if(c=='v'){
		return 's';
	}
	if(c=='>'){
		return 'd';
	}
  return '?';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
	if(c=='W'){
                return '^';
        }
        if(c=='A'){
                return '<';
        }
        if(c=='S'){
                return 'v';
        }
        if(c=='D'){
                return '>';
        }
  return '?';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
	if(c=='^' || c == 'w' ||  c == 'W'){
                return cur_row-1;
        }
	if(c=='v' || c == 's' || c == 'S'){
                return cur_row+1;
        }
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  	if(c=='>' || c == 'd' || c == 'D'){
                return cur_col+1;
        }
        if(c=='<' || c == 'a' || c == 'A'){
                return cur_col-1;
        }
	return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
	snake_t* snake = &(state->snakes[snum]);
	char cur_head = state->board[snake->head_row][snake->head_col];
	unsigned int next_row = get_next_row(snake->head_row,cur_head);
	unsigned int next_col = get_next_col(snake->head_col,cur_head);
	return get_board_at(state,next_row,next_col);
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
	snake_t* snake = &(state->snakes[snum]);
        char cur_head = state->board[snake->head_row][snake->head_col];
        unsigned int next_row = get_next_row(snake->head_row,cur_head);
        unsigned int next_col = get_next_col(snake->head_col,cur_head);
	set_board_at(state,snake->head_row,snake->head_col,head_to_body(cur_head));
	snake->head_row = next_row;
	snake->head_col = next_col;
	set_board_at(state,next_row,next_col,cur_head);
  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
	snake_t* snake = &(state->snakes[snum]);
        char cur_tail = state->board[snake->tail_row][snake->tail_col];
        unsigned int next_row = get_next_row(snake->tail_row,cur_tail);
        unsigned int next_col = get_next_col(snake->tail_col,cur_tail);
	set_board_at(state,snake->tail_row,snake->tail_col,' ');
	snake->tail_row = next_row;
        snake->tail_col = next_col;
	set_board_at(state,next_row,next_col,body_to_tail(get_board_at(state,next_row,next_col)));
  return;
}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  // TODO: Implement this function
	unsigned int snum = 0;
	while(snum < state->num_snakes){
		snake_t* snake = &(state->snakes[snum]);
        	char cur_head = get_board_at(state,snake->head_row,snake->head_col);
		if (is_head(cur_head)){
        		unsigned int next_row = get_next_row(snake->head_row,cur_head);
        		unsigned int next_col = get_next_col(snake->head_col,cur_head);
			char next_head = get_board_at(state,next_row,next_col);
			if(next_head == '#' || is_snake(next_head)){
				set_board_at(state,snake->head_row,snake->head_col,'x');
				snake->live = false;
			}else{
				if(next_head == '*'){
					update_head(state,snum);
					add_food(state);
				}else{
					update_tail(state,snum);
					update_head(state,snum);
			}
			snake->head_row = next_row;
              		snake->head_col = next_col;
			}
		}
		snum++;
	}
	return;
}

/* Task 5 */
game_state_t* load_board(FILE* fp) {
  // TODO: Implement this function.
	unsigned int rows = 0;
	game_state_t* state = malloc(sizeof(game_state_t));
	state->board = NULL;
	char cur = ' ';
	while(cur != EOF){
		cur = (char)fgetc(fp);
		if (cur == EOF){
			break;
		}else{
		unsigned int cols = 0;
		state -> board = realloc(state->board, sizeof(char*)*(rows+1));
                state -> board[rows] = malloc(sizeof(char)*(cols + 1));
		while(cur != '\n'){
			char* temp = (char *) realloc(state-> board[rows], sizeof(char)*(cols+2));
			state->board[rows] = temp;
			if (cur == '\n'){
				break;
			}else{
				state->board[rows][cols] = cur;
				cols++;
				cur = (char)fgetc(fp);
			}
		}
		state->board[rows][cols] = '\0';
		rows++;
		}
	}
	state->num_rows = rows;
	state->num_snakes = 0;
	state->snakes = NULL;
	fclose(fp);
	return state;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
 snake_t* snake = &(state->snakes[snum]);
        char cur_tail = state->board[snake->tail_row][snake->tail_col];
        unsigned int next_row = get_next_row(snake->tail_row,cur_tail);
        unsigned int next_col = get_next_col(snake->tail_col,cur_tail);
        while(is_snake(get_board_at(state,next_row,next_col)) && !is_head(get_board_at(state, next_row,next_col))){
                cur_tail = state->board[next_row][next_col];
                next_row = get_next_row(next_row,cur_tail);
                next_col = get_next_col(next_col,cur_tail);
        }
        snake->head_row = next_row;
        snake->head_col = next_col;
 
return;
}

/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implement this function.
	unsigned int num_snakes = 0;
	snake_t* snakes  = malloc(sizeof(snake_t*)*num_snakes);
	for(unsigned int row = 0; row < state->num_rows; row++){
		unsigned int col = 0;
		while(get_board_at(state,row,col)!='\0'){
			if(is_tail(get_board_at(state,row,col))){
				num_snakes++;
                                snake_t* snake  = malloc(sizeof(snake_t*)*1);
                                snake->tail_row = row;
                                snake->tail_col = col;
                                snake->head_col = 0;
                                snake->head_row = 0;
                                snake->live = true;
                                snake_t* temp = realloc(snakes,sizeof(snake_t)*(num_snakes));
                                temp[num_snakes-1] = *snake;
                                snakes = temp;
			}
			col++;
		}
	}
	state->snakes = snakes;
	for(unsigned int snum = 0; snum<num_snakes; snum++){
		find_head(state,snum);
	}
	state->num_snakes = num_snakes;
	
  return state;
}
