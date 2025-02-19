#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <ncurses.h>


struct reactive_keys_args {
	bool q_key_pressed;
};


void* user_input_reader(void *argv);
int** init_bit_map(void);
void test_show_bitmap(int**);


int main(int argc, char *argv[]){
    initscr();    

    int** bit_map = init_bit_map();

    struct reactive_keys_args key_args;    
    key_args.q_key_pressed = false;

	while(true){
        test_show_bitmap(bit_map);

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


int** init_bit_map(void){
    int **bit_map = malloc(LINES * sizeof(int*));
    
    for(int i = 0; i < LINES; i++){
        int *initiated_line = calloc(COLS, sizeof(int));
        bit_map[i] = initiated_line;

    }

    return bit_map;
}

void test_show_bitmap(int** bit_map){
    for(int i = 0; i < LINES; i++){
        for(int j = 0; j < COLS; j++){
            printw("%d", bit_map[i][j]);
        }
        //printw("\n");
    }
}