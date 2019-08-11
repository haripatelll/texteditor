//#include "include.h"
// This file contains the includes and defines that are needed to run the program
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdarg.h>
#include <fcntl.h>
#include <time.h>
#include <termios.h>


// Bitwise-ANDs a character with the value 00011111
#define CTRL_KEY(k) ((k) & 0x1f)
#define QUIT_COUNT  1;
const int TAB_STOP = 8;


// Designates the keys to characters
enum arrowkeys 
{
  BACKSPACE = 127,
  LEFT = 1000,
  RIGHT,
  UP,
  DOWN,
  DELETE,
  HOME,
  END,
  PAGE_UP,
  PAGE_DOWN
};


// Allows us to store the data for the text in the editor
typedef struct editor_row 
{
	int size;
	char *data;
	int tabsize;
	char *tabrend;
} editor_row;


struct base_editor 
{
	int x_coor;
	int y_coor;
	// cursor tab fix
	int rx_coor;
	int row;
	int column;
	int rowcount;
	// row scrolling
	int row_offset;
	// horizontal scrolling
	int column_offset;
	// Essentially a dirty bit to see 
	// if we modified the file
	int modified;
	// dynamic rows
	editor_row *rows;
	char *file;
	char statmesg[80];
	time_t statmesg_time;
	struct termios terminall;
};


struct base_editor editor;


#define DYNAMIC {NULL, 0}


struct dynamicbuff 
{
	char *buff;
	int leng;
};


char *userprompt(char *prompt, void (*func)(char *, int));


void setstatus(const char *mesg, ...);


void screen_refresh();


void esc_clear()
{
	// Write 4 bytes to the terminal view
	write(STDOUT_FILENO, "\x1b[2J", 4);	
}


void esc_cursor()
{
	// 3 bytes for the cursor psotioning
	write(STDOUT_FILENO, "\x1b[H", 3);
}


//#include "terminal.h"
void kill(const char *ch) 
{
  // When exiting the terminal Refresh and clear the cursor
	esc_clear();
	esc_cursor();
	perror(ch);
	exit(1);
}


void disablecho() 
{
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &editor.terminall) == -1)
	{
		kill("tcsetattr");
	}
}


void enablecho()
{
	if (tcgetattr(STDIN_FILENO, &editor.terminall) == -1) 
	{
		kill("tcsetattr");
	}
	atexit(disablecho);
	struct termios echoo = editor.terminall;
  // Read input in bytes not lines
  // Turn off ctrl-c/v/z
	echoo.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  // Turn off ctrl-s/q
	echoo.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  // Output processing is turned off
	echoo.c_oflag &= ~(OPOST);
  // Set chat to 8 per byte
	echoo.c_cflag |= (CS8);
  // Bottom two are for timeout
	echoo.c_cc[VMIN] = 0;
	echoo.c_cc[VTIME] = 1;
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &echoo) == -1) 
	{
		kill("tcsetattr");
	}
}


int read_editor() 
{
	int key;
	char ch;
	while ((key = read(STDIN_FILENO, &ch, 1)) != 1)
	{
		if (key == -1 && errno != EAGAIN) 
		{
			kill("read");
		}
	}  
	if (ch == '\x1b') 
	{
		char arrow[3];
		if (read(STDIN_FILENO, &arrow[0], 1) != 1) 
		{
			return '\x1b';
		}
		if (read(STDIN_FILENO, &arrow[1], 1) != 1) 
		{
			return '\x1b';
		}
		if (arrow[0] == '[') 
		{
			if (arrow[1] >= '0' && arrow[1] <= '9') 
			{
				if (read(STDIN_FILENO, &arrow[2], 1) != 1) 
				{
					return '\x1b';
				}
				if (arrow[2] == '#') 
				{
					switch (arrow[1]) 
					{           
						case '1': return HOME;
						case '3': return DELETE;
						case '4': return END;
						case '5': return PAGE_UP;
						case '6': return PAGE_DOWN;
						case '7': return HOME;
						case '8': return END;
					}
				}
			} 
			else 
			{
				switch (arrow[1]) 
				{
					case 'A': return UP;
					case 'B': return DOWN;
					case 'C': return RIGHT;
					case 'D': return LEFT;
					case 'H': return HOME;
					case 'F': return END;
				}
			}
		}
		else if (arrow[0] == '0')
		{
			switch (arrow[1])
			{
				case 'H': return HOME;
				case 'F': return END;        
			}
		}
		return '\x1b';
	} 
	else 
	{
		return ch;
	}
}


