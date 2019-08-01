#include "inputs.h"


void cursormovement_editor(int keys)
{
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
		break;
		case DOWN_ARROW:
		boundcheck = editor.row - 1
		if (editor.y_coor != boundcheck)
		{
			editor.y_coor++;			
		}
		break;
		case RIGHT_ARROW:
		boundcheck = editor.column - 1
		if (editor.x_coor != boundcheck)
		{
			editor.x_coor++;
		}
		break;
	}
}

void process_editor() 
{  
	int ch = read_editor();
	int boundcheck;
	switch (ch) {
		case CTRL_KEY('q'):
		esc_clear();
		esc_cursor();
		exit(0);
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
			int amunt = editor.row;
			while (amunt--)
				cursormovement_editor(ch == UP_PAGE ? UP_ARROW : DOWN_ARROW);
		}
		break;
		case HOME:
		editor.x_coor = 0;
		break;
		case END:
		boundcheck = editor.column - 1;
		editor.x_coor = boundcheck;
		break;
	}
}

