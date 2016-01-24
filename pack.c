/** 
 * This program takes a plaintext file and compresses it into
 * a smaller file. It takes two command line arguments, the first
 * is the file to be compressed, and the second is the output file
 * that will be written to.
 *  
 * @file pack.c
 * @author Louis Warner & David Sturgill
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "wordlist.h"
#include "bits.h"


/**
 * This function uses a resizable array to read the entire contents
 * of the given file into a dynamically allocated string. It will return a pointer
 * to a dynamically allocated, null terminated string containing the entire contents 
 * of the file. The caller is responsible for freeing this string.
 *
 * @param FILE *fp - pointer to the input file
 * @return char* buffer - string containing the entire contents of the input file
 */
 char *readFile( FILE *fp )
{
  // Allocate a string with a small, initial capacity.
  int capacity = 5;
  char *buffer = (char *)malloc( (capacity + 1) * sizeof(char)); 
  unsigned char next;
  int len = 0;
  next = fgetc(fp);
  
  // Read from file until we reach end-of-file, storing all characters in buffer.
  // Keep enlarging the buffer as needed until it contains the whole file.
  while ( !feof(fp) ) {
    
    if ( len >= capacity ) {
      capacity *= 2;
      char *newBuffer = (char *)malloc( (capacity + 1)* sizeof( char ) );
      memcpy( newBuffer, buffer, len * sizeof( char ) );
      free( buffer );
      buffer = newBuffer;
    }
    if(validChar(next)){
      buffer[len] = next;
      len++;
    } else {
      fprintf(stderr, "Invalid character code: %X\n", next);
      exit( EXIT_FAILURE );
    }
    buffer[len] = '\0';
    next = fgetc(fp);
  }
  
  return buffer;
}


/**
 * This is the main function for pack.c, it takes either 2 or 3 command line arguments.
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
      fprintf(stderr, "usage: pack <input.txt> <compressed.raw> [word_file.txt]\n");
      exit( EXIT_FAILURE );
  }
  
  // If the user provides a wordfile, replace the default wordfile.
  if (argc == 4)
  {
    wordFile = argv[ 3 ];
  }
  
  WordList *wordList = readWordList( wordFile );

#ifdef DEBUG
  // Report the entire contents of the word list, once it's built.
  printf( "---- word list -----\n" );
  for ( int i = 0; i < wordList->len; i++ )
    printf( "%d == %s\n", i, wordList->words[ i ] );
  printf( "--------------------\n" );
#endif

  // Check for valid input and output files.
  if((input = fopen( argv[ 1 ], "r" ) ) == NULL ) 
  {
    fprintf(stderr, "Can't open file: %s\n", argv[ 1 ]);
    fprintf(stderr, "usage: pack <input.txt> <compressed.raw> [word_file.txt]\n");
    exit( EXIT_FAILURE );
  }
  if((output = fopen( argv[ 2 ], "w" ))  == NULL )
  {
    fprintf(stderr, "Can't open file: %s\n", argv[ 2 ]);
    fprintf(stderr, "usage: pack <input.txt> <compressed.raw> [word_file.txt]\n");
    exit( EXIT_FAILURE );
  }

  // Read the contents of the whole file into one big buffer.  This could be more
  // efficient, but it simplifies the rest of the program.
  char *buffer = readFile( input );

  // Write out codes for everything in the buffer.
  int pos = 0;
  PendingBits pending = { 0, 0 };
  while ( buffer[ pos ] ) {
    // Get the next code.
    int code = bestCode( wordList, buffer + pos );
#ifdef DEBUG
    printf( "%d <- %s\n", code, wordList->words[ code ] );
#endif
    // Write it out and move ahead by the number of characters we just encoded.
    writeCode( code, &pending, output );
    pos += strlen( wordList->words[ code ] );
  }

  // Write out any remaining bits in the last, partial byte.
  flushBits( &pending, output );

  //Free remaining allocated memory and close the input and output files.
  freeWordList(wordList);
  free(buffer);
  fclose(input);
  fclose(output);

  return EXIT_SUCCESS;
}
