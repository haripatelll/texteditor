/*
This file contains all the information for inputs given to the editor
*/
#include "include.h"
#include "terminal.h"
#include "view.h"
#include "row.h"
#include "search.h"

// Prompt user to input a filename when saving a new file
// conatins callback function
char *userprompt(char *prompt, void (*func)(char *, int));

// Allows us to shift the cursor position
void cursormovement_editor(int arrowkeys);

// This process any keypress that is given
void process_editor();