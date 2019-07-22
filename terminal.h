/*
This file contains all the information for building the terminal field
*/

#include "include.h"

struct termios TERMINAL;

void kill(const char *s) 
{
  perror(s);
  exit(1);
}

void disablecho() 
{
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &TERMINAL) == -1)
    die("tcsetattr");
}

void enablecho() 
{
  if (tcgetattr(STDIN_FILENO, &TERMINAL) == -1) {
    kill("tcgetattr");
  }
  atexit(disablecho);
  struct termios raw = TERMINAL;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
    kill("tcsetattr");
  }
}
