/*
This file contains all the information for inputs given to the editor
*/
#include "include.h"
#include "terminal.h"
#include "view.h"
#include "row.h"

// Prompt user to input a filename when saving a new file
char *userprompt(char *prompt);

// Allows us to shift the cursor position
void cursormovement_editor(int arrowkeys);

// This process any keypress that is given
void process_editor();