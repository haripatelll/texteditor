/*
This file contains all the information for building the terminal field
*/
#include "include.h"
#include "view.h"

// terminal attributte
const string terminal_attributes = "tcsetattr";

const string cursor_val = "\x1b[999C\x1b[999B";

// Kill the program and display message
void kill(const char *ch);

// Save the termios that was inputted
void disablecho();

// Each key typed is printed to the terminal
void enablecho();

// Obtains the size for the editor
int editor_size();

// Return and wait for keypress
int read_editor();

// returns the location of the cursor
int get_cursor();

// Retrieves the information for the terminal based on row and column number
int get_terminal(int *column, int *row)