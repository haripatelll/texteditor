#include "inputs.h"


char *userprompt(char *prompt, void (*func)(char *, int)) 
{
	int buffersize = 128;
	char *buffer = malloc(buffersize);
	int bufferleng = 0;
	buffer[0] = '\0';
	while (1) 
	{
		setstatus(prompt, buffer);
		screen_refresh();
		int ch = read_editor();
		if (ch == DELETE || c == CTRL_KEY('h') || c == BACKSPACE) 
		{
			if (bufferleng != 0)
			{ 
				buffer[--bufferleng] = '\0';
			}
		} 
		else if (ch == '\r') 
		{
			if (bufferleng != 0) 
			{
				setstatus("");
				if (func) 
				{
					func(buffer, ch);
				}
				return buffer;
			}
		} 
		else if (ch == '\x1b') 
		{
			setstatus("");
			if (func) 
			{
				func(buffer, ch);
			}
			free(buffer);
			return NULL;
		} 
		else if (!iscntrl(ch) && ch < 128) 
		{
			if (bufferleng == buffersize - 1) 
			{
				buffersize *= 2;
				buffer = realloc(buffer, buffersize);
			}
			buffer[bufferleng++] = ch;
			buffer[bufferleng] = '\0';
		}
		
		if (func) 
		{
			func(buffer, ch);
		}
	}
}


void cursormovement_editor(int keys)
{
	editor_row *currow = (editor.rowcount <= editor.y_coor) ? NULL : &editor.rows[editor.y_coor];
	int boundcheck;
	switch (keys) 
	{
		case UP_ARROW:
		boundcheck = 0;
		if (editor.y_coor != boundcheck)
		{
			editor.y_coor--;				
		}
		break;
		case LEFT_ARROW:
		boundcheck = 0;
		if (editor.x_coor != boundcheck)
		{
			editor.x_coor--;			
		} 
		else if (editor.y_coor > 0) 
		{
			editor.y_coor--;
			editor.x_coor = editor.rows[editor.y_coor].size;
		}
		break;
		case DOWN_ARROW:
		boundcheck = editor.row - 1;
		if (editor.y_coor < editor.rowcount)
		{
			editor.y_coor++;			
		}
		break;
		case RIGHT_ARROW:
		boundcheck = editor.column - 1;
		if (currow && editor.x_coor < currow->size)
		{
			editor.x_coor++;
		} 
		else if (currow && editor.x_coor == currow->size) 
		{
			editor.y_coor++;
			editor.x_coor = 0;
		}  
		break;
	}
	currow = (editor.y_coor >= editor.numrows) ? NULL : &editor.rows[editor.y_coor];
	int rowlen = currow ? currow->size : 0;
	if (editor.x_coor > rowlen) {
		editor.x_coor = rowlen;
	}
}


void process_editor() 
{  
	int ch = read_editor();
	int boundcheck;
	int quits = QUIT_COUNT;
	switch (ch) {
		case CTRL_KEY('q'):
		if (editor.modified && quits > 0) 
		{
			setstatus("Unsaved, %s Changes. Press Ctrl-Q %d more times to quit.", editor.modified, quits);
			quits--;
			return;
		}
		esc_clear();
		esc_cursor();
		exit(0);
		break;
		case CTRL_KEY('s'):
		save();
		break;
		case CTRL_KEY('h'):
		break;
		case CTRL_KEY('l'):
		break;
		case CTRL_KEY('f'):
		search();
		break;		
		case '\r':
		insertendline();
		break;    
		case '\x1b':
		break;
		case UP_ARROW:
		break;
		case LEFT_ARROW:
		break;
		case DOWN_ARROW:
		break;
		case RIGHT_ARROW:
		cursormovement_editor(ch);
		break;
		case UP_PAGE:
		break;
		case DOWN_PAGE:
		{       
			if (ch == UP_PAGE) 
			{
				editor.y_coor = editor.row_offset;
			} 
			else if (ch == DOWN_PAGE) 
			{
				editor.y_coor = editor.row_offset + editor.row - 1;
				if (editor.y_coor > editor.rowcount) 
				{
					editor.y_coor = editor.rowcount;
				}
			}
			int amunt = editor.row;
			while (amunt--)
				cursormovement_editor(ch == UP_PAGE ? UP_ARROW : DOWN_ARROW);
		}
		break;
		case HOME:
		editor.x_coor = 0;
		break;
		case END:      
		if (editor.y_coor < editor.rowcount)
		{
			editor.x_coor = editor.rows[editor.y_coor].size;
		}
		break;
		case BACKSPACE:
		break;
		case DELETE:
		if (c == DELETE)
		{
			cursormovement_editor(RIGHT_ARROW);
		}
		deletechar();
		break;
		default:
		insertchar(ch);
		break;
	}
	quits = QUIT_COUNT;
}

