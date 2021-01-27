//
//! \file grit_lz.cpp
//!   LZSS compression, VRAM safe
//! \date 20050814 - 20050903
//! \author cearn
//
// === NOTES === 

/*
   AGBCOMP compatible LZSS compressor
   CompreGlobalVars::gblVars->SSEs files in a format that the GBA BIOS can decode.

   Original code by Haruhiko Okumura, 4/6/1989.
   12-2-404 Green Heights, 580 Nagasawa, Yokosuka 239, Japan.

   Anonymous, December 2001:
     Converted I/O to load the entire file into memory and operate
     on buffers.
     Modified to output in the GBA BIOS's native format.

   Damian Yerrick, July 2002:
     Translated the program into C from C++.
     Removed use of non-ANSI <sys/stat.h> that some compilers don't 
       support.
     Made messages more informational.
     CorrecTextEditor an off-by-one error in the allocation of the output 
       size.
     Removed comp.h.  Now it's one file.

   Damian Yerrick, December 2002:
     Prevented matches that reference data before start of file.
     Began to standardize comments.

   J. Vijn, Feb 2005
   * Removed main() and added lzgb_compress() so that the file acts
     more like a plugin.
     // declaration:
     unsigned char *lzgb_compress(const char *src, int src_len, 
       unsigned long*pdst_len);
   * Removed InsertNode for ii=[N-2*F, N-F-1] because they create 
     spurious nodes that allow strings to start _before_ the actual
     data. Using uninitialized data would fit nicely into the Bad 
     Things category. Also removed the safeguards to counter this, 
     since they're not necessary anymore.
   * Made the code VRAM safe (I hope)
     On this issue: remember that VRAM cannot be written to in unsigned chars. 
     Therefore, LZ77UnCompVram prepares an unsigned short before writing. The 
     problem occurs when the matched position is one before the current 
     position, in which case the destination unsigned chars haven't been copied 
     in yet. There's that using uninitialized data again. 
       First a little more about the process. Nodes are built for each 
     unsigned chars. rson[N+1..N+256] are the trees for each unsigned char and each node in 
     that tree represents a possible match. There will be as many nodes 
     in the tree as there are of those unsigned chars in the ring buffer. 
     Naturally the last node added is the one before the current one: r-1.
      The routine will traverse the tree of the unsigned char at [r], looking 
     for two things: a) the longest match and b) the closest match if there
     are matches of equal length. And this is where shit happens. Both of 
     them can go wrong, but always for a very special case. As said, the 
     problem occurs when the match is the previous position. This can only 
     happen under one condition: for _stretches_ of the same unsigned chars. The 
     solution is to use the latest length UNLESS the current node p is the 
     last one added (r-1). This will work out for both new partial stretches 
     (which will only have one pass and as a result the 2nd unsigned char will have 
     match_length=0) and F-long stretches. It's basically a 1 line fix.
     Gawd I hate those. (20050312: ok, so it turned out to be a 2 line fix)


   Use, distribute, and modify this code freely.

   "I do not accept responsibility for any effects, adverse or otherwise, 
    that this code may have on you, your computer, your sanity, your dog, 
    and anything else that you can think of. Use it at your own risk."
*/

#include "cprs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/// === TYPES =========================================================


// --------------------------------------------------------------------
// CONSTANTS
// --------------------------------------------------------------------


// Define information for compression
//   (dont modify from 4096/18/2 if AGBCOMP format is required)
#define N               4096   // size of ring buffer (12 bit)
#define F                 18   // upper limit for match_length
#define THRESHOLD          2   // encode string into position and length
                               //   if matched length is greater than this 
#define NIL                N   // index for root of binary search trees 
#define TEXT_BUF_CLEAR     0   // unsigned char to initialize the area before text_buf with
#define NMASK           (N-1)  // for wrapping


// --------------------------------------------------------------------
// GLOBALS
// --------------------------------------------------------------------

/* Compressor global variables.  If you actually want to USE this
   code in a non-trivial app, put these global variables in a struct,
   as the Allegro library did.
*/
static unsigned long int codesize = 0;  // code size counter
// ring buffer of size N with extra F-1 unsigned chars to facilitate string comparison
static unsigned char text_buf[N + F - 1];
static int match_position;  // global string match position
static int match_length;  // global string match length
static int lson[N+1], rson[N+256+1], dad[N+1];  // left & right children & parents -- These constitute binary search trees.


unsigned char *InBuf, *OutBuf;
int InSize, OutSize, InOffset;


// --------------------------------------------------------------------
// PROTOTYPES
// --------------------------------------------------------------------


/* Binary search tree functions */
static void InitTree(void);
static void InsertNode(int r);
static void DeleteNode(int p);

