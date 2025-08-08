/*
 * FindPermutations.cpp
 *
 *  Created on: Jan 22, 2025
 *      Author: dad
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcgi_stdio.h>
#include <math.h>

#include "permute.h"

void swap(char *a, char *b) {
	char temp = *a;
	*a = *b;
	*b = temp;
}

void FindPermutations(char *str, int n, int r, int start, char *result) {
	if (r == 0) {
		// add the word to our list
		*(Wordlist + NumWordsInList) = (char*) malloc(strlen(result + 1));
		strcpy(*(Wordlist + NumWordsInList), result);
		NumWordsInList++;
		if (NumWordsInList > WordListSize) {
			perror("too many permutations found");
			FCGI_printf(
					"<h1>Program failed due to too many permutations found... this is a program bug!</h1>\n");
			exit(EXIT_FAILURE);
		}
		return;
	}

	for (int i = start; i < n; i++) {
		// Swap characters to generate new permutations
		swap((str + i), (str + start));
		result[r - 1] = str[start];
		FindPermutations(str, n, r - 1, start + 1, result);
		// Backtrack - swap characters back to original position
		swap((str + i), (str + start));
	}
}

// find the number of permutations P(n,m( = n!/(n-m)!)
int Pnm(int n, int m) {
	if (m > n)
		return (-1);
	if (n < 1 || m < 1)
		return (-2);
	if (n > 15)
		return (-3);
	return (tgamma(n + 1) / (tgamma(n - m + 1)));
}

