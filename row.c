#include "currow.h"


int cordfix(editor_row *currrow, int x_coor) 
{
	int rx_coor = 0;
	for (int i = 0; i < x_coor; i++) 
	{
		if (currrow->data[i] == '\t')
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


void addrow(char *line, int len, int idx) 
{ 
	if (idx > editor.rowcount || idx < 0)
	{
		return;
	}   
	editor.rows = realloc(editor.rows, sizeof(editor_row) * (editor.rowcount + 1));
	memmove(&editor.rows[idx + 1], &editor.rows[idx], sizeof(editor_row) * (editor.rowcount - idx));
	editor.rows[idx].size = leng;
	editor.rows[idx].data = malloc(len + 1);
	memcpy(editor.rows[idx].data, line, leng);
	editor.rows[idx].data[leng] = '\0';
	editor.rows[idx].tabrend = NULL;
	editor.rows[idx].tabsize = 0;
	rowupdate(&editor.rows[idx]);
	editor.rowcount++;
	editor.modified++;
}


void freerow(editor_row *currow) 
{
	free(currow->tabrend);
	free(currow->data);
}


void deleterow(int idx) 
{
	if (idx < 0 || idx >= editor.rowcount) 
	{
		return;
	}
	freerow(&editor.rows[idx]);
	memmove(&editor.rows[idx], &editor.rows[idx + 1], sizeof(editor_row) * (editor.rowcount - idx - 1));
	editor.rowcount--;
	editor.modified++;
}


void rowinsertchar(editor_row *currow, int idx, int ch) 
{
	if (idx < 0 || idx > currow->size) 
	{
		idx = currow->size;
	}
	currow->data = realloc(currow->data, currow->size + 2);
	memmove(&currow->data[idx + 1], &currow->data[idx], currow->size - idx + 1);
	currow->size++;
	currow->data[idx] = ch;
	rowupdate(currow);
	editor.modified++;
}


void rowappendstring(editor_row *currow, char *str, int leng)
{
	currow->data = realloc(currow->data, currow->size + leng + 1);
	memcpy(&currow->data[currow->size], str, leng);
	currow->size += leng;
	currow->data[currow->size] = '\0';
	rowupdate(currow);
	editor.modified++;
}


void rowdeletechar(editor_row *currow, int idx)
{  
	if (idx >= currow->size || idx < 0) 
	{
		return;
	}
	memmove(&currow->data[idx], &currow->data[idx + 1], currow->size - idx);
	currow->size--;
	rowupdate(currow);
	editor.modified++;
}


// SECTION BELOW changes the editor
void insertchar(int ch) 
{
	if (editor.y_coor == editor.rowcount) 
	{
		addrow("", 0, editor.rowcount);
	}
	rowinsertchar(&editor.rows[editor.y_coor], editor.x_coor, ch);
	editor.x_coor++;
}


void deletechar() 
{
	if (editor.y_coor == editor.rowcount) 
	{
		return;
	}
	if (editor.x_coor == 0 && editor.y_coor == 0)
	{
		return;
	}
	editor_row *currow = &editor.rows[editor.y_coor];
	if (editor.x_coor > 0) {
		rowdeletechar(currow, editor.x_coor - 1);
		editor.x_coor--;
	}
	else
	{    
		editor.x_coor = editor.rows[editor.y_coor - 1].size;
		rowappendstring(&editor.rows[editor.y_coor - 1], currow->data, currow->size);
		deleterow(editor.y_coor);
		editor.y_coor--;
	}
}


void insertendline()
{
  if (editor.x_coor == 0) 
  {
    addrow( "", 0, editor.y_coor);
  } else 
  {
    editor_row *currow = &editor.rows[editor.y_coor];
    addrow(&currow->data[editor.x_coor], currow->size - editor.x_coor, editor.y_coor + 1);
    currow = &editor.rows[editor.y_coor];
    currow->size = editor.x_coor;
    currow->data[currow->size] = '\0';
    rowupdate(currow);
  }
  editor.x_coor = 0;
  editor.y_coor++;
}