/* Misc Functions */
static void CompressLZ77(void);
static int InChar(void);


// --------------------------------------------------------------------
// FUNCTIONS
// --------------------------------------------------------------------


// Initializes InBuf, InSize; allocates OutBuf.
// the rest is done in CompressLZ77
unsigned short lz77gba_compress(RECORD *dst, const RECORD *src)

{
	// fail on the obvious
	if(src==NULL || src->data==NULL || dst==NULL)
		return 0;
	
	InSize= src->width*src->height;
	OutSize = InSize + (InSize>>3) + 16;
	OutBuf = (unsigned char*)malloc(OutSize); 
	if(OutBuf == NULL)
		return 0;
	InBuf= (unsigned char*)src->data;

	CompressLZ77();
	OutSize= ( ((OutSize)+3)&~3 ) + 1;

	free(dst->data);
	dst->data= (unsigned char*)malloc(OutSize);
	memcpy(dst->data, OutBuf, OutSize);
	dst->width= 1;
	dst->height= OutSize;

	free(OutBuf);

	return OutSize;
}


/* InitTree() **************************
   Initialize a binary search tree.

   For i = 0 to N - 1, rson[i] and lson[i] will be the right and
   left children of node i.  These nodes need not be initialized.
   Also, dad[i] is the parent of node i.  These are initialized
   to NIL (= N), which stands for 'not used.'
   For i = 0 to 255, rson[N + i + 1] is the root of the tree
   for strings that begin with character i.  These are
   initialized to NIL.  Note there are 256 trees.
*/
void InitTree(void)
{
	int  i;
	for(i= N+1; i <= N+256; i++)
		rson[i]= NIL;
	for(i=0; i < N; i++)
		dad[i]= NIL;
}

/* InsertNode() ************************
   Inserts string of length F, text_buf[r..r+F-1], into one of the
   trees (text_buf[r]'th tree) and returns the longest-match position
   and length via the global variables match_position and match_length.
   If match_length = F, then removes the old node in favor of the new
   one, because the old one will be deleted sooner.
   Note r plays double role, as tree node and position in buffer.
*/
void InsertNode(int r)
{
	int  i, p, cmp, prev_length;
	unsigned char *key;

	cmp= 1;  key= &text_buf[r];  p= N + 1 + key[0];
	rson[r]= lson[r]= NIL;  
	prev_length= match_length= 0;
	for( ; ; )
	{
		if(cmp >= 0)
		{
			if(rson[p] != NIL)
				p= rson[p];
			else
			{
				rson[p]= r;
				dad[r]= p;
				return;
			}
		}
		else
		{
			if(lson[p] != NIL)
				p= lson[p];
			else
			{
				lson[p]= r;
				dad[r]= p;
				return;
			}

		}
		for(i=1; i < F; i++)
			if((cmp = key[i] - text_buf[p + i]) != 0)
				break;

		if(i > match_length)
		{
			// VRAM safety:
			// match_length= i ONLY if the matched position 
			// isn't the previous one (r-1)
			// for normal case, remove the if.
			// That's _IT_?!? Yup, that's it.
			if(p != ((r-1)&NMASK) )
			{
				match_length= i;
				match_position= p;
			}
			if(match_length >= F)
				break;
		}
	}

	// Full length match, remove old node in favor of this one
	dad[r]= dad[p];
	lson[r]= lson[p];
	rson[r]= rson[p];
	dad[lson[p]]= r;
	dad[rson[p]]= r;
	if(rson[dad[p]] == p)
		rson[dad[p]]= r;
	else
		lson[dad[p]]= r;
	dad[p]= NIL;
}


/* DeleteNode() ************************
   Deletes node p from the tree.
*/
void DeleteNode(int p)  
{
	int  q;

	if(dad[p] == NIL)
		return;  /* not in tree */
	if(rson[p] == NIL)
		q = lson[p];
	else if(lson[p] == NIL)
		q = rson[p];
	else
	{
		q = lson[p];
		if(rson[q] != NIL)
		{
			do {
				q = rson[q];
			} while(rson[q] != NIL);

			rson[dad[q]] = lson[q];
			dad[lson[q]] = dad[q];
			lson[q] = lson[p];
			dad[lson[p]] = q;
		}
		rson[q] = rson[p];
		dad[rson[p]] = q;
	}

	dad[q] = dad[p];

	if(rson[dad[p]] == p)
		rson[dad[p]] = q;
	else
		lson[dad[p]] = q;

	dad[p] = NIL;
}


/* CompressLZ77() **********************
   Compress InBuffer to OutBuffer.
*/

