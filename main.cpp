#ifndef HEADER
#define HEADER
#include "header.h"
#endif

#ifndef LIST_DIR
#define LIST_DIR
#include "list_directory.h"
#endif

#include "command_mode.h"
#include "terminal_modes.h"


#ifndef PROCESS_NORMAL_MODE
#define PROCESS_NORMAL_MODE
#include "process_normal_mode_input.h"
#endif


void clear_screen() {
    /*
    This is a 4 byte escape character sequence.
    \x1b is an escape character (27) 
    */
    // write(STDOUT_FILENO, "\x1b[1J", 4);
    // printf("\033[2J\033[1;1H");

    printf("\033[H\033[J");
    // cout << "\033[H\033[J";
}

void processInputHelper(char c) {
    if(c == ESCAPE) {
        // read(STDIN_FILENO, &c, 1);
        // read(STDIN_FILENO, &c, 1);
        c=cin.get();
        c=cin.get();
        switch (c)
        {

        case UP_ARROW_KEY:
            move_cursor_up();
            break;
        
        case DOWN_ARROW_KEY:
            move_cursor_down();
            break;

        case LEFT_ARROW_KEY:
            // printf("Left Key Pressed\n");
            goto_prev_dir();
            break;

        case RIGHT_ARROW_KEY:
            goto_forward_dir();
            break;

        default:
            break;
        }
    }
    else if(c == BACKSPACE) {
        goto_parent();
        clear_stack(right_stack);
    }
    else if(c == K) {
        cout << "Scroll up\n";
    }
    else if(c == L) {
        cout << "Scroll down\n";
    }
    else if(c == H) {
        goto_home();
        clear_stack(right_stack);
    }
    else if(c == ENTER) {
        clear_stack(right_stack);
        string current_path = path_for_ls;
        string new_path = current_path;
        new_path += "/";
        new_path += directory_content_all[cursor_x - 1][NUM_LS_FIELDS - 1];
        char *file_path = &new_path[0];
        struct stat stat_file;
        if(stat(file_path, &stat_file) < 0) {
            raise_error("Could not find stats");
            exit(1);
        }
        if(S_ISREG(stat_file.st_mode)) {
            open_file_in_vi(file_path);
            //the file is a regular file and should be opened using vim
        }
        else if(S_ISDIR(stat_file.st_mode)) {
            goto_dir(new_path);
        }
        // cout << "Enter was pressed\n";
    }
    else if(c == COLON) {
        enter_command_mode();
        clear_screen();
        move_cursor(1, 1);
        list_directory_content();
    }
    
}


void processInput() {
    
    // char c = '\0';
    // if(read(STDIN_FILENO, &c, 1) == -1) {
    //     raise_error("Error in read");
    // }
    char c=cin.get();
    if(c == 'q') {
        //***
        clear_screen();
        exit(0);
    }
    processInputHelper(c);
    // if(c == 'q') {
    //     clrscr();
    //     exit(0);
    // }
}


int main() {
    // clrscr();
    clear_screen();
    startNCanonicalMode();
    getcwd(home_path, sizeof(home_path));
    strcpy(path_for_ls, home_path);
    list_directory_content();
    // atexit(clear_screen);
    atexit(endNCanonicalMode);
    while(1) {
        processInput();
    }

    return 0;
}