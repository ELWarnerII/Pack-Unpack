#include <stdio.h>
#include <stdlib.h>
#include <string.h>



  // Allocate a string with a small, initial capacity.
  int capacity = 5;
  char *buffer = (char *)malloc( (capacity + 1) * sizeof(char));

  // Number of characters we're currently using.
  int len = 0;
  
  char next;
  // Read from standard input (probably redirected from a file) until we
  // reach end-of-file, storing all characters in buffer.
  // Keep enlarging the buffer as needed until it contains the
  // whole file from standard input.  This took me 9 lines of code.
  // ...
  while ( !feof(fp) ) {
    
    fscanf(fp, "%c", &next);
    
    if ( len >= capacity ) {
      
      capacity *= 2;
      char *newBuffer = (char *)malloc( (capacity + 1)* sizeof( char ) );

      memcpy( newBuffer, buffer, len * sizeof( char ) );

      free( buffer );
      buffer = newBuffer;
    }
    
    buffer[len] = next;
    printf("%d\n", (int)next);
    len++;
    
  }
  
  buffer[len] = '\0';
  
  // Print out the total size of the file, in characters.
  printf( "%d\n", len );

  // Print out the whole file, it's one big string.
  printf( "%s", buffer );

  // Don't leak memory or open files.
  free(buffer);

  return 0;
}
