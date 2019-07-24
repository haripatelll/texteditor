#include "include.h"
#include "terminal.h"
#include "inputs.h"
#include "view.h"
#include "info.h"

void start_editor() 
{
	if (editor_size(&editor.column, &editor.row) == -1)
	{
		kill("Editor Size");
	}
	else 
	{

	}
}


int main() 
{
	enablecho();
	start_editor();
	screen_refresh():
	process_editor();

}