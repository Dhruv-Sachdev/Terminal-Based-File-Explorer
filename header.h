#include <unistd.h>
#include <iostream>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <termios.h>
#include <string>
#include <dirent.h>
#include <bits/stdc++.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <iomanip>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BYTE_TO_KB(x) ((x) / 1024.0)
#define DASH "-"
#define NUM_LS_FIELDS 6
#define READ_PERMISSION "r"
#define WRITE_PERMISSION "w"
#define X_PERMISSION "x"
#define ESCAPE 27
#define UP_ARROW_KEY 65
#define DOWN_ARROW_KEY 66
#define RIGHT_ARROW_KEY 67
#define LEFT_ARROW_KEY 68
#define BACKSPACE 127
#define K 107
#define L 108
#define H 104
#define ENTER 10
#define COLON 58

// #define clrscr() cout<<"\033[H\033[J";//clear screen
// #define poscur() cout<<"\033[5;1H";//positions cursor to top-left

using namespace std;

#ifndef HEADER
#define HEADER
#endif

struct termios actual_term, n_canonical_term;
vector<vector<string>> directory_content_all;
vector<int> max_width(NUM_LS_FIELDS);
stack<string> left_stack;
stack<string> right_stack;
char path_for_ls[1000];
char home_path[1000];
bool more_entries = false;
int current_start_idx = 0;
int current_end_idx = 0;
int total_entries = 0;
// char *path_for_ls;
int cursor_x = 0;
int cursor_y = 0;
int num_entries = 0;
int window_rows = 0;
int window_cols = 0;
struct winsize window_size;


void raise_error(const char *s) {
    perror(s);
    exit(1);
}

void move_cursor(int x, int y) {
    printf("\033[%d;%dH", x, y);
    cursor_x = x;
    cursor_y = y;
}

int updateWindowSize() {
    struct winsize window_size;
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &window_size) == -1 || window_size.ws_col == 0) {
        return -1;
    }
    window_rows = window_size.ws_row;
    window_cols = window_size.ws_col;
    return 0;
}