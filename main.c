#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#include "ascii.h"
#include "image.h"

struct reactive_keys_args {
	bool q_key_pressed;
    bool w_key_pressed;
    bool n_key_pressed;
};


void* user_input_reader(void *argv);
void draw_random_image(struct ascii_canvas *canv){
    struct char_gif *ch_i;
    
    ch_i = char_img_new_from_file("flower1.png");

    int x = rand() % ((COLS - ch_i->w/2) + 1);
    int y = rand() % ((LINES - ch_i->h/2) + 1 - horizon_line) + horizon_line; 

    ascii_canvas_add_img(canv, ch_i);
    char_gif_move(canv, ch_i, x, y); 
    ascii_canvas_draw_on(canv, ch_i);
}


int main(int argc, char *argv[]){    
    initscr();

    struct ascii_canvas *canv;

    struct reactive_keys_args key_args;    
    
    canv = ascii_canvas_new(COLS, LINES);

	while(true){
        key_args.q_key_pressed = false;
        key_args.w_key_pressed = false;
        key_args.n_key_pressed = false;

        clear();
        ascii_canvas_print(canv);

        refresh();

        pthread_t thread_io;
        pthread_create(&thread_io, NULL, &user_input_reader, (void *)&key_args);
        pthread_join(thread_io, NULL);
        if (key_args.q_key_pressed){
            break; 
        }
        if(key_args.w_key_pressed){
            ascii_canvas_progress_scene(canv);
            continue;
        }
        if (key_args.n_key_pressed){
            draw_random_image(canv);
            continue;
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
    if (input_reading == 'q') {
        args->q_key_pressed = true;
    } else if( input_reading == 'w'){
        args->w_key_pressed = true;
    } else if( input_reading == 'n'){
        args->n_key_pressed = true;
    }
    
    pthread_exit(NULL);
}