// This file contains the includes and defines that are needed to run the program
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "info.h"

// Bitwise-ANDs a character with the value 00011111
#define CTRL_KEY(k) ((k) & 0x1f)