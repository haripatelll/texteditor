#include "veiw.h"

void esc_clear()
{
	// Write 4 bytes to the terminal view
	write(STDOUT_FILENO, esc_sequence_clear, 4);	
}


void esc_cursor()
{
	// 3 bytes for the cursor psotioning
	write(STDOUT_FILENO, esc_sequence_cursor, 3);
}


void row_chars(struct dynamicbuff *db) 
{  
	int row_count;
	for (row_count = 0; row_count < editor.row; row_count++) 
	{
		int filerows =  editor.row_offset + row_count;
		if (filerows >= editor.rowcount) {
		// Intro
			if (editor.rowcount == 0 && y == editor.row / 3) 
			{
				char intro[80];
				int sizee = sizeof(intro);
				int introleng = snprintf(intro, sizee, "Tiny Texteditor");
				if (introleng > editor.column)
				{
					introleng = editor.column;
				}
				int pad = (editor.column - introleng) / 2;
				if (pad) 
				{
					cons_dynamic(db, startline, 1);	
					pad--;
				}
				while (pad--)
				{
					cons_dynamic(db, " ", 1);					
				}
				cons_dynamic(db, intro, introleng);	
			} 
			else 
			{
				cons_dynamic(db, startline, 1);			
			}
		} 
		else 
		{
			int leng = editor.rows[filerows].tabsize - editor.column_offset;
			if (leng < 0)
			{
				leng = 0;
			}
			if (leng > editor.column)
			{ 
				leng = editor.column;
			}
			addrow(db, &editor.rows[filerows].tabrender[editor.column_offset], leng);
		}
		cons_dynamic(&db, "\x1b[K", 4);
		if (row_count < editor.row - 1) 
		{
			cons_dynamic(STDOUT_FILENO, row_print, 2);
		}
	}
}  


void scroll() 
{
	if (editor.y_coor >= editor.row_offset + editor.row) 
	{
		editor.row_offset = editor.cy - editor.row + 1;
	}
	if (editor.y_coor < editor.row_offset) 
	{
		editor.row_offset = editor.cy;
	}
	if (editor.x_coor < editor.column_offset) 
	{
		editor.column_offset = editor.x_coor;
	}
	if (editor.x_coor >= editor.column_offset + editor.column) 
	{
		editor.column_offset = editor.x_coor - editor.column + 1;
	}
}


void screen_refresh()
{  
	scroll();
	struct dynamicbuff db = DYNAMIC;
	// Set Mode
	cons_dynamic(&db, "\x1b[?25l", 6);
	cons_dynamic(&db, esc_sequence_cursor, 3);
	row_chars(&db);
	char buffer[32];
	// Position fix
	snprintf(buffer, sizeof(buffer), "\x1b[%d;%dH", (editor.y_coor - editor.row_offset) + 1, 
		(editor.x_coor - editor.column_offset) + 1);
	cons_dynamic(&db, buffer, strlen(buffer));
	cons_dynamic(&db, esc_sequence_cursor, 3);
	// Reset Mode
	cons_dynamic(&db, "\x1b[?25h", 6);
	write(STDOUT_FILENO, db.buff, db.leng);
	des_dynamic(&db);
}