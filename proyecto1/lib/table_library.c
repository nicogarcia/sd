#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "table_library.h"

table* table_initialize(int columns, int* widths) {
	int i;
	table* t = (table*) malloc(sizeof(struct s_table));

	t->widths = (int*) malloc(sizeof(int*) * columns);
	t->width = 0;

	for (i = 0; i < columns; i++) {
		t->width += widths[i];
		t->widths[i] = widths[i];
	}

	t->columns = columns;
	t->rows = 0;
	t->initialized = 1;
	t->rows_data = NULL;

	return t;
}

void table_add_row(table* t) {
	int i;

	if (t->initialized != 1) {
		printf(" ERROR: Attempt to add a row on an uninitialized table\n");
		return;
	}

	// Grow rows
	t->rows_data = (char** *) realloc(t->rows_data,	sizeof(char**) * (t->rows + 1));

	// Create row
	t->rows_data[t->rows] = (char**) malloc(sizeof(char*) * t->columns);

	// Allocate row strings
	for (i = 0; i < t->columns; i++) {
		t->rows_data[t->rows][i] = (char*) malloc(sizeof(char*) * t->widths[i]);
		t->rows_data[t->rows][i][0] = 0;
	}

	t->rows++;
}

void table_add_data(table* t, int row, int column, char* format, ...) {
	if (row >= t->rows && column >= t->columns) {
		printf("*** ERROR: Attempt to add data in bad index (%i, %i)\n", row,
				column);
		return;
	}

	va_list arguments;

	va_start(arguments, format);
	vsprintf(t->rows_data[row][column], format, arguments);
	va_end(arguments);
}

void table_print(table* t) {
	int i, j, pos = 0;
	int actual_table_width;
	char* line;
	char* separator;

	if (t->initialized != 1) {
		printf(" ERROR: Attempt to print an uninitialized table\n");
		return;
	}

	// Line initialization
	actual_table_width = t->width + 3 * t->columns + 1;
	line = (char*) malloc(sizeof(char*) * actual_table_width);

	separator = (char*) malloc(sizeof(char*) * actual_table_width);
	separator[actual_table_width - 1] = 0;

	// Print header (first row)
	separator[pos++] = '+';
	for (i = 0; i < t->columns; i++) {
		separator[pos++] = '-'; // cell space
		for (j = 0; j < t->widths[i]; j++)
			separator[pos++] = '-';
		separator[pos++] = '-'; // cell space
		separator[pos++] = '+';
	}
	printf("%s\n", separator);

	// Print the rest
	for (i = 0; i < t->rows; i++) {
		pos = 0;
		memset(line, ' ', actual_table_width);
		line[pos++] = '|';
		for (j = 0; j < t->columns; j++) {
			line[pos++] = ' ';

			strcpy(line + pos, t->rows_data[i][j]);
			line[strlen(t->rows_data[i][j]) + pos] = ' ';
			pos += t->widths[j];

			line[pos++] = ' ';
			line[pos++] = '|';
		}
		printf("%s\n", line);
		printf("%s\n", separator);
	}

}

