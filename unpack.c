/** 
 * This program does the reverse operation of pack.c.
 * It takes two command line arguments. The first is the compressed
 * file that needs to be decompressed, and the second is the plaintext
 * file that the output should be written to.  
 *  
 * @file unpack.c
 * @author Louis Warner
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "wordlist.h"
#include "bits.h"

/**
 * This is the main function for unpack.c, it takes either 2 or 3 command line arguments.
 * If it is given only two arguments, it will use the default word list in the file "words.txt".
 * A third argument will switch the word list to whatever the user specified file is.
 */
int main( int argc, char *argv[] )
{
  char *wordFile = "words.txt";

  // Check command-line arguments and open the input file.
  FILE *input;
  FILE *output;
  
  if ( argc != 3 && argc != 4 )
  {
      fprintf(stderr, "usage: unpack <compressed.raw> <output.txt> [word_file.txt]\n");
      exit( EXIT_FAILURE );
  }
  
  // If the user provides a wordfile, replace the default wordfile.
  if (argc == 4)
  {
    wordFile = argv[ 3 ];
  }
  
  WordList *wordList = readWordList( wordFile );
  
  // Check for valid input and output files.
  if((input = fopen( argv[ 1 ], "r" ) ) == NULL ) 
  {
    fprintf(stderr, "Can't open file: %s\n", argv[ 1 ]);
    fprintf(stderr, "usage: unpack <compressed.raw> <output.txt> [word_file.txt]\n");
    exit( EXIT_FAILURE );
  }
  if((output = fopen( argv[ 2 ], "w" ))  == NULL )
  {
    fprintf(stderr, "Can't open file: %s\n", argv[ 2 ]);
    fprintf(stderr, "usage: unpack <compressed.raw> <output.txt> [word_file.txt]\n");
    exit( EXIT_FAILURE );
  }
  
  // Initialize PendingBits struct and integer for the word code.
  PendingBits pending = { 0, 0 };
  int code = 0;
  
  while(!feof(input)){
    code = readCode( &pending, input );
    if(code != -1){
      fprintf(output, "%s", wordList->words[code] );
    }
  }
  
  // Free any allocated memory and close files.
  freeWordList(wordList);
  fclose(input);
  fclose(output);

  return EXIT_SUCCESS;
}