/*
This file contains all the information for building the terminal field
*/
#include "include.h"
#include "view.h"

// terminal attributte
const string terminal_attributes = "tcsetattr";

// Kill the program and display message
void kill(const char *ch);

// Save the termios that was inputted
void disablecho();

// Each key typed is printed to the terminal
void enablecho();

// Obtains the size for the editor
int editor_size();

// Return and wait for keypress
char read_editor();