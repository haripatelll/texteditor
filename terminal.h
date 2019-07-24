/*
This file contains all the information for building the terminal field
*/
#include "include.h"
#include "view.h"

// terminal attributte
const string terminal_attributes = "tcsetattr";
struct termios TERMINAL;

// Kill the program and display message
void kill(const char *ch);

// Save the termios that was inputted
void disablecho();

//Each key typed is printed to the terminal
void enablecho();

// Return and wait for keypress
char read_editor();