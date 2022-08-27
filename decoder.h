#include <stdio.h>
#include <malloc.h>
#include <stdint.h>

typedef struct Table
{
	int len;
	char* string;
}tbl;


void PRINT(FILE* dcd, int len, char* str);

tbl* init_table(uint32_t size);

void CreateTable(tbl* table);

int mypw(int osn, int dg);

void convert_from_temp_memory_for_write_to_new_elem_from_file(
	uint32_t* new_elem_from_file,
	uint32_t* temp_memory_for_write,
	int* using_place,
	int sz);

char* search_elem_in_table(tbl elem, int* len, int kol);

tbl* add_in_table(int  buf, char symbol, tbl* table, int mx_numb);

void clear_table_and_free_pointer(tbl* table, uint32_t size);

void convert_from_new_b_to_temp_memory_for_write(
	uint32_t* temp_memory_for_write,
	int new_b,
	int* using_place);

int decode(FILE* f, FILE* dcd, short max_number_using_bits);