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
		// Intro
		if (row_count == editor.row / 3)
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
		cons_dynamic(&db, "\x1b[K", 4);
		if (row_count < editor.row - 1) 
		{
			cons_dynamic(STDOUT_FILENO, row_print, 2);
		}
	}
}  


void screen_refresh()
{  
	struct dynamicbuff db = DYNAMIC;
	// Set Mode
	cons_dynamic(&db, "\x1b[?25l", 6);
	cons_dynamic(&db, esc_sequence_cursor, 3);
	row_chars(&db);
	cons_dynamic(&db, esc_sequence_cursor, 3);
	// Reset Mode
	cons_dynamic(&db, "\x1b[?25h", 6);
	write(STDOUT_FILENO, db.buff, db.leng);
	des_dynamic(&db);
}