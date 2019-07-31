#include "terminal.h"

void kill(const char *ch) 
{
  // When exiting the terminal Refresh and clear the cursor
  void esc_clear();
  void esc_cursor();
  
  perror(ch);
  exit(1);
}


void disablecho() 
{
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &editor.terminall) == -1)
    die(terminal_attributes);
}


void enablecho()
{
  if (tcgetattr(STDIN_FILENO, editor.terminall) == -1) {
    kill(terminal_attributes);
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
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &echoo) == -1) {
    kill(terminal_attributes);
  }
}


int editor_size(int *column, int *row)
{
  struct winsize editsize;
  if (editsize.ws_col == 0)
  {
    return -1;
  }
  else if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &editsize) == -1)
  {
    return -1;      
  }
  else
  {
    *column = editsize.ws_col; 
    *row = editsize.ws_row;
  }
}


char read_editor() 
{
  int key;
  char ch;
  const string READ_ERROR = "read";
  while ((key = read(STDIN_FILENO, &ch, 1)) != 1)
  {
    if (key == -1 && errno != EAGAIN) 
    {
      kill(READ_ERROR);
    }
  }
  return ch;
}


int get_cursor(int *column, int *row) 
{
  char buffer_zone[32];  
  if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) 
  {
    return -1;
  }
  for (unsigned int i = 0; i < sizeof(buffer_zone) - 1; i++) 
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


int get_terminal(int *column, int *row) 
{
  struct winsize lcl_terminal;
  if ( ioctl(STDOUT_FILENO, TIOCGWINSZ, &lcl_terminal) == -1 || lcl_terminal.ws_col == 0) 
  {
    if (write(STDOUT_FILENO, cursor_val, 12) != 12) 
    {
      return -1;
    }
    return get_cursor(column, row);
  } 
  else 
  {
    *cols = lcl_terminal.ws_col;
    *rows = lcl_terminal.ws_row;
    return 0;
  }
}