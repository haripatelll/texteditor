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


void row_chars() 
{  
	int row_count;
	for (row_count = 0; row_count < editor.row; row_count++) 
	{
		write(STDOUT_FILENO, startline, 1);
		if (row_count < editor.row - 1) 
		{
			write(STDOUT_FILENO, "\r\n", 2);
		}
	}
}  

void screen_refresh()
{
	esc_clear();
	esc_cursor();
	row_chars();
	esc_cursor();
}