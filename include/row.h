/*
This file contains all the row functions
*/
#include "include.h"

// convert x_coor to render x_coor
int cordfix(editor_row *currrow, int x_coor);

// conver rx_coor to x_coor
int rendercordfix(editor_row *currrow, int rx_coor);

// updates the rows insdie the editor
void rowupdate(editor_row *currrow);

// adds a row by taking in a line of characters
void addrow(char *line, int len, int idx);

// Frees the current row 
void freerow(editor_row *row);

// Deletes the row, calls freerow to free up the momory
void deleterow(int idx);

// Inserts a character into an editor row character one at a time
void rowinsertchar(editor_row *currow, int idx, int ch);

// appends a string to the end of a row.
void rowappendstring(editor_row *currow, char *str, int leng);

// Inserts a character into an editor row character one at a time
void rowdeletechar(editor_row *currow, int idx);

// Inserts one character
void insertchar(int ch);

// Deletes one character
void deletechar();

// This will handle the end key
void insertendline();