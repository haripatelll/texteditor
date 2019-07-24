// This file contains the includes and defines that are needed to run the program
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

// Bitwise-ANDs a character with the value 00011111
#define CTRL_KEY(k) ((k) & 0x1f)