#ifndef HEADER
#define HEADER
#include "header.cpp"
#endif


void endNCanonicalMode() {

    // if(tcsetattr(STDIN_FILENO, TCSANOW, &actual_term) < 0) {
    //     raise_error("Could not set back to actual termios settings\n");
    // }
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &actual_term) < 0) {
		printf("Could not set attributes\n");
		exit(0);
	}
}

void startNCanonicalMode() {
    tcgetattr(STDIN_FILENO, &actual_term);
    
    atexit(endNCanonicalMode);
    n_canonical_term = actual_term;
    n_canonical_term.c_lflag &= ~(ECHO | ICANON);
    // n_canonical_term.c_cc[VMIN] = 0;
    // n_canonical_term.c_cc[VTIME] = 1;
    // n_canonical_term.c_iflag &= ~(IXON); // This is not needed for our requirements really
    /* What TCSAFLUSH does is that it waits for all 
    output to be written to the terminal first and 
    it discard input that has not been read
    */
    if(tcsetattr(STDIN_FILENO, TCSANOW, &n_canonical_term) < 0) {
        raise_error("Could not change termios settings\n");
    }
}