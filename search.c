#include "search.h"

void searchfunc(char *find, int key)
{  
	int prev_find = -1;
	int direction = 1;
	if (key == '\r' || key == '\x1b') 
	{
		prev_find = -1;
		direction = 1;
		return;
	}
	else if (key == UP_ARROW) 
	{
		direction = -1;
	}
	else if (key == DOWN_ARROW) 
	{
		direction = 1;
	}
	else 
	{
		prev_find = -1;
		direction = 1;
	}
	if (prev_find == -1) 
	{
		direction = 1;
	}
	int cur_find = prev_find;
	for (int i = 0; i < editor.rowcount; i++) 
	{
		cur_find += direction;
		if (cur_find == -1)
		{
			cur_find = editor.rowcount - 1;
		}
		else if (cur_find == editor.rowcount)
		{
			cur_find = 0;
		}
		editor_row *currow = &editor.rows[cur_find];
		char *same = strstr(currow->tabrend, find);
		if (same) 
		{
			prev_find = cur_find;
			editor.y_coor = cur_find;
			editor.x_coor = rendercordfix(currow, same - currow->tabrend);
			editor.x_coor = same - currow->tabrend;
			editor.row_offset = editor.rowcount;
			break;
		}
	}
}


void search()
{  
	int cur_x = editor.x_coor;
	int cur_y = editor.y_coor;
	int cur_coloff = editor.column_offset;
	int cur_rowoff = editor.row_offset;
	char *find = userprompt("Searching: %s Press ESC/ENTER to cancel, ARROWS to Navigate", searchfunc);
	if (find) 
	{
		free(find);
	} 
	else 
	{
		editor.x_coor = cur_x;
		editor.y_coor = cur_y;
		editor.column_offset = cur_coloff;
		editor.row_offset = cur_rowoff;
	}
}