int get_cursor(int *row, int *column) 
{
	char buffer_zone[32];  
	unsigned int i = 0;
	if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) 
	{
		return -1;
	}
	for (i = 0; i < sizeof(buffer_zone) - 1; i++) 
	{
		if (buffer_zone[i] == 'R') 
		{
			break;
		}
		if (read(STDIN_FILENO, &buffer_zone[i], 1) != 1) 
		{
			break;
		}
	}
	buffer_zone[i] = '\0';
	if (buffer_zone[0] != '\x1b' || buffer_zone[1] != '[') 
	{
		return -1;
	}
	if (sscanf(&buffer_zone[2], "%d;%d", row, column) != 2)
	{
		return -1;
	}
	else 
	{
		return 0;
	}
}


int editor_size(int *row, int *column) 
{
	struct winsize lcl_terminal;
	if ( ioctl(STDOUT_FILENO, TIOCGWINSZ, &lcl_terminal) == -1 || lcl_terminal.ws_col == 0) 
	{
		if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) 
		{
			return -1;
		}
		return get_cursor(row, column);
	} 
	else 
	{
		*row = lcl_terminal.ws_row;
		*column = lcl_terminal.ws_col;
		return 0;
	}
}


int cordfix(editor_row *currow, int x_coor) 
{
	int rx_coor = 0;
	for (int i = 0; i < x_coor; i++) 
	{
		if (currow->data[i] == '\t')
		{
			rx_coor += (TAB_STOP - 1) - (rx_coor % TAB_STOP);
		}
		rx_coor++;
	}
	return rx_coor;
}



int rendercordfix(editor_row *currow, int rx_coor)
{	 
	int current_rx = 0;
	int x_coorr = 0;
	for (x_coorr = 0; x_coorr < currow->size; x_coorr++) 
	{
		if (currow->data[x_coorr] == '\t')
		{
			current_rx += (TAB_STOP - 1) - (current_rx % TAB_STOP);
		}
		current_rx++;
		if (current_rx > rx_coor)
		{
			return x_coorr;
		}

	}
	return x_coorr;
}


void rowupdate(editor_row *currow) 
{
	int tab = 0;
	for (int i = 0; i < currow->size; i++)
	{
		if (currow->data[i] == '\t') 
		{
			tab++;
		}
	}
	free(currow->tabrend);
	currow->tabrend = malloc(currow->size + tab*(TAB_STOP - 1) + 1);
	int idx = 0;
	for (int i = 0; i < currow->size; i++) 
	{    
		if (currow->data[i] == '\t') 
		{
			currow->tabrend[idx++] = ' ';
			while (idx % TAB_STOP != 0) 
			{
				currow->tabrend[idx++] = ' ';
			}
		} else {
			currow->tabrend[idx++] = currow->data[i];
		}
	}
	currow->tabrend[idx] = '\0';
	currow->tabsize = idx;
}


