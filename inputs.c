#include "inputs.h"

void process_editor() 
{  
char ch = read_editor();
  switch (ch) {
    case CTRL_KEY('q'):
    esc_clear();
    esc_cursor();
    exit(0);
    break;
  }
}