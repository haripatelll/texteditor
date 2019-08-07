#include "veiw.h"

void esc_clear()
{
	// Write 4 bytes to the terminal view
	write(STDOUT_FILENO, ESC_CLEAR, 4);	
}


void esc_cursor()
{
	// 3 bytes for the cursor psotioning
	write(STDOUT_FILENO, ESC_CURSOR, 3);
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
					cons_dynamic(db, STARTLINE, 1);	
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
				cons_dynamic(db, STARTLINE, 1);			
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
		cons_dynamic(&db, "\x1b[K", 3);
		cons_dynamic(STDOUT_FILENO, ROW_PRINT, 2);
	}
}  


void statusbar(struct dynamicbuff *db) 
{
	cons_dynamic(db, "\x1b[7m", 4);
	char status[80];
	char renderstatus[80];
	int leng = snprintf(status, sizeof(status), "%.20s - %d %s",
		editor.file ? editor.file : "[No Name]", editor.rowcount,
		editor.modified ? "(modified)" : "");
	int renderleng = snprintf(renderstatus, sizeof(renderstatus), "%d/%d",
		editor.y_coor + 1, editor.rowcount);
	if (leng > editor.column) 
	{
		leng = editor.column;
	}
	cons_dynamic(db, status, leng);
	while (leng < editor.column) 
	{
		if (editor.column - leng == renderleng) {
			cons_dynamic(db, renderstatus, renderleng);
			break;
		} else {
			leng++;
			cons_dynamic(db, " ", 1);
		}
	}
	cons_dynamic(db, "\x1b[m", 3);
	cons_dynamic(db, "\r\n", 2);
}


void setstatus(const char *mesg, ...) 
{
	va_list argpassed;
	va_start(argpassed, mesg);
	vsnprintf(editor.statmesg, sizeof(editor.statmesg), mesg, argpassed);
	va_end(argpassed);
	editor.statmesg_time = time(NULL);
}


void mesgbar(struct dynamicbuff *db) 
{
	cons_dynamic(db, "\x1b[K", 3);
	int leng = strlen(editor.statmesg);
	if (leng > editor.column) 
	{
		leng = editor.column;
	}
	if (leng && time(NULL) - editor.statmesg_time < 5)
	{
		cons_dynamic(db, editor.statmesg, leng);
	}
}

void scroll() 
{
	editor.rx_coor = 0;
	if (editor.y_coor < editor.rowcount) 
	{
		editor.x_coor = cordfix(&editor.rows[editor.y_coor],
			editor.x_coor);
	}
	if (editor.y_coor >= editor.row_offset + editor.row) 
	{
		editor.row_offset = editor.y_coor - editor.row + 1;
	}
	if (editor.y_coor < editor.row_offset) 
	{
		editor.row_offset = editor.y_coor;
	}
	if (editor.rx_coor < editor.column_offset) 
	{
		editor.column_offset = editor.rx_coor;
	}
	if (editor.rx_coor >= editor.column_offset + editor.column) 
	{
		editor.column_offset = editor.rx_coor - editor.column + 1;
	}
}


void screen_refresh()
{  
	scroll();
	struct dynamicbuff db = DYNAMIC;
	// Set Mode
	cons_dynamic(&db, "\x1b[?25l", 6);
	cons_dynamic(&db, ESC_CURSOR, 3);
	row_chars(&db);
	statusbar(&db);
	mesgbar(&db);
	char buffer[32];
	// Position fix
	snprintf(buffer, sizeof(buffer), "\x1b[%d;%dH", (editor.y_coor - editor.row_offset) + 1, 
		(editor.rx_coor - editor.column_offset) + 1);
	cons_dynamic(&db, buffer, strlen(buffer));
	cons_dynamic(&db, ESC_CURSOR, 3);
	// Reset Mode
	cons_dynamic(&db, "\x1b[?25h", 6);
	write(STDOUT_FILENO, db.buff, db.leng);
	des_dynamic(&db);
}