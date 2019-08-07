/*
This file contains all the information for incremental search in the text editor
*/
#include "include.h"
#include "row.h"

// callback function for userprompt
// does the actual searching
// Arrow up goes to prev find
// Arrow down goes to next find
void searchfunc(char *find, int key);

// search through the text editor
void search();