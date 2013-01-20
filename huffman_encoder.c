/***********************************************************************
 * Huffman file encoder / decoder
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

#define READ_SIZE 512

void get_probabilities(FILE *input_file, unsigned long n_symbols, unsigned long *probs)
{
	unsigned long current_symbol;
	unsigned char *buffer, *buffer_point;
	unsigned int bytes_read, i, j;
	unsigned int length = n_symbols / 256;


	buffer = malloc(length * READ_SIZE);

	// Initialize the probabilities vector to 0
	for (i = 0; i < n_symbols; i++)
		probs[i] = 0;

	// Get the probabilities of each symbol in the file
	while(!feof(input_file)) {
		bytes_read = fread(buffer, length, READ_SIZE, input_file);
		buffer_point = buffer;

		for (; bytes_read > 0; bytes_read--) {
			current_symbol = 0;

			for (j = 0; j < length; j++) {
				current_symbol += *buffer_point * pow_int(256, j);
				buffer_point++;
			}
			probs[current_symbol]++;
		}
	}
	free(buffer);
}

void create_tree(unsigned long *probs, unsigned long n_symbols, unsigned long *tree)
{
	unsigned long total_symbols = n_symbols;
	unsigned long indexes[n_symbols];
	unsigned int i;
	for (i = 0; i < n_symbols; i++)
		indexes[i] = i;


	for (i = 0; i < n_symbols - 1; i++) {
		bubble_sort(probs, indexes, total_symbols, 1);

		tree[i * 2] = indexes[total_symbols - 2];
		tree[(i * 2) + 1] = indexes[total_symbols - 1];

		probs[total_symbols - 2] += probs[total_symbols - 1];

		total_symbols--;
	}
}

void create_huff_code( unsigned long *tree, unsigned long n_symbols, code *codefinal, unsigned long *order)
{
	unsigned long i, j;
	code *codeh;
	codeh = malloc(n_symbols * 2 * sizeof(code));

	for (i = 0; i < n_symbols; i++)
		codeh[i].length = 0;


	for (i = 0; i < n_symbols; i++)
		order[i] = i;

	for (i = 0; i < n_symbols; i++)
		codeh[i].children.length = 0;

	unsigned int parent;
	for (i = 0; i < (n_symbols - 1); i++) {
		parent = tree[i * 2];
		fill(0, codeh, parent);
		parent = tree[(i * 2) +1 ];
		fill(1, codeh, parent);
		codeh[tree[i * 2]].children.value[codeh[tree[i * 2]].children.length] = tree[(i * 2) + 1];
		codeh[tree[i * 2]].children.length++;
	}

	unsigned long ordercopy[n_symbols];
	for (i = 0; i < n_symbols; i++)
		ordercopy[i] = order[i];

	unsigned long indexes[n_symbols];
	for (i = 0; i < n_symbols; i++)
		indexes[i] = i;
	bubble_sort(ordercopy, indexes, n_symbols, -1);

	code codefinalre[n_symbols];


	for (i = 0; i < n_symbols; i++)
		codefinalre[i] = codeh[indexes[i]];
	for (i = 0; i < n_symbols; i++) {
		for (j = 0; j < codefinalre[i].length; j++)
			codefinal[i].value[j] = codefinalre[i].value[codefinalre[i].length - 1 - j];

		codefinal[i].length = codefinalre[i].length;
	}
	//Show final codes
	/*
	for (i = 0; i < n_symbols; i++) {
		printf("%lu - ",i);
		for (j = 0; j < codefinal[i].length; j++)
			printf("%i",codefinal[i].value[j]);
		printf("\n");
	}*/
}

