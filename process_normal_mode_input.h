#ifndef HEADER
#define HEADER
#include "header.h"
#endif

#ifndef LIST_DIR
#define LIST_DIR
#include "list_directory.h"
#endif

void erase_screen() {
    printf("\033[1J");
    move_cursor(1, 1);
}

void update_path_for_ls(string s) {
    strcpy(path_for_ls, s.c_str());
    move_cursor(num_entries + 5, 0);
    // printf("path for ls %s\n", path_for_ls);
    num_entries++;
}

void move_cursor_up() {
    if(cursor_x > 1) {
        move_cursor(cursor_x - 1, cursor_y);  
    }
}

void move_cursor_down() {
    if(cursor_x < num_entries) {
        move_cursor(cursor_x + 1, cursor_y);
    }
}


void push_to_stack(stack<string> &st, string s) {
    st.push(s);
}

void clear_stack(stack<string> &st) {
    while(!st.empty())
        st.pop();
}

void goto_prev_dir() {
    if(left_stack.empty()) {
        return;
    }
    push_to_stack(right_stack, path_for_ls);
    update_path_for_ls(left_stack.top());
    left_stack.pop();
    
    erase_screen();
    list_directory_content();
}

/*
This method should be called when 
we change to a different directory 
in cmd mode or we press enter in 
normal mode to move to a sub directory
*/
void goto_forward_dir() {
    if(right_stack.empty())
        return;

    push_to_stack(left_stack, path_for_ls);
    update_path_for_ls(right_stack.top());
    right_stack.pop();

    erase_screen();
    list_directory_content();

}



void goto_dir(string new_path, bool normal_mode = true) {
    move_cursor(num_entries + 1, 0);
    
    push_to_stack(left_stack, path_for_ls);
    update_path_for_ls(new_path);
    if(normal_mode) {
        erase_screen();
        list_directory_content();
    }
}

void goto_parent() {
    string destination_path = path_for_ls;
    destination_path += "/..";
    goto_dir(destination_path);
}

void goto_home() {
    string destination_path = home_path;
    goto_dir(destination_path);
}

void open_file_in_vi(char *file_path) {
    pid_t p = fork();
    if(p == 0) {
        //child process
        execl("/usr/bin/vim", "vi", file_path, NULL);
        exit(0);
    }
    wait(NULL);
}
