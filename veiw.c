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
	int row_count = editor.row;
	for (int i = 0; i < row_count; i++)
	{
		write(STDOUT_FILENO, row_print, 3);
	}
}
  

void screen_refresh()
{
	esc_clear();
	esc_cursor();
	row_chars();
	esc_cursor();
}