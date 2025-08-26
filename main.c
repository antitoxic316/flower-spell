#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <ncurses.h>

#include "image.h"

struct reactive_keys_args {
	bool q_key_pressed;
};


void* user_input_reader(void *argv);
char** init_char_map(uint32_t w, uint32_t h);
void show_char_map(char**, uint32_t w, uint32_t h);


int main(int argc, char *argv[]){
    initscr();    

    char** char_map = init_char_map(COLS, LINES);

    //char_map = img_to_ascii(char_map, COLS, LINES);

    struct reactive_keys_args key_args;    
    key_args.q_key_pressed = false;

	while(true){
        show_char_map(char_map, COLS, LINES);

        refresh();

        pthread_t thread_io;
        pthread_create(&thread_io, NULL, &user_input_reader, (void *)&key_args);
        pthread_join(thread_io, NULL);
        if (key_args.q_key_pressed){
            break; 
        }
    }
    
    endwin();
    return 0;
}


void* user_input_reader(void *argv){
    setvbuf(stdin, NULL, _IONBF, 0);

    struct reactive_keys_args *args = (struct reactive_keys_args *)argv;

    char input_reading;

    scanf("%c", &input_reading);
    fflush(stdin);
    if (input_reading == 'q')
        //printf("%d\n", args -> q_key_pressed);
        args -> q_key_pressed = true;
    
    pthread_exit(NULL);
}


char** init_char_map(uint32_t w, uint32_t h){
    char **char_map = malloc(h * sizeof(char*));
    
    for(int i = 0; i < h; i++){
        char *initiated_line = calloc(w, sizeof(char));
        char_map[i] = initiated_line;
    }

    return char_map;
}

void show_char_map(char** char_map, uint32_t w, uint32_t h){
    //bitmap cropping to termianal size
    uint32_t w_drw_lim = w;
    uint32_t h_drw_lim = h;
    if(w > COLS){
        w_drw_lim = COLS;
    }
    if(h > LINES){
        h_drw_lim = LINES;
    }

    for(int i = 0; i < h_drw_lim; i++){
        for(int j = 0; j < w_drw_lim; j++){
            printw("%d", char_map[i][j]);
        }
        //printw("\n");
    }
}