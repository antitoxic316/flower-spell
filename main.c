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

    char **bg_ascii_map;
    size_t bg_w, bg_h;

    char** frame_ascii_map;
    size_t frame_w, frame_h;

    Image *frame_list;
    Image *frame;
    int frame_i = 0;

    struct reactive_keys_args key_args;    
    
    bg_ascii_map = ascii_map_init(COLS, LINES);
    bg_w = COLS;
    bg_h = LINES;

    frame_list = img_generate_sprite_frames("flower1.png");
    frame = GetImageFromList(frame_list, frame_i);
    if(frame == NULL){
        printf("failed to parse frame1\n");
        return -1;
    }

    img_transform_to_ascii(frame, &frame_ascii_map, &frame_w, &frame_h);
    ascii_map_draw_on(
        bg_ascii_map, bg_w, bg_h,
        frame_ascii_map, frame_w, frame_h,
        bg_w/2, bg_h/2
    );

    ascii_map_draw_on(
        bg_ascii_map, bg_w, bg_h,
        frame_ascii_map, frame_w, frame_h,
        0, bg_h-frame_h
    );

    key_args.q_key_pressed = false;
    key_args.w_key_pressed = false;

	while(true){
        ascii_map_print(bg_ascii_map, bg_w, bg_h);

        refresh();

        pthread_t thread_io;
        pthread_create(&thread_io, NULL, &user_input_reader, (void *)&key_args);
        pthread_join(thread_io, NULL);
        if (key_args.q_key_pressed){
            break; 
        }
        if(key_args.w_key_pressed){
            frame_i++;
            frame = GetImageFromList(frame_list, frame_i);
            if(!frame){
                frame_i = 0;
                continue;
            }
            
            // FULLY FREE CHARMAP HERE
            img_transform_to_ascii(frame, &frame_ascii_map, &frame_w, &frame_h);
            ascii_map_draw_on(
                bg_ascii_map, bg_w, bg_h,
                frame_ascii_map, frame_w, frame_h,
                bg_w/2, bg_h/2
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