int encode(FILE *input_file, unsigned long file_size, FILE *output_file, unsigned int length)
{
	unsigned long i, j, n_symbols, current_symbol, tmp;
	unsigned long *probs;
	unsigned char *buffer;
	code *codefinal;
	unsigned long *order;


	n_symbols = pow_int(2, 8 * length);

	buffer = malloc(length * READ_SIZE);
	probs = malloc(pow_int(2, 8 * length) * sizeof(unsigned long));

	get_probabilities(input_file, n_symbols, probs);

	unsigned long tree[(n_symbols - 1) * 2];

	create_tree(probs, n_symbols, tree);

	// Show packed tree
	for (i = 0; i < (n_symbols - 1); i++)
		printf("(%lu, %lu) ", tree[i * 2], tree[i * 2 + 1]);
	printf("\n");

	order = malloc(n_symbols * sizeof(unsigned long));
	codefinal = malloc(n_symbols * 2 * sizeof(code));
	create_huff_code(tree, n_symbols, codefinal, order);

	fseek(input_file, 0L, SEEK_SET);
	unsigned char binarybuffer[n_symbols];
	unsigned long binarybuffersize = 0;

	unsigned char *outbuffer;
	outbuffer = malloc(length);

	//Headers of file
	//X will be the number of bits added to create the last byte
	char filetype[] = ".huf1.0X";
	for (i = 0; i < 8; i++)
		fwrite(&filetype[i], 1, 1, output_file);

	fwrite(&length, 1, 1, output_file);

	for (i = 0; i < n_symbols; i++)
		fwrite(&order[i], sizeof(unsigned long) ,1 ,output_file);

	for (i = 0; i < (n_symbols - 1) * 2; i++)
		fwrite(&tree[i], sizeof(unsigned long), 1, output_file);


	for (i = 0; i < file_size/length; i++) {
		fread(buffer, length, 1, input_file);
		current_symbol = 0;
		for (j = 0; j < length; j++)
			current_symbol += buffer[j]*pow_int(2,8*(length - 1 - j));

		for (j = 0; j < codefinal[current_symbol].length; j++)
			binarybuffer[binarybuffersize+j] = codefinal[current_symbol].value[j];

		binarybuffersize += codefinal[current_symbol].length;

		while (binarybuffersize >= length*8){
			*outbuffer = 0;
			for (j = 0; j < length*8; j++)
				*outbuffer += binarybuffer[length*8 - 1 - j] * pow_int(2,j);

			fwrite(outbuffer ,length ,1 ,output_file);

			for (j = 0; j < binarybuffersize - length*8; j++)
				binarybuffer[j] = binarybuffer[j+length*8];

			binarybuffersize -= length*8;
		}
	}
	unsigned char filled = length*8 - binarybuffersize;
	*outbuffer = 0;
	for (j = 0; j < length*8; j++)
		*outbuffer += binarybuffer[length*8 - 1 - j] * pow_int(2,j);

	fwrite(outbuffer ,length ,1 ,output_file);

	fseek(output_file, 7 * length, SEEK_SET);
	fwrite(&filled, 1, 1, output_file);

	free(probs);
	free(order);
	free(outbuffer);

	return 0;
}

