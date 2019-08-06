#include "include.h"
#include "terminal.h"
#include "inputs.h"
#include "view.h"
#include "info.h"


char *rowstring(int * bufferleng) {
	int maxleng = 0;
	for (int i = 0; i < editor.rowcount; i++)
	{
		maxleng += editor.rows[i].size + 1;
	}
	* bufferleng = maxleng;
	char *buffer = malloc(maxleng);
	char *point = buffer;
	for (int i = 0; i < editor.rowcount; i++) 
	{
		memcpy(p, editor.rows[i].data, editor.rows[i].size);
		point += editor.rows[i].size;
		*point = '\n';
		point++;
	}
	return buffer;
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
		addrow(introline, introcapacity, editor.rowcount);
	}  
	free(introline);
	fclose(openedfile);
	editor.modifed = 0;
}


void save() 
{
	if (editor.file == NULL) 
	{    
		editor.file = userprompt("Save as: %s, Press ESC to cancel");
		if (editor.filename == NULL) 
		{
			setstatus("Save Stopped");
			return;
		}
	}
	int leng;
	char *buffer = rowstring(&leng);
	// Read and write open files
	int curfile = open(editor.file, O_RDWR | O_CREAT, 0644);
	if (curfile != -1) 
	{
		if (ftruncate(curfile, leng) != -1) 
		{
			if (write(curfile, buffer, leng) == leng) 
			{
				close(curfile);
				free(buffer);
				editor.modifed = 0;
				setstatus("%d data saved", leng);
				return;
			}
		}
		close(curfile);
	}
	free(buffer);
	setstatus("Save Error: %s", strerror(errno));
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
	editor.modifed = 0;
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
	setstatus("HELP: Ctrl-S = save & Ctrl-Q = quit");
	screen_refresh():
	process_editor();

}