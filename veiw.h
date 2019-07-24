/*
This file contains all the information for inputs given to the editor
*/
#include "include.h"

// \x1b is the escape character 
// 2 is to clear the entire screen
// J clear screen
const string esc_sequence_clear = "\x1b[2J";
// H takes the row number and column number for cursor
const string esc_sequence_cursor = "\x1b[H";
// Start of each row will be this 
const string row_print = "#\r\n";

// Refresh screen;
void esc_clear();

// Reposition the cursor
void esc_cursor();

// Function to display characters for each row
void row_chars();

// Function to clear the screen 
void screen_refresh();