int decode(FILE *input_file, unsigned long file_size, char *input_file_name, FILE *output_file)
{
	unsigned long i, j, k;

	char filetype[] = ".huf1.0";

	unsigned char *bufferheader;
	bufferheader = malloc(7);

	fread(bufferheader ,7 ,1 , input_file);
	unsigned char filled;
	fread(&filled, 1, 1, input_file);

	for (i = 0; i < 4; i++) {
		if (filetype[i] != bufferheader[i])
			break;
	}
	if (i != 4) {
		printf("File %s is not a valid huffman file\n", input_file_name);
		return 2;
	}
	printf("- Huffman file %s is version %c%c%c\n", input_file_name, bufferheader[4], bufferheader[5], bufferheader[6]);

	unsigned char length;
	fread(&length, 1, 1, input_file);
	printf("- Length = %u\n", length);

	unsigned long n_symbols = pow_int(2, 8 * length);
	unsigned long order[n_symbols];
	unsigned long tree[(n_symbols - 1) * 2];

	printf("- Number of symbols: %lu\n", n_symbols);

	fread(order, n_symbols, sizeof(unsigned long), input_file);
	fread(tree, (n_symbols - 1) * 2, sizeof(unsigned long), input_file);

	code codefinal[n_symbols];

	create_huff_code(tree, n_symbols, codefinal, order);

	unsigned char buffer = 0;

	unsigned char binarybuffer[n_symbols*2];
	int binarybuffersize = 0;

	i = (unsigned long) ftell(input_file);

	while (!feof(input_file)) {
		while (binarybuffersize < n_symbols && !feof(input_file)) {
			fread(&buffer, 1, 1, input_file);
			for (j = 0; j < 8 * length; j++) {
				binarybuffer[binarybuffersize - j + 8 * length - 1] = buffer % 2;
				buffer = buffer/2;
			}
			binarybuffersize += 8 * length;
		}

		if (feof(input_file))
			binarybuffersize -= filled + length * 8;

		if (binarybuffersize > 0) {
			for (j = 0; j < n_symbols; j++) {
				for (k = 0; k < codefinal[j].length; k++) {
					if (codefinal[j].value[k] != binarybuffer[k])
						break;
				}

				if (codefinal[j].length == k) {
					fwrite(&j,length,1,output_file);
					for (k = 0; k < n_symbols * 2 - codefinal[j].length; k++)
						binarybuffer[k] = binarybuffer[k+codefinal[j].length];

					binarybuffersize -= codefinal[j].length;
					break;
				}
			}
		}
	}
	while (binarybuffersize > 0) {
		for (j = 0; j < n_symbols; j++) {
			for (k = 0; k < codefinal[j].length; k++){ // <--
				if (codefinal[j].value[k] != binarybuffer[k])
					break;
			}

			if (codefinal[j].length == k) {
				fwrite(&j,length,1,output_file);
				for (k = 0; k < n_symbols * 2 - codefinal[j].length; k++)
					binarybuffer[k] = binarybuffer[k+codefinal[j].length];

				binarybuffersize -= codefinal[j].length;
				break;
			}
		}
	}
	free(bufferheader);

return 0;
}

int process_command(char command, char *input_file_name, char *output_file_name, unsigned int length)
{
	printf("- Opening file %s\n", input_file_name);
	FILE *input_file = fopen(input_file_name,"rb");
	if (!input_file) {
		printf("File %s doesn't exist\n", input_file_name);
		return 2;
	}
	FILE *output_file = fopen(output_file_name,"wb");
	if (!output_file) {
		printf("Error with output file %s\n", output_file_name);
		return 3;
	}

	unsigned long file_size = get_file_size(input_file_name);

	switch (command) {
	case 'e':
		printf(">>> Encoding\n");
		printf("- File size: %lu bytes\n", file_size);
		encode(input_file, file_size, output_file, length);

		unsigned long file_sizecomp = get_file_size(output_file_name);
		printf("- Compressed file size: %lu bytes\n", file_sizecomp);
		printf("- Compression ratio: %f\n", (float) file_size / file_sizecomp);

		break;

	case 'd':
		printf(">>> Decoding\n");
		decode(input_file, file_size, input_file_name, output_file);

		break;
	}

	fclose(input_file);
	fclose(output_file);
}

int main(int argc, char **argv)
{
	char command = 'n';
	char help_flag = 0;
	char non_option_flag = 0;
	unsigned int length = 1;
	char *input_file_name = NULL;
	char *output_file_name = NULL;
	int i, c;

	opterr = 0;

	while ((c = getopt (argc, argv, "edhl:i:o:")) != -1) {
		switch (c) {
		case 'e':
			command = 'e';
			break;
		case 'd':
			command = 'd';
			break;
		case 'h':
			help_flag = 1;
			break;
		case 'l':
			length = *optarg - 48;
			printf("Length = %i\n",length);
			break;
		case 'i':
			input_file_name = optarg;
			break;
		case 'o':
			output_file_name = optarg;
			break;
		case '?':
			if (optopt == 'c')
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint (optopt))
				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
			else
				fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
			return 1;
		default:
			abort();
		}
	}

	for (i = optind; i < argc; i++) {
		if (!non_option_flag)
			non_option_flag = 1;
		printf("Non-option argument: %s\n", argv[i]);
	}

	if (non_option_flag)
		return 1;

	else if (help_flag || (argc == 1))
		show_help(argv[0]);

	else if (command != 'n') {

		if (!input_file_name)
			printf("You need to specify an input file\n");

		else if (!output_file_name)
			printf("You need to specify an output file\n");

		else if (length < 1)
			printf("You need to specify a positive length\n");

		else
			process_command(command, input_file_name, output_file_name, length);

	}
	return 0;
}