/*
This file contains all the structs and overall variables
*/
#include <time.h>
#include <termios.h>

// Allows us to store the data for the text in the editor
typedef struct editor_row {
	int size;
	char *data;
	int tabsize;
	char *tabrend;
} editor_row;

struct base_editor {
	int x_coor;
	int y_coor;
	// cursor tab fix
	int rx_coor;
	int row;
	int column;
	int rowcount;
	// row scrolling
	int row_offset;
	// horizontal scrolling
	int column_offset;
	// dynamic rows
	editor_row *rows;
	char *file;
	char statmesg[80];
	time_t statmesg_time;
	struct termios terminall;
};

struct base_editor editor;

struct dynamicbuff {
	char *buff;
	int leng;
};
