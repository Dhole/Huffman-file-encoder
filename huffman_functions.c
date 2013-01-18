/***********************************************************************
 * Functions for huffman_encoder.c
 *
 * version: 0.2
 *
 * Authors: Dhole
 ***********************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include "huffman_functions.h"

unsigned long pow_int(int base, int exp)
{
	int res = 1;
	for (; exp > 0; exp--)
		res = res*base;

	return res;
}

void bubble_sort2(unsigned long numbers[], unsigned long indexes[], unsigned int order[],  int array_size)
{
	int i, j, temp;

	for (i = (array_size - 1); i > 0; i--) {
	for (j = 1; j <= i; j++) {
		if (numbers[j - 1] < numbers[j])
		{
			temp = numbers[j - 1];
			numbers[j - 1] = numbers[j];
			numbers[j] = temp;

			temp = indexes[j - 1];
			indexes[j - 1] = indexes[j];
			indexes[j] = temp;

			temp = order[j - 1];
			order[j - 1] = order[j];
			order[j] = temp;
		}
	}
	}
}

void bubble_sort(unsigned long num[], unsigned long ind[], int array_size, long order)
{
	int i, j, tmp;

	for (i = (array_size - 1); i > 0; i--) {
	for (j = 1; j <= i; j++) {
		if (order * (long)num[j - 1] < order * (long)num[j]) {
			tmp = num[j - 1];
			num[j - 1] = num[j];
			num[j] = tmp;

			tmp = ind[j - 1];
			ind[j - 1] = ind[j];
			ind[j] = tmp;
		}
	}
	}
}

int show_help(char program_name[])
{
	printf("Huffman encoder (March 2012). \n\n"
	"Usage: %s [-e/-d] [-i input_file] [-o outpufile]\n"
	"  -e  encode\n"
	"  -d  decode\n"
	"  -l  lenght (1 if omitted)\n"
	"  -h  help\n"
	"\nLimitations: Max file size = 4GiB\n", program_name);
	return 0;
}

unsigned long get_file_size(char *file_name)
{
	struct stat st;
	stat(file_name, &st);
	return st.st_size;
}

int fill(unsigned char value, code codeh[], unsigned long parent)
{
	codeh[parent].value[codeh[parent].length] = value;
	codeh[parent].length++;
	unsigned int current_children_length = codeh[parent].children.length;
	while ( current_children_length != 0 ) {
		fill(value, codeh, codeh[parent].children.value[current_children_length - 1]);
		current_children_length--;
	}
}