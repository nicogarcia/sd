#ifndef TABLE_LIBRARY_H
#define TABLE_LIBRARY_H

typedef struct s_table{
	int initialized;
	int columns;
	int rows;
	int width;
	int* widths;
	char*** rows_data;
} table;

table* table_initialize(int columns, int* widths);

void table_add_row(table* t);

void table_add_data(table* t, int row, int column, char* format, ...);

void table_print(table* t);

#endif
