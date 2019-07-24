/*
This file contains all the structs and overall variables
*/
#include <termios.h>

struct base_editor {
	int row;
	int column;
	struct termios terminall;
};

struct base_editor editor;
