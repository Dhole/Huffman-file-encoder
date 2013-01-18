#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>


typedef struct {
	unsigned int value[256];
	unsigned int length;
} child;

typedef struct {
	unsigned int value[256];
	unsigned int length;
	child children;
} code;



unsigned long powInt(int base, int exp)
{
	int res;
	for (; exp > 0; exp--)
		res = res*base;

 	return res;
}

void bubbleSort2(unsigned long numbers[], unsigned long indexes[], unsigned int order[],  int array_size)
{
	int i, j, temp;

	for (i = (array_size - 1); i > 0; i--)
	{
		for (j = 1; j <= i; j++)
		{
			if (numbers[j-1] < numbers[j])
			{
				temp = numbers[j-1];
				numbers[j-1] = numbers[j];
				numbers[j] = temp;

				temp = indexes[j-1];
				indexes[j-1] = indexes[j];
				indexes[j] = temp;

				temp = order[j-1];
				order[j-1] = order[j];
				order[j] = temp;
			}
		}
	}
}

void bubbleSort(unsigned long numbers[], unsigned long indexes[], int array_size)
{
	int i, j, temp;

	for (i = (array_size - 1); i > 0; i--) {
		for (j = 1; j <= i; j++) {
			if (numbers[j-1] < numbers[j]) {
				temp = numbers[j-1];
				numbers[j-1] = numbers[j];
				numbers[j] = temp;

				temp = indexes[j-1];
				indexes[j-1] = indexes[j];
				indexes[j] = temp;
			}
		}
	}
}

void bubbleSortR(unsigned long numbers[], unsigned long indexes[], int array_size)
{
	int i, j, temp;

	for (i = (array_size - 1); i > 0; i--) {
		for (j = 1; j <= i; j++) {
			if (numbers[j-1] > numbers[j]) {
				temp = numbers[j-1];
				numbers[j-1] = numbers[j];
				numbers[j] = temp;

				temp = indexes[j-1];
				indexes[j-1] = indexes[j];
				indexes[j] = temp;
			}
		}
	}
}

int showHelp(char program_name[])
{
	printf("Huffman encoder (March 2012). \n\n"
			"Usage: %s [-e/-d] [-i inputfile] [-o outpufile]\n"
			"  -e  encode\n"
			"  -d  decode\n"
			"  -l  lenght (1 if omitted)\n"
			"  -h  help\n"
			"\nLimitations: Max file size = 4GiB\n", program_name);
	return 0;
}

unsigned long getFileSize(char *filename)
{
	struct stat st;
	stat(filename, &st);
	return st.st_size;
}

int fill(unsigned char value, code codeh[], unsigned long parent)
{
	codeh[parent].value[codeh[parent].length] = value;
	codeh[parent].length++;
	unsigned int currentchildrenlength = codeh[parent].children.length;
	while ( currentchildrenlength != 0 ) {
		fill(value, codeh, codeh[parent].children.value[currentchildrenlength-1]);
		currentchildrenlength--;
	}
}

