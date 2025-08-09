/*
 * permute.h
 *
 *  Created on: Jan 22, 2025
 *      Author: dad
 */

#ifndef PERMUTE_H_
#define PERMUTE_H_

#define MAXSETSIZE	15

#ifdef IN_MAIN
#define EXTERN
#define REXTERN		extern
#define INT2	=3
#define MXSZ	=MAXSETSIZE
#define PNULL   =NULL
#define INT0    =0
#else
#define EXTERN		extern
#define REXTERN
#define INT2
#define MXSZ
#define PNULL
#define INT0
#endif

#define MAXSETSIZE	15

EXTERN long unsigned int minWordSize INT2;
EXTERN long unsigned int maxWordSize MXSZ;
EXTERN char** Wordlist PNULL;
EXTERN long long unsigned int WordListSize INT0;
EXTERN long long unsigned int NumWordsInList INT0;

REXTERN void FindPermutations(char *str, int n, int r, int start, char *result);
REXTERN long long int Pnm(int n, int m);
REXTERN long long int factorial (int n);

#endif /* PERMUTE_H_ */
