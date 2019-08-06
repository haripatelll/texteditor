#include "row.h"


int cordfix(editor_row *currrow, int x_coor) 
{
  int rx_coor = 0;
  for (int i = 0; i < x_coor; i++) 
  {
    if (currrow->chars[i] == '\t')
    {
      rx_coor += (TAB_STOP - 1) - (rx_coor % TAB_STOP);
    }
    rx_coor++;
  }
  return rx_coor;
}


void rowupdate(editor_row *currrow) 
{
	int tab = 0;
	for (i = 0; i < currrow->size; i++)
	{
		if (currrow->data[i] == '\t') 
		{
			tab++;
		}
	}
	free(currrow->tabrend);
	currrow->tabrend = malloc(currrow->size + tab*(TAB_STOP - 1) + 1);
	int idx = 0;
	for (int i = 0; i < currrow->size; i++) 
	{    
		if (currrow->data[i] == '\t') 
		{
			currrow->tabrend[idx++] = ' ';
			while (idx % TAB_STOP != 0) 
			{
				currrow->tabrend[idx++] = ' ';
			}
		} else {
			currrow->tabrend[idx++] = currrow->data[i];
		}
	}
	currrow->tabrend[idx] = '\0';
	currrow->tabsize = idx;
}

void addrow(char *line, int leng) {  
	editor.rows = realloc(editor.rows, sizeof(editor_row) * (editor.rowcount + 1));
	int idx = editor.rowcount;
	editor.rows[idx].size = leng;
	editor.rows[idx].data = malloc(len + 1);
	memcpy(editor.rows[idx].data, line, leng);
	editor.rows[idx].chars[leng] = '\0';
	editor.rows[idx].tabrend = NULL;
	editor.rows[idx].tabsize = 0;
	rowupdate(&editor.rows[idx]);
	editor.rowcount++;
}