int encode(char *input_file_value, char *output_file_value, unsigned char length_value)
{
	unsigned long i;
	unsigned long j;
	printf("  Encoding\n");
	printf("-Opening file %s\n", input_file_value);

	FILE *inputfile = fopen(input_file_value,"rb");
	if (!inputfile) {
		printf("File %s doesn't exist\n", input_file_value);
		return 2;
	}

	unsigned long filesize = getFileSize(input_file_value);
	printf("-File size: %lu bytes\n", filesize);

	unsigned long nsymbols = powInt(2,8*length_value);
	unsigned long currentsymbol = 0;

	unsigned char *buffer;
	buffer = malloc(length_value);

	printf("-Number of symbols: %lu\n",nsymbols);

	unsigned long *probs;
	probs = malloc(powInt(2,8*length_value)*sizeof(unsigned long));
	for (i = 0; i < nsymbols; i++)
		probs[i] = 0;

	for (i = 0; i < filesize/length_value; i++) {
		fread(buffer, length_value, 1, inputfile);
		currentsymbol = 0;
		for (j = 0; j < length_value; j++)
			currentsymbol += buffer[j]*powInt(2,8*(length_value - 1 - j));

		probs[currentsymbol]++;
	}

	FILE *probsresult = fopen("probs.txt","w");
	for (i=0; i < nsymbols; i++)
		fprintf(probsresult, "%lu\n",probs[i]);

	fclose(probsresult);

	unsigned long tree[(nsymbols - 1) * 2];
	unsigned long indexes[nsymbols];
	for (i = 0; i < nsymbols; i++)
		indexes[i] = i;

	unsigned long min1;
	unsigned long min2;
	unsigned int position1 = 0;
	unsigned int position2 = 0;
	unsigned long totalsymbols = nsymbols;

	code codeh[nsymbols*2];
	for (i = 0; i < nsymbols; i++)
		codeh[i].length = 0;

	unsigned long order[nsymbols];
	for (i = 0; i < nsymbols; i++)
		order[i] = i;

	bubbleSort(probs, order, totalsymbols);

	unsigned long tmp;
	for (i = 0; i < nsymbols - 1; i++) {
		bubbleSort(probs, indexes, totalsymbols);

		if ( indexes[totalsymbols-2] < indexes[totalsymbols-1] ) {
			tree[i * 2] = indexes[totalsymbols-2];
			tree[(i * 2) + 1] = indexes[totalsymbols-1];
		} else {
			tmp = probs[totalsymbols-2];
			probs[totalsymbols-2] = probs[totalsymbols-1];
			probs[totalsymbols-1] = tmp;

			tmp = indexes[totalsymbols-2];
			indexes[totalsymbols-2] = indexes[totalsymbols-1];
			indexes[totalsymbols-1] = tmp;

			tree[i * 2] = indexes[totalsymbols-2]; //0
			tree[(i * 2) + 1] = indexes[totalsymbols-1]; //1
		}

		probs[totalsymbols-2] += probs[totalsymbols-1];

		totalsymbols--;
	}

	printf("\n");

	for (i = 0; i < nsymbols; i++)
		codeh[i].children.length = 0;

	unsigned int parent;
	for (i = 0; i < (nsymbols - 1); i++) {
		parent = tree[i * 2];
		fill(0, codeh, parent);
		parent = tree[(i * 2) +1 ];
		fill(1, codeh, parent);
		codeh[tree[i * 2]].children.value[codeh[tree[i * 2]].children.length] = tree[(i * 2) + 1];
		codeh[tree[i * 2]].children.length++;
	}

	unsigned long ordercopy[nsymbols];
	for (i = 0; i < nsymbols; i++)
		ordercopy[i] = order[i];


	for (i = 0; i < nsymbols; i++)
		indexes[i] = i;
	bubbleSortR(ordercopy, indexes, nsymbols);

	code codefinalre[nsymbols];
	code codefinal[nsymbols];


	for (i = 0; i < nsymbols; i++)
		codefinalre[i] = codeh[indexes[i]];
	for (i = 0; i < nsymbols; i++) {
		for (j = 0; j < codefinalre[i].length; j++)
			codefinal[i].value[j] = codefinalre[i].value[codefinalre[i].length - 1 - j];

		codefinal[i].length = codefinalre[i].length;
	}
	//Show final codes

	/*for (i = 0; i < nsymbols; i++) {
	  printf("%lu - ",i);
	  for (j = 0; j < codefinal[i].length; j++)
	  printf("%i",codefinal[i].value[j]);
	  printf("\n");
	  }*/

	/*  for (i = 0; i < (nsymbols - 1); i++)
	    printf("(%u %u) ", tree[i * 2], tree[(i * 2) + 1]);*/

	/*  min1 = probs[0];

	    for (i = 0; i < nsymbols; i++) {
	    if (probs[i] < min1) {
	    min1 = probs[i];
	    position1 = i;
	    }
	    }
	    min2 = min1;
	    for (i = 0; i < nsymbols; i++) {
	    if (probs[i] <= min2 && i != position1) {
	    min2 = probs[i];
	    position2 = i;
	    }
	    }


	    printf("\nLes probabs mes petites son: %lu a %i i %lu a %i\n",\
	    min1, position1, min2, position2);*/
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	fseek(inputfile, 0L, SEEK_SET);
	unsigned char binarybuffer[nsymbols];
	unsigned long binarybuffersize = 0;

	unsigned char *outbuffer;
	outbuffer = malloc(length_value);

	FILE *outputfile = fopen(output_file_value,"wb");
	if (!outputfile) {
		printf("Error with output file %s\n", output_file_value);
		return 3;
	}

	//Headers of file
	//X will be the number of bits added to create the last byte
	char filetype[] = ".huf1.0X";
	for (i = 0; i < 8; i++)
		fwrite(&filetype[i] ,1 ,1 ,outputfile);

	fwrite(&length_value ,1 ,1 ,outputfile);

	for (i = 0; i < nsymbols; i++)
		fwrite(&order[i], sizeof(unsigned long) ,1 ,outputfile);

	for (i = 0; i < (nsymbols - 1) * 2; i++)
		fwrite(&tree[i], sizeof(unsigned long), 1, outputfile);


	for (i = 0; i < filesize/length_value; i++) {
		fread(buffer, length_value, 1, inputfile);
		currentsymbol = 0;
		for (j = 0; j < length_value; j++)
			currentsymbol += buffer[j]*powInt(2,8*(length_value - 1 - j));

		for (j = 0; j < codefinal[currentsymbol].length; j++)
			binarybuffer[binarybuffersize+j] = codefinal[currentsymbol].value[j];

		binarybuffersize += codefinal[currentsymbol].length;

		while (binarybuffersize >= length_value*8){
			*outbuffer = 0;
			for (j = 0; j < length_value*8; j++)
				*outbuffer += binarybuffer[length_value*8-1 - j] * powInt(2,j);

			fwrite(outbuffer ,length_value ,1 ,outputfile);

			for (j = 0; j < binarybuffersize - length_value*8; j++)
				binarybuffer[j] = binarybuffer[j+length_value*8];

			binarybuffersize -= length_value*8;
		}
	}
	unsigned char filled = length_value*8 - binarybuffersize;
	*outbuffer = 0;
	for (j = 0; j < length_value*8; j++)
		*outbuffer += binarybuffer[length_value*8-1 - j] * powInt(2,j);

	fwrite(outbuffer ,length_value ,1 ,outputfile);

	fseek(outputfile, 7 * length_value, SEEK_SET);
	fwrite(&filled, 1, 1, outputfile);

	free(probs);
	free(buffer);
	fclose(inputfile);
	fclose(outputfile);
	free(outbuffer);

	unsigned long filesizecomp = getFileSize(output_file_value);
	printf("-Compressed file size: %lu bytes\n", filesizecomp);
	printf("-Compression ratio: %f\n", (float) filesize / filesizecomp);

	return 0;
}

