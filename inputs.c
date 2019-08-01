#include "inputs.h"


void cursormovement_editor(char arrowkeys)
{
	switch (arrowkeys) 
	{
		case 'w':
		editor.y_coor--;
		break;
		case 'a':
		editor.x_coor--;
		break;
		case 's':
		editor.y_coor++;
		break;
		case 'd':
		editor.x_coor++;
		break;
	}
}

void process_editor() 
{  
	char ch = read_editor();
	switch (ch) {
		case CTRL_KEY('q'):
		esc_clear();
		esc_cursor();
		exit(0);
		break;

		case 'w':
		case 'a':
		case 's':
		case 'd':
		cursormovement_editor(ch);
		break;
	}
}