void addrow(int idx, char *line, size_t leng) 
{ 
	if (idx > editor.rowcount || idx < 0)
	{
		return;
	}   
	editor.rows = realloc(editor.rows, sizeof(editor_row) * (editor.rowcount + 1));
	memmove(&editor.rows[idx + 1], &editor.rows[idx], sizeof(editor_row) * (editor.rowcount - idx));
	editor.rows[idx].size = leng;
	editor.rows[idx].data = malloc(leng + 1);
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


void rowappendstring(editor_row *currow, char *str, size_t leng)
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


void insertchar(int ch) 
{
	if (editor.y_coor == editor.rowcount) 
	{
		addrow(editor.rowcount, "", 0);
	}
	rowinsertchar(&editor.rows[editor.y_coor], editor.x_coor, ch);
	editor.x_coor++;
}


void insertendline()
{
	if (editor.x_coor == 0) 
	{
		addrow(editor.y_coor, "", 0);
	} 
	else 
	{
		editor_row *currow = &editor.rows[editor.y_coor];
		addrow(editor.y_coor + 1, &currow->data[editor.x_coor],
			currow->size - editor.x_coor);
		currow = &editor.rows[editor.y_coor];
		currow->size = editor.x_coor;
		currow->data[currow->size] = '\0';
		rowupdate(currow);
	}
	editor.x_coor = 0;
	editor.y_coor++;
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
	if (editor.x_coor > 0) 
	{
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


char* rowstring(int *bufferleng) 
{
	char *buffer = NULL;
	char *point = NULL;
	int maxleng = 0;
	for (int i = 0; i < editor.rowcount; i++)
	{
		maxleng += editor.rows[i].size + 1;
	}
	*bufferleng = maxleng;
	buffer = malloc(maxleng);
	point = buffer;
	for (int i = 0; i < editor.rowcount; i++) 
	{
		memcpy(point, editor.rows[i].data, editor.rows[i].size);
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
	size_t introcapacity = 0;
	ssize_t leng = 0;
	while ((leng = getline(&introline, &introcapacity, openedfile) != 1)) 
	{
		while (leng > 0 && 
			(introline[leng - 1] == '\r' || introline[leng - 1] == '\n'))
		{
			leng--;
		}    
		addrow(editor.rowcount, introline, introcapacity);
	}  
	free(introline);
	fclose(openedfile);
	editor.modified = 0;
}


void save() 
{
	if (editor.file == NULL) 
	{    
		editor.file = userprompt("Save as: %s, Press ESC to cancel", NULL);
		if (editor.file == NULL) 
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
				editor.modified = 0;
				setstatus("%d data saved", leng);
				return;
			}
		}
		close(curfile);
	}
	free(buffer);
	setstatus("Save Error: %s", strerror(errno));
}


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
	else if (key == UP) 
	{
		direction = -1;
	}
	else if (key == DOWN) 
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


// Constructs the Dynamic String
void cons_dynamic(struct dynamicbuff *db, const char *buff, int leng) 
{
	char *new = realloc(db->buff, db->leng + leng);
	if (new == NULL) 
	{
		return;
	}
	memcpy(&new[db->leng], buff, leng);
	db->buff = new;
	db->leng += leng;
}


// This is the Destructor
void des_dynamic(struct dynamicbuff *db) 
{
	free(db->buff);
}


void scroll() 
{
	editor.rx_coor = 0;
	if (editor.y_coor < editor.rowcount) 
	{
		editor.rx_coor = cordfix(&editor.rows[editor.y_coor],
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


void row_chars(struct dynamicbuff *db) 
{  
	int row_count;
	for (row_count = 0; row_count < editor.row; row_count++) 
	{
		int filerows =  editor.row_offset + row_count;
		if (filerows >= editor.rowcount) 
		{
		// Intro
			if (editor.rowcount == 0 && row_count == editor.row / 3) 
			{
				char intro[80];
				int sizee = sizeof(intro);
				int introleng = snprintf(intro, sizee, "Tiny Texteditor - Harish Patel");
				if (introleng > editor.column)
				{
					introleng = editor.column;
				}
				int pad = (editor.column - introleng) / 2;
				if (pad) 
				{
					cons_dynamic(db, "#", 1);	
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
				cons_dynamic(db, "#", 1);			
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
			cons_dynamic(db, &editor.rows[filerows].tabrend[editor.column_offset], leng);
			/*
			char *ch = &editor.rows[filerows].tabrend[editor.column_offset];
			for (int i = 0; i < leng; i++) 
			{
				if (isdigit(ch[i]))
				 {
					cons_dynamic(db, "\x1b[31m", 5);
					cons_dynamic(db, &ch[i], 1);
					cons_dynamic(db, "\x1b[39m", 5);
				} else {
					cons_dynamic(db, &ch[i], 1);
				}
			}
			*/
		}
		cons_dynamic(db, "\x1b[K", 3);
		cons_dynamic(db, "\r\n", 2);
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
		if (editor.column - leng == renderleng) 
		{
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


void screen_refresh()
{  
	scroll();
	struct dynamicbuff db = DYNAMIC;
	// Set Mode
	cons_dynamic(&db, "\x1b[?25l", 6);
	cons_dynamic(&db, "\x1b[H", 3);
	row_chars(&db);
	statusbar(&db);
	mesgbar(&db);
	char buffer[32];
	// Position fix
	snprintf(buffer, sizeof(buffer), "\x1b[%d;%dH", (editor.y_coor - editor.row_offset) + 1, 
		(editor.rx_coor - editor.column_offset) + 1);
	cons_dynamic(&db, buffer, strlen(buffer));
	// Reset Mode
	cons_dynamic(&db, "\x1b[?25h", 6);
	write(STDOUT_FILENO, db.buff, db.leng);
	des_dynamic(&db);
}


void setstatus(const char *mesg, ...) 
{
	va_list argpassed;
	va_start(argpassed, mesg);
	vsnprintf(editor.statmesg, sizeof(editor.statmesg), mesg, argpassed);
	va_end(argpassed);
	editor.statmesg_time = time(NULL);
}


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
		if (ch == DELETE || ch == CTRL_KEY('h') || ch == BACKSPACE) 
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
	editor_row *currow = (editor.y_coor >= editor.rowcount) ? NULL : &editor.rows[editor.y_coor];
	switch (keys) 
	{
		case UP:
		if (editor.y_coor != 0)
		{
			editor.y_coor--;				
		}
		break;

		case LEFT:
		if (editor.x_coor != 0)
		{
			editor.x_coor--;			
		} 
		else if (editor.y_coor > 0) 
		{
			editor.y_coor--;
			editor.x_coor = editor.rows[editor.y_coor].size;
		}
		break;

		case DOWN:
		if (editor.y_coor < editor.rowcount)
		{
			editor.y_coor++;			
		}
		break;

		case RIGHT:
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
	currow = (editor.y_coor >= editor.rowcount) ? NULL : &editor.rows[editor.y_coor];
	int rowlen = currow ? currow->size : 0;
	if (editor.x_coor > rowlen)
	{
		editor.x_coor = rowlen;
	}
}


void process_editor() 
{  
	int ch = read_editor();
	static int quits = QUIT_COUNT;
	switch (ch) 
	{
		case CTRL_KEY('q'):
		if (editor.modified && quits > 0) 
		{
			setstatus("Unsaved, %d Changes. Press Ctrl-Q again to Quit", editor.modified);
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

		case CTRL_KEY('l'):

		case CTRL_KEY('f'):
		search();
		break;

		case '\r':
		insertendline();
		break; 

		case '\x1b':
		break;

		case UP:
		case LEFT:
		case DOWN:
		case RIGHT:
		cursormovement_editor(ch);
		break;

		case PAGE_UP:		
		case PAGE_DOWN:
		{       
			if (ch == PAGE_UP) 
			{
				editor.y_coor = editor.row_offset;
			} 
			else if (ch == PAGE_DOWN) 
			{
				editor.y_coor = editor.row_offset + editor.row - 1;
				if (editor.y_coor > editor.rowcount) 
				{
					editor.y_coor = editor.rowcount;
				}
			}
			int amunt = editor.row;
			while (amunt--)
				cursormovement_editor(ch == PAGE_UP ? UP : DOWN);
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
		case CTRL_KEY('h'):
		case DELETE:
		if (ch == DELETE)
		{
			cursormovement_editor(RIGHT);
		}
		deletechar();
		break;
		
		default:
		insertchar(ch);
		break;
	}
	quits = QUIT_COUNT;
}


void start_editor() 
{
	editor.x_coor = 0;
	editor.y_coor = 0;
	editor.rowcount = 0;
	editor.rows = NULL;
	editor.row_offset = 0;	
	editor.column_offset = 0;
	editor.rx_coor = 0;
	editor.file = NULL;
	editor.statmesg[0] = '\0';
	editor.statmesg_time = 0;
	editor.modified = 0;
	if (editor_size(&editor.row, &editor.column) == -1)
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
		openfile(argv[1]);
	}
	setstatus("HELP: Ctrl-S = save && Ctrl-Q = quit && Ctrl-F = Search");
	while (1) 
	{
		screen_refresh();
		process_editor();
	}
}