int decode(char *input_file_value, char *output_file_value)
{
	unsigned long i, j, k;
	printf("  Decoding\n");
	printf("-Opening file %s\n", input_file_value);

	FILE *inputfile = fopen(input_file_value,"rb");
	if (!inputfile) {
		printf("File %s doesn't exist\n", input_file_value);
		return 2;
	}

	char filetype[] = ".huf1.0";

	unsigned char *bufferheader;
	bufferheader = malloc(7);

	fread(bufferheader ,7 ,1 , inputfile);
	unsigned char filled;
	fread(&filled, 1, 1, inputfile);

	for (i = 0; i < 4; i++) {
		if (filetype[i] != bufferheader[i])
			break;
	}
	if (i != 4) {
		printf("File %s is not a valid huffman file\n", input_file_value);
		return 2;
	}
	printf("-Huffman file %s is version %c%c%c\n", input_file_value, bufferheader[4], bufferheader[5], bufferheader[6]);

	unsigned char length_value;
	fread(&length_value, 1, 1, inputfile);
	printf("-Length = %u\n", length_value);

	unsigned long nsymbols = powInt(2,8*length_value);
	unsigned long order[nsymbols];
	unsigned long tree[(nsymbols - 1) * 2];

	printf("-Number of symbols: %lu\n", nsymbols);

	fread(order, nsymbols, sizeof(unsigned long), inputfile);
	fread(tree, (nsymbols - 1) * 2, sizeof(unsigned long), inputfile);

	code codeh[nsymbols*2];
	for (i = 0; i < nsymbols; i++)
		codeh[i].length = 0;

	for (i = 0; i < nsymbols; i++)
		codeh[i].children.length = 0;

	unsigned int parent;
	for (i = 0; i < (nsymbols - 1); i++) {
		parent = tree[i * 2];
		fill(0, codeh, parent);
		parent = tree[(i * 2) +1 ];
		fill(1, codeh, parent);
		codeh[tree[i * 2]].children.value[codeh[tree[i * 2]].children.length] = tree[(i * 2) + 1];
		codeh[tree[i * 2]].children.length++;
	}

	unsigned long ordercopy[nsymbols];
	for (i = 0; i < nsymbols; i++)
		ordercopy[i] = order[i];

	unsigned long indexes[nsymbols];
	for (i = 0; i < nsymbols; i++)
		indexes[i] = i;

	bubbleSortR(ordercopy, indexes, nsymbols);

	code codefinalre[nsymbols];
	code codefinal[nsymbols];

	for (i = 0; i < nsymbols; i++)
		codefinalre[i] = codeh[indexes[i]];
	for (i = 0; i < nsymbols; i++) {
		for (j = 0; j < codefinalre[i].length; j++)
			codefinal[i].value[j] = codefinalre[i].value[codefinalre[i].length - 1 - j];
		codefinal[i].length = codefinalre[i].length;
	}

	FILE *outputfile = fopen(output_file_value,"wb");
	if (!outputfile) {
		printf("Error with output file %s\n", output_file_value);
		return 3;
	}
	unsigned char buffer = 0;

	unsigned char binarybuffer[nsymbols*2];
	int binarybuffersize = 0;

	i = (unsigned long) ftell(inputfile);
	unsigned long filesize = getFileSize(input_file_value);

	while (!feof(inputfile)) {
		while (binarybuffersize < nsymbols && !feof(inputfile)) {
			fread(&buffer, 1, 1, inputfile);
			for (j = 0; j < 8*length_value; j++) {
				binarybuffer[binarybuffersize-j+8*length_value-1] = buffer%2;
				buffer = buffer/2;
			}
			binarybuffersize += 8*length_value;
		}

		if(feof(inputfile))
			binarybuffersize -= filled+length_value*8;

		if ( binarybuffersize > 0) {
			for (j = 0; j < nsymbols; j++) {
				for (k = 0; k < codefinal[j].length; k++) {
					if (codefinal[j].value[k] != binarybuffer[k])
						break;
				}

				if (codefinal[j].length == k) {
					fwrite(&j,length_value,1,outputfile);
					for (k = 0; k < nsymbols*2-codefinal[j].length; k++)
						binarybuffer[k] = binarybuffer[k+codefinal[j].length];

					binarybuffersize -= codefinal[j].length;
					break;
				}
			}
		}
	}
	while (binarybuffersize > 0) {
		for (j = 0; j < nsymbols; j++) {
			for (k = 0; k < codefinal[j].length; k++){ // <--
				if (codefinal[j].value[k] != binarybuffer[k])
					break;
			}

			if (codefinal[j].length == k) {
				fwrite(&j,length_value,1,outputfile);
				for (k = 0; k < nsymbols*2-codefinal[j].length; k++)
					binarybuffer[k] = binarybuffer[k+codefinal[j].length];

				binarybuffersize -= codefinal[j].length;
				break;
			}
		}
	}
	free(bufferheader);
	fclose(inputfile);
	fclose(outputfile);

return 0;
}

int main(int argc, char **argv)
{
	int decode_flag = 0;
	int encode_flag = 0;
	int help_flag = 0;
	int non_option_flag = 0;
	int length_value = 1;
	char *input_file_value = NULL;
	char *output_file_value = NULL;
	int i, c;

	opterr = 0;

	while ((c = getopt (argc, argv, "edhl:i:o:")) != -1) {
		switch (c) {
			case 'e':
				encode_flag = 1;
				break;
			case 'd':
				decode_flag = 1;
				break;
			case 'h':
				help_flag = 1;
				break;
			case 'l':
				length_value = *optarg - 48;
				printf("Length = %i\n",length_value);
				break;
			case 'i':
				input_file_value = optarg;
				break;
			case 'o':
				output_file_value = optarg;
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
		showHelp(argv[0]);
	else if (encode_flag || decode_flag) {

		if (!input_file_value)
			printf("You need to specify an input file\n");
		else if (!output_file_value)
			printf("You need to specify an output file\n");
		else if (length_value < 1)
			printf("You need to specify a positive length\n");
		else if (encode_flag)
			encode(input_file_value, output_file_value, length_value);
		else if (decode_flag)
			decode(input_file_value, output_file_value);
	}

	return 0;
}