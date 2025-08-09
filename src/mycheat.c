// Compile with: gcc mycheat.c -o mycheat.fcgi -lfcgi -lm -laspell -O3 -Wall -Wextra -pedantic -std=c11
#include <fcgi_stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aspell.h>
#include <ctype.h>
#define IN_MAIN
#include "permute.h"

#define BUFFER_SIZE 100000
#define COMMAND_SIZE 1000

int CompareString(const void *str1, const void *str2);
void SortWords(char* lines[], long long int count);
int DedupeWords(char* lines[], long long unsigned int* count);
void filter_alphabetic(char *str);
void toUppercase(char *str);
extern void OS_LibShutdown(void);

int main(void) {
	char *data;
	char letters[256]; // Buffer to store the extracted string of letters
	char * str2;
	char * str3;
	char * result;
	int n; // Take permutations of size n

	while (FCGI_Accept() >= 0) {
		FCGI_printf("Status: 200 OK\r\n");
		FCGI_printf("Content-type: text/html\r\n\r\n");
		FCGI_printf("<html><head><title>Sayang\'s cheat sheet</title>\n");
		FCGI_printf(
				"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
		FCGI_printf("<style>\r\n");
		FCGI_printf(" html {font-size: 18px;\n}\n");
		// Adjust base font size for screens up to 768px wide
		FCGI_printf(
				"@media screen and (max-width: 768px) {\n html {\n   font-size: 16px;\n   }\n}\r\n");
		// Further adjust base font size for screens up to 480px wide (e.g., mobile)
		FCGI_printf(
				"@media screen and (max-width: 480px) {\n html {\n   font-size: 14px;\n   }\n}\r\n");
		FCGI_printf(
				" body {font-family: Arial, sans-serif;\n font-size: 1rem;\n}\n");
		FCGI_printf(
				" h1 {font-size: 2.5rem;\nmargin-top: .05em;\nmargin-bottom: .05em;\n }\n");
		FCGI_printf(" p {font-size: 1.1rem;\n}\n");
		FCGI_printf("</style>\r\n");
		FCGI_printf("</head>\r\n");

		FCGI_printf("<body>\r\n");

		// Check if the request method is POST
		if (strcmp(getenv("REQUEST_METHOD"), "POST") == 0) {
			long content_length = 0;
			char *len_str = getenv("CONTENT_LENGTH");
			if (len_str) {
				content_length = atol(len_str);
			}

			if (content_length > 0) {
				data = (char *) malloc(content_length + 1);
				if (data) {
					fread(data, 1, content_length, stdin);
					data[content_length] = '\0'; // Null-terminate the string

					// Parse the data ("letters=value")
					char *name_start = strstr(data, "letters=");
					if (name_start) {
						name_start += strlen("letters=");
						char *amp_pos = strchr(name_start, '&');
						if (amp_pos) {
							strncpy(letters, name_start, amp_pos - name_start);
							letters[amp_pos - name_start] = '\0';
						} else {
							strcpy(letters, name_start);
						}
						// remove all non-alphabetic characters
						filter_alphabetic(letters);
						// put everything in upper case
						toUppercase(letters);
						FCGI_printf("<h1>Letters = %s</h1>\n", letters);
						if ((strlen(letters) < minWordSize) || (strlen(letters) > maxWordSize)) {
							FCGI_perror("Bad input string");
							FCGI_printf(
									"<h1>Bad input: the number of letters must be between %i and %i</h1>\n",minWordSize,maxWordSize);
							exit(EXIT_FAILURE);
						}
						str2 = letters;
						n = strlen(str2);
						str3 = (char*) malloc(n + 2);
						result = (char*) calloc(n + 2, 1);
						if (n < (int) maxWordSize)
							maxWordSize = (long unsigned int) n;

						// check for min size greater than max size
						if (minWordSize > maxWordSize) {
							FCGI_printf(
									"<h1> ///// ERROR : The minimum word size cannot be greater than the maximum word size.</h1>\n");
							exit(EXIT_FAILURE);
						}
						// find all permutations taken r at a time where r goes from minWordSize to maxWordSize
						// first determin how many to expect
						for (int m = (int)minWordSize; m <= n; m++) {
							WordListSize += Pnm(n, m);
						}
						Wordlist = (char**) malloc(
								sizeof(char*) * (WordListSize + 1));
						for (int r = (int) minWordSize;
								r < (int) (maxWordSize + 1); r++) {
							strcpy(str3, str2);
							FindPermutations(str3, n, r, 0, result);
						}
						// now, put the list in alphabetical order
						SortWords(Wordlist, NumWordsInList);
						// and eliminate dupes
						DedupeWords(Wordlist, &NumWordsInList);
						// now check the spelling of each word in the list. If it appears in the dictionary, print it out.
						// Create a config
						AspellConfig *config = new_aspell_config();
						// Create a speller using the config
						AspellCanHaveError *possible_err = new_aspell_speller(
								config);
						if (aspell_error_number(possible_err) != 0) {
							FCGI_printf("<h1>Aspell init error: %s</h1>\n",
									aspell_error_message(possible_err));
							delete_aspell_config(config);
							return EXIT_FAILURE;
						}
						// Cast to AspellSpeller after successful creation
						AspellSpeller *speller = to_aspell_speller(
								possible_err);
						for (long long unsigned int m = 0; m < NumWordsInList; m++) {
							// Check if the word is correctly spelled
							int correct = aspell_speller_check(speller,
									Wordlist[m], strlen(Wordlist[m]));
							if (correct) {
								FCGI_printf("<h1>%s</h1>\n", Wordlist[m]);
							}
							// we can free the words up while we go...
							free(Wordlist[m]);
						}
						// all done...
						delete_aspell_speller(speller);
						delete_aspell_config(config);
						free(str3);
						free(result);
						free(Wordlist);
					} else {
						FCGI_printf(
								"<h1>Letters not found in POST data.</h1>\n");
					}
					free(data);
				} else {
					FCGI_printf("<h1>Memory allocation error.</h1>\n");
				}
			} else {
				FCGI_printf("<h1>No POST data received.</h1>\n");
			}
		} else {
			FCGI_printf("<h1>This program expects a POST request.</h1>\n");
		}

		FCGI_printf("</body></html>\n");
	}
	FCGI_Finish();
	OS_LibShutdown();
	return EXIT_SUCCESS;
}

int CompareString(const void *str1, const void *str2) {
	char * const *pp1 = str1;
	char * const *pp2 = str2;
	return strcmp(*pp1, *pp2);
}

void SortWords(char* lines[], long long int count) {
	qsort(lines, count, sizeof(*lines), CompareString);
}

int DedupeWords(char* lines[], long long unsigned int* count) {
	if (*count < 2)
		return (*count);
	long long unsigned int* next;
	long long unsigned int i;
	next = (long long unsigned int*)malloc(sizeof(long long unsigned int) * (*count));
	for ( i = 0; i < (*count - 1); i++) next[i] = i + 1; // create a linked list so that we can save work garbage collecting
	next[*count -1] = 0;
	i = 0;
	while ( next[i] != 0 ) { // loop until we find the end of the list
		while ( (next[i] != 0) && (strcmp(lines[i], lines[next[i]]) == 0) ) { // check for the last element - we don't want to compare it
			// a dupe is found, delete it and move everyone up one
			free(lines[next[i]]);
			lines[next[i]] = NULL;
			next[i] = next[next[i]];
		}
		i = next[i]; // move on to the next (undeleted) element
	}
	i = 0; // we are all de-duped; now we need to reassemble the array
	long long unsigned int newcount = 1;
	while ( next[i] != 0 ) {
		lines[newcount] = lines[next[i]];
		i = next[i];
		newcount++;
	}
	free(next); // don't forget to free up the linked list that we created above
	*count = newcount;
	return (newcount);
}

void filter_alphabetic(char *str) {
    if (str == NULL) {
        return; // Handle null pointer input
    }

    int i, j;
    j = 0; // Index for the new string (containing only alphabetic characters)

    for (i = 0; str[i] != '\0'; i++) {
        if (isalpha((unsigned char)str[i])) { // Check if the character is an alphabet
            str[j] = str[i]; // Copy alphabetic character to the new position
            j++;
        }
    }
    str[j] = '\0'; // Null-terminate the modified string
}

void toUppercase(char *str) {
    int i = 0;
    if (str == NULL) {
        return; // Handle null pointer input
    }
    while (str[i] != '\0') {
        str[i] = toupper(str[i]);
        i++;
    }
}
