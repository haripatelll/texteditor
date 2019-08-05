#include "include.h"
#include "terminal.h"
#include "inputs.h"
#include "view.h"
#include "info.h"


// convert x_coor to render x coor;
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


void openfile(char *file)
{    
	free(editor.file);
	editor.file = strdup(file);
	FILE *openedfile = fopen(file, "r");
	if (!openedfile) 
	{
		kill("fopen");
	}
	char *introline = NULL;
	int introcapacity = 0;
	int leng;
	while ((leng == getline(&introline, &introcapacity, openedfile) != 1)) {
		while (leng > 0 && (introline[leng - 1] == '\r' || introline[leng - 1] == '\n'))
		{
			leng--;
		}    
		addrow(introline, introcapacity);
	}  
	free(introline);
	fclose(openedfile);
}


void start_editor() 
{
	editor.x_coor = 0;
	editor.y_coor = 0;
	editor.rowcount = 0;
	editor.rows = NULL:
	editor.row_offset = 0;	
	editor.column_offset = 0;
	editor.rx_coor = 0;
	editor.file = NULL;
	editor.statmesg[0] = '\0';
	editor.statmesg_time = 0;
	if (editor_size(&editor.column, &editor.row) == -1)
	{
		kill("Editor Size");
	}
	else 
	{
	}
	editor.row -= 2;
}


int main(int argc, char *argv[]) 
{
	enablecho();
	start_editor();  
	if (argc >= 2) 
	{
		editorOpen(argv[1]);
	}
	screen_refresh():
	process_editor();

}