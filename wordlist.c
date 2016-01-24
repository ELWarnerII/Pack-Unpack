/** 
 * This file provides support for reading the 
 * word file and managing the word list.
 *  
 * @file wordlist.c
 * @author Louis Warner
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/** Maximum length of a word in wordlist. */
#define WORD_MAX 20
/** Minimum length of a word in wordlist. */
#define WORD_MIN 2
/** Minimum length of a word in wordlist. */
#define MAX_LISTLEN 512
/** Initial WordList capacity */
#define INITIAL_CAPACITY 5

/** Macros for valid characters */
#define TAB 9
#define NEWLINE 10
#define CARRIAGE 13
#define BOTTOM_RANGE 32

/** Number of ASCII values between from BOTTOM_RANGE and TOP_RANGE */
#define CYCLE 95


/** Word type, used to store elements of the word list,
    with room for a word of up to 20 characters. */
typedef char Word[ WORD_MAX + 1 ];

/** Representation for the whole wordlist.  It contains
    the list of words as a resizable, dynamically allocated
    array, along with supporting fields for resizing. */
typedef struct {
  /** Number of words in the wordlist. */
  int len;

  /** Capacity of the wordlist, so we can know when we need to resize. */
  int capacity;

  /** List of words.  Should be sorted lexicographically once the word list
      has been read in. */
  Word *words;
} WordList;


/**
 * Given a character, this function returns true if it's one of the 
 * 98 valid characters that can appear in a word of the wordlist or in the 
 * text file being compressed. This function is may be used internally while 
 * reading the word file, but it's also exposed in the header file so other 
 * components can use it.
 *
 * @param char ch
 * @return bool yesno
 */
bool validChar( char ch )
{
  bool yesno = false;
  if(ch == TAB || ch == NEWLINE || ch == CARRIAGE){
    yesno = true;
  }
  for(int i = 0; i < CYCLE; i++){
    if(ch == BOTTOM_RANGE + i){
      yesno = true;
    }
  }
  return yesno;
}


/**
 * Comparison function for sorting words with qsort. Puts the words in lexicographical order.
 *
 * @param const void *first - first word to be compared
 * @param const void *second - second word to be compared
 * @return 0 if equivalent, positive if first is greater, negative if first is lesser
 */
int compareWords(const void *first, const void *second)
{
  const char *ifirst = (const char *)first;
  const char *isecond = (const char *)second;
  return strcmp(ifirst, isecond);
}


/**
 * Function to add all of the 98 valid character types to the given word list as individual words.
 *
 * @param Wordlist list
 */
 void addValidChars(WordList *list){
   char temp[2] = {'\0'};
   temp[0] = TAB;
   strcpy(list->words[list->len], temp);
   list->len++;
   temp[0] = NEWLINE;
   strcpy(list->words[list->len], temp);
   list->len++;
   temp[0] = CARRIAGE;
   strcpy(list->words[list->len], temp);
   list->len++;
   for(int i = 0; i < CYCLE; i++){
     temp[0] = BOTTOM_RANGE + i;
     strcpy(list->words[list->len], temp);
     list->len++;
     if ( list->len >= list->capacity ) {
        list->capacity *= 2;
        Word *newList = (Word *)malloc( (list->capacity + 1)* sizeof( Word ) );
        memcpy( newList, list->words, list->len * sizeof( Word ) );
        free( list->words );
        list->words = newList;
      }
   }
 }


/**
 * This function is responsible for building the word list. It reads words from a word file 
 * given as fname. Before reading all the words from the word file, it adds single-character 
 * words for each of the 98 valid characters. Finally, it sorts the word list lexicographically 
 * so that the index of each word is its code.
 *
 * @param char const *fname - file name for the word list file
 * @return Wordlist *list - a pointer to the word list
 */
WordList *readWordList( char const *fname )
{
  FILE *fp;
  WordList *list = (WordList *) malloc( sizeof( WordList ) );
  if(( fp = fopen( fname, "r" )) == NULL ){
    fprintf(stderr, "Can't open word file: %s\n", fname);
    exit( EXIT_FAILURE );
  }
  
  list->capacity = INITIAL_CAPACITY;
  list->len = 0;
  list->words = (Word *)malloc( (list->capacity + 1) * sizeof(Word));
  
  // Set to negative value for EOF checking with dangling chars
  int wordLength = -1;
  char wordString[WORD_MAX + 1] = {'\0'};
  char temp = ' ';
  
  addValidChars(list);

  while ( !feof(fp) ) {
    // Get word length
    fscanf(fp, "%d", &wordLength);
    
    // If word length is still -1 at this point, the word file has characters left over (possibly just a new line) such that
    // EOF was not detected. If this is the case then break out of the while loop.
    if( wordLength == -1){
      break;
    }
    
    // Check whether length is within the allowed range for strings in the word list.
    if(wordLength <= WORD_MAX && wordLength >= WORD_MIN){    
     
      // Skip a single space
      getc(fp);
     
      // Get the word and check if each character is valid.
      for(int j = 0; j < wordLength; j++){       
        temp = getc(fp);
        if(validChar(temp)){
          wordString[j] = temp;
        } else {
          fprintf(stderr, "Invalid word file\n");
          exit( EXIT_FAILURE );
        }
      }
      
      // Check if enough memory is allocated, if not then resize the word list.
      if ( list->len >= list->capacity ) {
        list->capacity *= 2;
        Word *newList = (Word *)malloc( (list->capacity + 1)* sizeof( Word ) );
        memcpy( newList, list->words, list->len * sizeof( Word ) );
        free( list->words );
        list->words = newList;
      }
      
      // Copy the temporary string into a word in the word list.
      strcpy(list->words[list->len], wordString);
      list->len++;
      
      // If there are too many words in the list then print an error.
      if(list->len > MAX_LISTLEN){
        fprintf(stderr, "Invalid word file\n");
        exit(EXIT_FAILURE);
      }
    } else {
      fprintf(stderr, "Invalid word file\n");
      exit(EXIT_FAILURE);
    }
      
    // Reset temporary string and length variables.
    for(int k = 0; k < WORD_MAX; k++){
      wordString[k] = '\0';
    }
    wordLength = -1;    
  }
  
  //Sort the word list
  qsort(list->words, list->len, sizeof( Word ), compareWords) ;
  
  //Close the word file
  fclose(fp);
  
  return list;
} 


/**
 * This function takes a string and compares it to the strings in the word list.
 * It utilizes binary search methods for efficient searching through the word list.
 * It returns the index code for the largest matching string found in the list.
 *
 * @param WordList *wordlist - pointer to the word list
 * @param char const *str - pointer to the string to be compared
 * @param int ind - index of the largest matching string in wordlist
 */
int bestCode( WordList *wordList, char const *str )
{
  char copyString[WORD_MAX + 1] = {'\0'};
  Word *tempWord;
  
  for(int i = 0; i < WORD_MAX + 1; i++){
    strncpy(copyString, str, WORD_MAX + 1 - i);
    copyString[WORD_MAX + 1 - i] = '\0';
    tempWord = bsearch(copyString, wordList->words, wordList->len, sizeof( Word ), compareWords);
    if(tempWord != NULL){
      break;
    }
  }

  size_t index = tempWord - wordList->words;
  int ind = (int)index;

  return ind;
}


/**
 * This function frees the memory for the given wordList, including the dynamically allocated list of words inside and the wordList structure itself.
 * 
 * @param WordList *wordList - pointer to the word list to be freed
 */
void freeWordList( WordList *wordList )
{
  free( wordList->words );
  free( wordList );
}