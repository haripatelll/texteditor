// This file contains the includes and defines that are needed to run the program
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include "info.h"

// Bitwise-ANDs a character with the value 00011111
#define CTRL_KEY(k) ((k) & 0x1f)

// Designates the keys to characters
enum arrowkeys 
{
  LEFT_ARROW = 1000, RIGHT_ARROW = 1001, 
  UP_ARROW = 1002, DOWN_ARROW = 1003,
  UP_PAGE = 1004, DOWN_PAGE = 1005,
  HOME  = 1006, END = 1007,
  DELETE = 1008
};

/* 
Dynmaic String 
This Is an Append Buffer
*/
// Constructor
#define DYNAMIC {NULL, 0}

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
void des_dynamic(struct abuf *db) 
{
	free(db->buff);
}