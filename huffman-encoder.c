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


	

unsigned long powint (int base, int exponent) {
  int i;
  unsigned long result = 1;
  for (i = 0; i < exponent; i++)
    result = result*base; 
  return result;
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
      }
    }
  }
}

void bubbleSortR(unsigned long numbers[], unsigned long indexes[], int array_size)
{
  int i, j, temp;

  for (i = (array_size - 1); i > 0; i--)
  {
    for (j = 1; j <= i; j++)
    {
      if (numbers[j-1] > numbers[j])
      {
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

int showhelp () {
  printf("Huffman encoder (March 2012). Usage:\n"
  "huffman-encoder [-e/-d] [-i inputfile] [-o outpufile]\n"
  "  -e  encode\n"
  "  -d  decode\n"
  "  -d  lenght (1 if omitted)\n"
  "  -h  help\n"
  "Limitations: Max file size = 4GiB\n");
  return 0;
}

unsigned long getfilesize (char *filename) {
  struct stat st;
  stat(filename, &st);
  return st.st_size;
}

/*int fill (unsigned char value, unsigned char **code, unsigned long codelength[], unsigned int **children, unsigned int childrenlength[], unsigned long parent) {
  //printf("OK\n");
  //printf("%lu\n",parent);
  //printf("%lu\n",codelength[parent]);
  //printf("%u\n",code[6][0]);
  code[parent][codelength[parent]] = value;
  codelength[parent]++;
  unsigned int currentchildrenlength = childrenlength[parent];
  while ( currentchildrenlength > 0 ) {
    fill(value, code, codelength, children, childrenlength, children[parent][currentchildrenlength]);
    currentchildrenlength--;
  } 
}*/

int fill (unsigned char value, code codeh[], unsigned long parent) {
  //printf("OK\n");
  //printf("%lu\n",parent);
  //printf("%lu\n",codelength[parent]);
  //printf("%u\n",code[6][0]);
  codeh[parent].value[codeh[parent].length] = value;
  //printf("%lu - %u\n", parent, value);
  codeh[parent].length++;
  unsigned int currentchildrenlength = codeh[parent].children.length;
  while ( currentchildrenlength != 0 ) {
    fill(value, codeh, codeh[parent].children.value[currentchildrenlength-1]);
    currentchildrenlength--;
  } 
}

int encode (char *inputfilevalue, char *outputfilevalue, unsigned char lengthvalue) {
  unsigned long i;
  unsigned long j;
  printf("  Encoding\n");
  printf("-Opening file %s\n", inputfilevalue);
  
  FILE *inputfile = fopen(inputfilevalue,"rb");
  if (!inputfile){
    printf("File %s doesn't exist\n", inputfilevalue);
    return 2;
  }
  
  unsigned long filesize = getfilesize(inputfilevalue);
  printf("-File size: %lu bytes\n", filesize);
  
  unsigned long nsymbols = powint(2,8*lengthvalue);
  unsigned long currentsymbol = 0;
  
  //unsigned char buffer[lengthvalue];
  
  unsigned char *buffer;
  buffer = malloc(lengthvalue);
  
  printf("-Number of symbols: %lu\n",nsymbols);
  //unsigned long probs[nsymbols];
  unsigned long *probs;
  probs = malloc(powint(2,8*lengthvalue)*sizeof(unsigned long));
  for (i = 0; i < nsymbols; i++)
    probs[i] = 0;
    
  for (i = 0; i < filesize/lengthvalue; i++) {
    fread(buffer, lengthvalue, 1, inputfile);
    //printf("%4X ", *buffer);
    //printf("%d ", *buffer);
    currentsymbol = 0;
    for (j = 0; j < lengthvalue; j++){
      currentsymbol += buffer[j]*powint(2,8*(lengthvalue - 1 - j));
    }
    probs[currentsymbol]++;
    //printf("%.4X ", currentsymbol);
  }
  
  FILE *probsresult = fopen("probs.txt","w");
  for (i=0; i < nsymbols; i++) {
    //printf("%.2X : %lu\n", i, probs[i]);
    fprintf(probsresult, "%lu\n",probs[i]);
  }
  fclose(probsresult);
  

  //Valors de prova
  //nsymbols=8;
  //unsigned long probs2[] = {31, 30, 20, 10, 4, 3, 1, 1};
  //unsigned long probs2[] = {20, 30, 1, 31, 4, 10, 3, 1};
  //unsigned long probs2[] = {40,15,15,10,10,5,4,1};
  //for (i = 0; i < nsymbols; i++)
  //  probs[i] = probs2[i];
  
  unsigned long tree[(nsymbols - 1) * 2];
  unsigned long indexes[nsymbols];
  for (i = 0; i < nsymbols; i++)
    indexes[i] = i;
  
  unsigned long min1;
  unsigned long min2;
  unsigned int position1 = 0;
  unsigned int position2 = 0;
  /*for (i = 0; i < nsymbols; i++)
    printf("%i %lu\n", indexes[i], probs[i]);*/
  unsigned long totalsymbols = nsymbols;
  //unsigned long codelength[nsymbols];
  //for (i = 0; i < nsymbols; i++)
  //  codelength[i] = 0;
    
  //unsigned char code[nsymbols*2][lengthvalue*8];
  
  code codeh[nsymbols*2];
  for (i = 0; i < nsymbols; i++)
    codeh[i].length = 0;
  unsigned long order[nsymbols];
  for (i = 0; i < nsymbols; i++)
    order[i] = i;
  
  /*
  for (i = 0; i < nsymbols - 1; i++) { // nsymbols - 1
    bubbleSort(probs, indexes, totalsymbols);
    tree[i * 2] = indexes[totalsymbols-2]; //0
    code[indexes[totalsymbols-2]][codelength[indexes[totalsymbols-2]]] = 0;
    codelength[indexes[totalsymbols-2]]++;
    tree[(i * 2) + 1] = indexes[totalsymbols-1]; //1
    for (j = totalsymbols-1; j < nsymbols; j++){
      code[indexes[j]][codelength[indexes[j]]] = 1;
      codelength[indexes[j]]++;
    }
    probs[indexes[totalsymbols-2]] += probs[indexes[totalsymbols-1]];
    totalsymbols--;
  }
  */

  bubbleSort(probs, order, totalsymbols);
  
  /*for (i = 0; i < nsymbols; i++)
    printf("%lu - %lu, ", order[i], probs[i]);
  printf("XXX\n\n");*/
  
  unsigned long tmp;
  for (i = 0; i < nsymbols - 1; i++) { // nsymbols - 1
    bubbleSort(probs, indexes, totalsymbols);

    if ( indexes[totalsymbols-2] < indexes[totalsymbols-1] ) {
      tree[i * 2] = indexes[totalsymbols-2]; //0
      tree[(i * 2) + 1] = indexes[totalsymbols-1]; //1
    } else {
      //printf("Gira");
      tmp = probs[totalsymbols-2];
      probs[totalsymbols-2] = probs[totalsymbols-1];
      probs[totalsymbols-1] = tmp;
      
      tmp = indexes[totalsymbols-2];
      indexes[totalsymbols-2] = indexes[totalsymbols-1];
      indexes[totalsymbols-1] = tmp;
      
      tree[i * 2] = indexes[totalsymbols-2]; //0
      tree[(i * 2) + 1] = indexes[totalsymbols-1]; //1     
    } 
    /*for (j = 0; j < totalsymbols; j++) 
      printf("%lu-%lu ", probs[j], indexes[j]+1);*/
    //printf("(%u %u-%lu,%lu>%u)\n", tree[i*2]+1, tree[(i*2)+1]+1,probs[tree[i*2]],probs[tree[(i*2)+1]], totalsymbols);
    
    probs[totalsymbols-2] += probs[totalsymbols-1];

    totalsymbols--;
  }
    
  printf("\n");
  //unsigned int children[nsymbols][nsymbols];
  //unsigned int childrenlength[nsymbols];
  //for (i = 0; i < nsymbols; i++)
  //  childrenlength[i] = 0;
  for (i = 0; i < nsymbols; i++)
    codeh[i].children.length = 0;
  
  unsigned int parent;
  for (i = 0; i < (nsymbols - 1); i++) {
/*    code[tree[i * 2]][codelength[tree[i * 2]]] = 0;
    codelength[tree[i * 2]]++;*/
    parent = tree[i * 2];
    //printf("->%u\n",code[6][0]);
    fill(0, codeh, parent);
/*    if ( child[parent] != -1 ) {
      do {
        code[child[parent]][codelength[child[parent]]] = 0;
        codelength[child[parent]]++;
        parent = child[parent];
      } while ( parent != -1 );
    }*/
    
/*    code[tree[(i * 2) +1 ]][codelength[tree[(i * 2) +1 ]]] = 1;
    codelength[tree[(i * 2) +1 ]]++;*/
    parent = tree[(i * 2) +1 ];
    fill(1, codeh, parent);
/*    if ( child[parent] != -1 ) {
      do {
        code[child[parent]][codelength[child[parent]]] = 1;
        codelength[child[parent]]++;
        parent = child[parent];
      } while ( parent != -1 );
    }*/
    //children[tree[i * 2]][childrenlength[tree[i * 2]]] = tree[(i * 2) +1 ];
	codeh[tree[i * 2]].children.value[codeh[tree[i * 2]].children.length] = tree[(i * 2) + 1];
    //childrenlength[tree[i * 2]]++;
	codeh[tree[i * 2]].children.length++;
  }
  
  /*
  bubble Sort(probs, indexes, totalsymbols);
  
  min1 = probs[0];
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
  }*/
  
/*  
  printf("\n");
  for (i = 0; i < nsymbols; i++) {
    printf("%lu - ", order[i]);
    for (j = 0; j < codeh[i].length; j++)
      printf("%i",codeh[i].value[j]);
    printf("\n");
  }
  printf("-----\n");
*/
/*
  i = 32;
  for (j = 0; j < codeh[i].length; j++)
      printf("%i",codeh[i].value[j]);
  
  printf("\n-----\n");
*/  
/*  for (i = 0; i < nsymbols; i++) {
    printf("%lu - ",i);
    printf("%lu", codelength[i]);
    printf("\n");
  }*/
/*  
  for (i = 0; i < nsymbols; i++)
    indexes[i] = i;
  unsigned long codelengthcopy[nsymbols];
  for (i = 0; i < nsymbols; i++)
    codelengthcopy[i] = codeh[i].length;
*/  
  unsigned long ordercopy[nsymbols];
  for (i = 0; i < nsymbols; i++)
    ordercopy[i] = order[i];
  

  for (i = 0; i < nsymbols; i++)
    indexes[i] = i;
  bubbleSortR(ordercopy, indexes, nsymbols);
/*  
  for (i = 0; i < nsymbols; i++) {
    printf("%lu - ",order[i]);
    for (j = 0; j < codeh[indexes[i]].length; j++)
      printf("%i",codeh[indexes[i]].value[j]);
    printf("\n");
  }
  
  printf("\n-----\n");
  i = 32;
  for (j = 0; j < codeh[indexes[i]].length; j++)
      printf("%i",codeh[indexes[i]].value[j]);
  printf("\n-----\n");
*/  
  code codefinal[nsymbols];
  
  for (i = 0; i < nsymbols; i++)
    codefinal[i] = codeh[indexes[i]];
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
  outbuffer = malloc(lengthvalue);
  
  FILE *outputfile = fopen(outputfilevalue,"wb");
  if (!outputfile) {
    printf("Error with output file %s\n", outputfilevalue);
    return 3;
  }
  
  //Headers of file
  char filetype[] = ".huf1.0";
  for (i = 0; i < 7; i++)
    fwrite(&filetype[i] ,1 ,1 ,outputfile);
    
  fwrite(&lengthvalue ,1 ,1 ,outputfile);
  
  /*for (i = 0; i < nsymbols; i++)
    printf("%lu\n", order[i]);*/
  /*for (i = 0; i < (nsymbols - 1) * 2; i++)
    printf("%lu\n", tree[i]);*/
  
  for (i = 0; i < nsymbols; i++)
    fwrite(&order[i], sizeof(unsigned long) ,1 ,outputfile);
    
  for (i = 0; i < (nsymbols - 1) * 2; i++)
    fwrite(&tree[i], sizeof(unsigned long), 1, outputfile);
  
  
  for (i = 0; i < filesize/lengthvalue; i++) {
    fread(buffer, lengthvalue, 1, inputfile);
    currentsymbol = 0;
    for (j = 0; j < lengthvalue; j++){
      currentsymbol += buffer[j]*powint(2,8*(lengthvalue - 1 - j));
    }
    for (j = 0; j < codefinal[currentsymbol].length; j++)
      binarybuffer[binarybuffersize+j] = codefinal[currentsymbol].value[j];
    binarybuffersize += codefinal[currentsymbol].length;
    
    while (binarybuffersize > lengthvalue*8 - 1){
      for (j = 0; j < lengthvalue*8; j++)
        *outbuffer += binarybuffer[j] * powint(2,j);
      fwrite(outbuffer ,lengthvalue ,1 ,outputfile);
      
      for (j = 0; j < binarybuffersize - lengthvalue*8; j++)
        binarybuffer[j] = binarybuffer[j+lengthvalue*8];
      binarybuffersize -= lengthvalue*8;
    }
  }
  
  free(probs);
  free(buffer);
  fclose(inputfile);
  fclose(outputfile);    
  free(outbuffer);

  unsigned long filesizecomp = getfilesize(outputfilevalue);
  printf("-Compressed file size: %lu bytes\n", filesizecomp);
  printf("-Compression ratio: %f\n", (float) filesize / filesizecomp);
  
  return 0;
}

int decode (char *inputfilevalue, char *outpufilevalue) {
  unsigned long i;
  unsigned long j;
  printf("  Decoding\n");
  printf("-Opening file %s\n", inputfilevalue);
  
  FILE *inputfile = fopen(inputfilevalue,"rb");
  if (!inputfile){
    printf("File %s doesn't exist\n", inputfilevalue);
    return 2;
  }
  
  char filetype[] = ".huf1.0";
  
  unsigned char *bufferheader;
  bufferheader = malloc(7);
  unsigned char *buffer;
  //buffer = malloc()
  
  fread(bufferheader ,7 ,1 , inputfile);
  
  for (i = 0; i < 4; i++) {
    if (filetype[i] != bufferheader[i])
        break;
  }
  if (i != 4) {
    printf("File %s is not a valid huffman file\n", inputfilevalue);
    return 2;
  }
  printf("-Huffman file %s is version %c%c%c\n", inputfilevalue, bufferheader[4], bufferheader[5], bufferheader[6]);
  
  unsigned char lengthvalue;
  fread(&lengthvalue, 1, 1, inputfile);
  printf("-Length = %u\n", lengthvalue);
  
  unsigned long nsymbols = powint(2,8*lengthvalue);
  unsigned long order[nsymbols];
  unsigned long tree[(nsymbols - 1) * 2];
  
  printf("-Number of symbols: %lu\n", nsymbols);
  
  fread(order, nsymbols, sizeof(unsigned long), inputfile);
  fread(tree, (nsymbols - 1) * 2, sizeof(unsigned long), inputfile);
  
  /*for (i = 0; i < nsymbols; i++)
    printf("%lu\n", order[i]);*/
  /*for (i = 0; i < (nsymbols - 1) * 2; i++)
    printf("%lu\n", tree[i]);*/
    
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
 
  code codefinal[nsymbols];
  for (i = 0; i < nsymbols; i++)
    codefinal[i] = codeh[indexes[i]];
    
  /*for (i = 0; i < nsymbols; i++) {
    printf("%lu - ",i);
    for (j = 0; j < codefinal[i].length; j++)
      printf("%i",codefinal[i].value[j]);
  printf("\n");
  }*/
  
  /*
  for (i = 0; i < nsymbols; i++)
    fwrite(&order[i] ,1 ,1 ,outputfile);
    
  for (i = 0; i < (nsymbols - 1) * 2; i++)
    fwrite(&tree[i], 1, 1, outputfile);
    */
  
  free(bufferheader);
  fclose(inputfile);
  
  return 0;
}

int main (int argc, char **argv) {
  int decodeflag = 0;
  int encodeflag = 0;
  int helpflag = 0;
  int lengthvalue = 1;
  char *inputfilevalue = NULL;
  char *outputfilevalue = NULL;
  int index;
  int c;
  
  int nonoptionflag=0;
  opterr = 0;

  while ((c = getopt (argc, argv, "edhl:i:o:")) != -1)
    switch (c) {
      case 'e':
        encodeflag = 1;
        break;
      case 'd':
        decodeflag = 1;
        
        break;
      case 'h':
        helpflag = 1;
        break;
      case 'l':
        lengthvalue = *optarg - 48;
        printf("Length = %i\n",lengthvalue);
        break;
      case 'i':
        inputfilevalue = optarg;
        break;
      case 'o':
        outputfilevalue = optarg;
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

  for (index = optind; index < argc; index++) {
    if (!nonoptionflag)
      nonoptionflag = 1;
    printf("Non-option argument: %s\n", argv[index]);
  }

  if (nonoptionflag)
    return 1;
  else if (helpflag || (argc == 1))
    showhelp ();
  else if (encodeflag || decodeflag){
  
    if (!inputfilevalue)
      printf("You need to specify an input file\n");
    else if (!outputfilevalue)
      printf("You need to specify an output file\n");
    else if (lengthvalue < 1 || lengthvalue > 2)
      printf("You need to specify a positive length\n");
    else if (encodeflag)
      encode(inputfilevalue, outputfilevalue, lengthvalue);
    else if (decodeflag)
      decode(inputfilevalue, outputfilevalue);   

  }
  
  return 0;
}
