/*
This file contains all the structs and overall variables
*/
#include <termios.h>

struct base_editor {
	int x_coor;
	int y_coor;
	int row;
	int column;
	struct termios terminall;
};

struct base_editor editor;

struct dynamicbuff {
  char *buff;
  int leng;
};
