#include "decoder.h"

void PRINT(FILE* dcd, int len, char* str)
{
	int i;
	for (i = 0; i < len; i++)
	{
		fwrite(&str[i], sizeof(char), 1, dcd);
	}
}


tbl* init_table(uint32_t size)
{
	tbl* tmp_buf_for_this_func = (tbl*)calloc(size, sizeof(tbl));
	return tmp_buf_for_this_func;
}

void create_table(tbl* table)
{
	char* s = NULL;
	int i;

	for (i = 0; i < 256;i++)//точно работает
	{
		s = (char*)malloc(sizeof(char));
		s[0] = i;
		table[i].len = 1;
		table[i].string = s;
	}
}

int mypw(int osn, int dg)
{
	int rez = 1;
	while (dg > 0)
	{
		rez *= osn;
		dg--;
	}
	return rez;
}

void convert_from_temp_memory_for_write_to_new_elem_from_file(
	uint32_t* new_elem_from_file,
	uint32_t* temp_memory_for_write,
	int* using_place,
	int sz)
{
	uint32_t tmp = *temp_memory_for_write;
	tmp >>= (*using_place - sz);
	*new_elem_from_file = tmp;
	tmp = *temp_memory_for_write;
	tmp <<= (sizeof(int) * 8 - *using_place + sz);
	tmp >>= (sizeof(int) * 8 - *using_place + sz);
	if (*using_place == sz)
	{
		tmp = 0;
	}
	*temp_memory_for_write = tmp;
	*using_place = *using_place - sz;
}

char* search_elem_in_table(tbl elem, int* len, int kol)
{
	int i;
	char* s;
	s = (char*)malloc((int)elem.len * sizeof(char) + kol);
	i = 0;
	while (i < elem.len)
	{
		s[i] = elem.string[i];
		i++;
	}
	*len = i;
	return s;
}

tbl* add_in_table(int  buf, char symbol, tbl* table, int mx_numb)
{
	int i = 0;
	char* s = (char*)malloc(table[buf].len * sizeof(char) + 1);
	while (i < table[buf].len)
	{
		s[i] = table[buf].string[i];
		i++;
	}
	s[i] = symbol;
	table[mx_numb].len = ++i;
	table[mx_numb].string = s;
}

void clear_table_and_free_pointer(tbl* table, uint32_t size)
{
	int i;
	for (i = 0; i < size; i++)
	{
		table[i].len = 0;
		free(table[i].string);
	}
}

void convert_from_new_b_to_temp_memory_for_write(
	uint32_t* temp_memory_for_write, 
	int new_b, 
	int* using_place)
{
	*temp_memory_for_write <<= 8;
	*temp_memory_for_write |= new_b;
	*using_place += 8;
}



int decode(FILE* f, FILE* dcd, short max_number_using_bits)
{
	const int new_elem_init = 1;
	const int vse_ok = 0;

	int buf = 0;
	int new_b = 0;
	int sz;
	int using_place;
	int mx_numb;
	unsigned char symbol;
	int length = 0;
	uint32_t temp_memory_for_write = 0;
	uint32_t new_elem_from_file = 0;
	char* str = NULL;
	int ch = 512;



	tbl* table = NULL;


	uint32_t size = mypw(2, max_number_using_bits);
	table = init_table(size);
	if (table == NULL)
	{
		printf("ERROR! Not enough memory\n");
		return;
	}
	create_table(table);

	sz = 9;
	mx_numb = 255;
	using_place = 0;
	if (fread(&buf, sizeof(char), 1, f) == 0)
	{
		return;
	}

	symbol = buf;
	PRINT(dcd, 1, &symbol);
	fread(&new_b, sizeof(char), 1, f);
	convert_from_new_b_to_temp_memory_for_write(
		&temp_memory_for_write,
		new_b,
		&using_place);
	while (fread(&new_b, sizeof(char), 1, f) == 1)
	{
		if (using_place < sz)
		{
			convert_from_new_b_to_temp_memory_for_write(
				&temp_memory_for_write,
				new_b,
				&using_place);
		}
		else
		{
			convert_from_temp_memory_for_write_to_new_elem_from_file(
				&new_elem_from_file,
				&temp_memory_for_write,
				&using_place,
				sz);

			convert_from_new_b_to_temp_memory_for_write(
				&temp_memory_for_write,
				new_b,
				&using_place);

			length = 0;
			if (new_elem_from_file > mx_numb)
			{
				if (new_elem_from_file - 1 != mx_numb)
				{
					printf("ERROR! Incorrect coding\n");
					return;
				}
				str = search_elem_in_table(table[buf], &length, new_elem_init);

				str[length] = symbol;
				length++;
			}
			else
			{
				str = search_elem_in_table(table[new_elem_from_file], &length, vse_ok);

			}

			PRINT(dcd, length, str);
			symbol = str[0];
			mx_numb++;
			add_in_table(buf, symbol, table, mx_numb);
			buf = new_elem_from_file;
			if (mx_numb >= ch - 1)
			{
				++sz;
				ch *= 2;
				if (sz - 1 == max_number_using_bits)
				{

					clear_table_and_free_pointer(table, size);

					create_table(table);
					buf = new_b;
					symbol = new_b;
					PRINT(dcd, 1, &symbol);
					fread(&new_b, sizeof(char), 1, f);

					temp_memory_for_write = 0;
					using_place = 0;

					convert_from_new_b_to_temp_memory_for_write(
						&temp_memory_for_write,
						new_b,
						&using_place);
					ch = 512;
					mx_numb = 255;
					sz = 9;
				}
			}
		}
	}
	fclose(f);
	while (using_place >= sz)
	{
		convert_from_temp_memory_for_write_to_new_elem_from_file(&new_elem_from_file, &temp_memory_for_write, &using_place, sz);
		length = 0;
		if (new_elem_from_file > mx_numb)
		{
			if (new_elem_from_file - 1 != mx_numb)
			{
				printf("ERROR! Incorrect coding\n");
				return;
			}
			str = search_elem_in_table(table[buf], &length, new_elem_init);

			str[length] = symbol;
			length++;
		}
		else
		{
			str = search_elem_in_table(table[new_elem_from_file], &length, vse_ok);
		}

		PRINT(dcd, length, str);
		symbol = str[0];
		mx_numb++;
		add_in_table(buf, symbol, table, mx_numb);
		buf = new_elem_from_file;
		if (mx_numb >= ch - 1)
		{
			++sz;
			ch *= 2;
			if (sz - 1 == max_number_using_bits)
			{
				clear_table_and_free_pointer(table, size);
				using_place = 0;
				create_table(table);
				buf = new_b;
				symbol = (unsigned char)new_b;
				PRINT(dcd, 1, &symbol);
				ch = 512;
				mx_numb = 255;
				sz = 9;
			}
		}
	}
	return 0;
}