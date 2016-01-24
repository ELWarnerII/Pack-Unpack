/** 
 * Header file for wordlist. This contains declarations for structs, typedefs,
 * and functions used in the other project classes.
 *  
 * @file wordlist.h
 * @author Louis Warner & David Sturgill
*/


#ifndef _WORDLIST_H_
#define _WORDLIST_H_

#include <stdbool.h>

/** Maximum length of a word in wordlist. */
#define WORD_MAX 20

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

#endif

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
bool validChar( char ch );


/**
 * This function is responsible for building the word list. It reads words from a word file 
 * given as fname. Before reading all the words from the word file, it adds single-character 
 * words for each of the 98 valid characters. Finally, it sorts the word list lexicographically 
 * so that the index of each word is its code.
 *
 * @param char const *fname - file name for the word list file
 * @return Wordlist *list - a pointer to the word list
 */
WordList *readWordList( char const *fname );


/**
 * This function takes a string and compares it to the strings in the word list.
 * It utilizes binary search methods for efficient searching through the word list.
 * It returns the index code for the largest matching string found in the list.
 *
 * @param WordList *wordlist - pointer to the word list
 * @param char const *str - pointer to the string to be compared
 * @param int ind - index of the largest matching string in wordlist
 */
int bestCode( WordList *wordList, char const *str );


/**
 * This function frees the memory for the given wordList, including the dynamically allocated list of words inside and the wordList structure itself.
 * 
 * @param WordList *wordList - pointer to the word list to be freed
 */
void freeWordList( WordList *wordList );