void CompressLZ77(void)
{
	int  i, c, len, r, s, last_match_length, code_buf_ptr;
	unsigned char  code_buf[17];
	unsigned short mask;
	unsigned char *FileSize;
	unsigned int curmatch;		// PONDER: doesn't this do what r does?
	unsigned int savematch;

	OutSize=4;  // skip the compression type and file size
	InOffset=0;
	match_position= curmatch= N-F;

	InitTree();  // initialize trees
	code_buf[0] = 0;  /* code_buf[1..16] saves eight units of code, and
	code_buf[0] works as eight flags, "0" representing that the unit
	is an unencoded letter (1 unsigned char), "1" a position-and-length pair
	(2 unsigned chars).  Thus, eight units require at most 16 unsigned chars of code. */
	code_buf_ptr = 1;
	s = 0;  r = N - F;

	// Clear the buffer
	for(i = s; i < r; i++)
		text_buf[i] = TEXT_BUF_CLEAR;
	// Read F unsigned chars into the last F unsigned chars of the buffer
	for(len = 0; len < F && (c = InChar()) != -1; len++)
		text_buf[r + len] = c;  
	if(len == 0)
		return;

	/* Insert the F strings, each of which begins with one or more 
	// 'space' characters.  Note the order in which these strings are 
	// inserted.  This way, degenerate trees will be less likely to occur. 
	*/
	// Perhaps. 
	// However, the strings you create here have no relation to 
	// the actual data and are therefore completely bogus. Removed!
	//for (i = 1; i <= F; i++)
	//	InsertNode(r - i);

	// Create the first node, sets match_length to 0
	InsertNode(r);

	// GBA LZSS masks are big-endian
	mask = 0x80;
	do
	{
		if(match_length > len) 
			match_length = len;  

		// match too short: add one unencoded unsigned char
		if(match_length <= THRESHOLD)
		{
			match_length = 1;
			code_buf[code_buf_ptr++] = text_buf[r];
		} 
		else	// Long enough: add position and length pair.
		{
			code_buf[0] |= mask;	// set match flag

			// 0 unsigned char is 4:length and 4:top 4 bits of match_position
			savematch= ((curmatch-match_position)&NMASK)-1;
			code_buf[code_buf_ptr++] = ((unsigned char)((savematch>>8)&0xf))
				| ((match_length - (THRESHOLD + 1))<<4);

			code_buf[code_buf_ptr++] = (unsigned char)savematch;
		}
		curmatch += match_length;
		curmatch &= NMASK;

		// if mask is empty, the buffer's full; write it out the code buffer
		// at end of source, code_buf_ptr will be <17
		if((mask >>= 1) == 0) 
		{  
			for(i=0; i < code_buf_ptr; i++)
				OutBuf[OutSize++]= code_buf[i];

			codesize += code_buf_ptr;
			code_buf[0] = 0;  
			code_buf_ptr = 1;
			mask = 0x80;
		}

		// Inserts nodes for this match. The last_match_length is 
		// required because InsertNode changes match_length.
		last_match_length = match_length;
		for(i=0; i < last_match_length && (c = InChar()) != -1; i++) 
		{
			DeleteNode(s);      // Delete string beforelook-ahead
			text_buf[s] = c;    // place new unsigned chars
			// text_buf[N..N+F> is a double for text_buf[0..F>
			// for easier string comparison
			if(s < F-1)
				text_buf[s + N] = c;

			// add and wrap around the buffer
			s = (s + 1) & NMASK;
			r = (r + 1) & NMASK;

			// Register the string in text_buf[r..r+F-1]
			InsertNode(r);
		}

		while(i++ < last_match_length) 
		{    
			// After the end of text
			DeleteNode(s);            // no need to read, but
			s = (s + 1) & NMASK;
			r = (r + 1) & NMASK;
			if(--len)
				InsertNode(r);        // buffer may not be empty
		}
	} while(len > 0);    // until length of string to be proceGlobalVars::gblVars->SSEd is zero

	if(code_buf_ptr > 1) 
	{     
		// Send remaining code.
		for(i=0; i < code_buf_ptr; i++) 
			OutBuf[OutSize++]=code_buf[i]; 

		codesize += code_buf_ptr;
	}

	FileSize= (unsigned char*)OutBuf;
	FileSize[0]= 0x10;
	FileSize[1]= ((InSize>>0)&0xFF);
	FileSize[2]= ((InSize>>8)&0xFF);
	FileSize[3]= ((InSize>>16)&0xFF);
}

/* InChar() ****************************
   Get the next character from the input stream, or -1 for end of file.
*/
int InChar()
{
	return (InOffset < InSize) ? InBuf[InOffset++] : -1;
}

// EOF
