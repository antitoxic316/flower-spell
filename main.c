#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#include "ascii.h"
#include "image.h"

struct reactive_keys_args {
	bool q_key_pressed;
    bool w_key_pressed;
};


void* user_input_reader(void *argv);

int main(int argc, char *argv[]){    
    initscr();

    struct ascii_canvas *canv;

    struct char_img *ch_i, *ch_i2;

    struct reactive_keys_args key_args;    
    
    canv = ascii_canvas_new(COLS, LINES);

    ch_i = char_img_new_from_file("flower1.png");
    ch_i2 = char_img_new_from_file("flower1.png");

    key_args.q_key_pressed = false;
    key_args.w_key_pressed = false;

	while(true){
        ascii_canvas_print(canv);

        refresh();

        pthread_t thread_io;
        pthread_create(&thread_io, NULL, &user_input_reader, (void *)&key_args);
        pthread_join(thread_io, NULL);
        if (key_args.q_key_pressed){
            break; 
        }
        if(key_args.w_key_pressed){
            char_img_next_frame(ch_i);
            ascii_canvas_draw_on(
                canv, ch_i, canv->bg_image->w/2, canv->bg_image->h/2
            );
            char_img_next_frame(ch_i2);
            ascii_canvas_draw_on(
                canv, ch_i2, canv->bg_image->w/8, canv->bg_image->h-28
            );
            clear();
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
    }
    
    pthread_exit(NULL);
}