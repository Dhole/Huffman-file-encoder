/***********************************************************************
 * Functions for huffman_encoder.c
 *
 * version: 0.2
 *
 * Authors: Dhole
 ***********************************************************************/

#ifndef HUFFMAN_FUNCTIONS_H
#define HUFFMAN_FUNCTIONS_H

typedef struct {
	unsigned int value[256];
	unsigned int length;
} code;

// Calcules base^exp
unsigned long pow_int(unsigned long base, unsigned long exp);

void bubble_sort2(unsigned long numbers[], unsigned long indexes[], unsigned int order[],  int array_size);

void bubble_sort(unsigned long num[], unsigned long ind[], int array_size, long order);

int show_help(char program_name[]);

unsigned long get_file_size(char *file_name);

int fill(unsigned char value, code codeh[], unsigned long parent);

#endif  /* HUFFMAN_FUNCTIONS_H */