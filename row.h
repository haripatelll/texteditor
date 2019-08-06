#include "include.h"

// convert x_coor to render x coor;
int cordfix(editor_row *currrow, int x_coor);

// updates the rows insdie the editor
void rowupdate(editor_row *currrow);

// adds a row by taking in a line of characters
void addrow(char *line, int leng);