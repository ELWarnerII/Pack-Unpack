/** 
 * This file contains helper methods for pack.c and unpack.c
 * It is primarily responsible for reading and writing sequences of bits.
 *  
 * @file bits.c
 * @author Louis Warner
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/** Number of bits per byte.  This isn't going to change, but it lets us give
    a good explanation instead of just the literal value, 8. */
#define BITS_PER_BYTE 8

/** Number of bits in each code written to or read from a file. */
#define BITS_PER_CODE 9

/** Buffer space for up to 8 bits that we're not finished processing.
    We have to read/write files one or more bytes at a time, but we
    need to access this data 9 bits at a time.  While writing a file,
    this buffer temporarily holds bits waiting to be written to the
    file (waiting for a full group of 8), or, while reading, bits
    we've already read but that the client code doesn't need yet. */
typedef struct {
  /** Storage for the unprocessed bits. */
  unsigned char bits;

  /** Number of unprocessed bits stored in this buffer. */
  int bitCount;
} PendingBits;


/** Write the 9 low-order bits from code to the given file.  
    @param code bits to write out, a value betteen 0 and 2^9 - 1.
    @param pending pointer to storage for unwritten bits left over
    from the previous call to writeCode().  After this call, any bits
    that partially fill the next byte will be left in the pending
    struct, to be written in the next call.
    @param fp file we're writing to, opened for writing.
*/
void writeCode( int code, PendingBits *pending, FILE *fp )
{
  int tempcode = 0;
  int bitsRemaining = BITS_PER_CODE;
  
  tempcode = code << pending->bitCount;
  pending->bits = pending->bits + tempcode;
  tempcode = code >> (BITS_PER_BYTE - pending->bitCount);
  bitsRemaining = pending->bitCount + 1;
  
  fputc(pending->bits, fp);
 
  pending->bits = tempcode; 
  pending->bitCount = bitsRemaining;
  
  if(pending->bitCount == BITS_PER_BYTE){
    
    fputc(pending->bits, fp);
    pending->bits = 0;
    pending->bitCount = 0;
  }
  
}


/** If there are any bits buffered in pending, write them out to the
    given file in the low-order bit positions of a byte, leaving zeros
    in the high-order bits.
    @param pending pointer to storage for unwritten bits left over
    from the most recent call to writeCode().
    @param fp file these bits are to be written to, opened for writing.
*/
void flushBits( PendingBits *pending, FILE *fp )
{
  int tempcode = 0;
  pending->bits = pending->bits + tempcode;
  
  if(pending->bitCount > 0){
    fputc(pending->bits, fp);
    pending->bits = 0;
  }
} 


/** Read and return the next 9-bit code from the given file.
    @param pending pointer to storage for left-over bits read during
    the last call to readCode().
    @param fp file bits are being read from, opened for reading.
    @return value of the 9-bit code read in, or -1 if we reach the
    end-of-file before getting 9 bits.
*/
int readCode( PendingBits *pending, FILE *fp )
{
  unsigned char temp = fgetc(fp);
  if(feof(fp)){
    return -1;
  }
  
  unsigned char maybe = ' ';
  int code = 0;
  
  if(pending->bitCount == 0){
    maybe = fgetc(fp);
    if(feof(fp)){
      return -1;
    }
    pending->bits = maybe;
    maybe = maybe << (BITS_PER_BYTE - 1);
    code += maybe;
    code = code << 1;
    code += temp;
    pending->bitCount = BITS_PER_BYTE - 1;
    pending->bits = pending->bits >> 1;

    return code;
  }
  
  maybe = pending->bits;
  pending->bits = temp >> (BITS_PER_CODE - pending->bitCount);
  temp = temp << (pending->bitCount - 1);
  code += temp;
  code = code << 1;
  code += maybe;
  pending->bitCount--;
  
  return code;
}