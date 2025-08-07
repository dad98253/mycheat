// Compile with: gcc mycheat.c -o mycheat.fcgi -lfcgi -lm -laspell -O3 -Wall -Wextra -pedantic -std=c11
#include <fcgi_stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aspell.h>
#define IN_MAIN
#include "permute.h"

#define BUFFER_SIZE 100000
#define COMMAND_SIZE 1000

int CompareString(const void *str1, const void *str2);
void SortWords(char* lines[], int count);
int DedupeWords(char* lines[], long unsigned int* count);

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
						FCGI_printf("<h1>Letters = %s</h1>\n", letters);
						if ((strlen(letters) < 3) || (strlen(letters) > 8)) {
							perror("Bad input string");
							FCGI_printf(
									"<h1>Bad input: the number of letters must be between 3 and 8</h1>\n");
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
						for (long unsigned int m = 0; m < NumWordsInList; m++) {
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
	return EXIT_SUCCESS;
}

int CompareString(const void *str1, const void *str2) {
	char * const *pp1 = str1;
	char * const *pp2 = str2;
	return strcmp(*pp1, *pp2);
}

void SortWords(char* lines[], int count) {
	qsort(lines, count, sizeof(*lines), CompareString);
}

int DedupeWords(char* lines[], long unsigned int* count) {
	if (*count < 2)
		return (*count);
	for (long unsigned int i = 0; i < (*count - 1); i++) {
		while ( (i < (*count - 1)) && (strcmp(lines[i], lines[i + 1]) == 0) ) {
			// a dupe is found, delete it, move everyone up one, and decrement count
			free(lines[i + 1]);
			if (i < (*count + 1)) { // check for last word in list
				for (long unsigned int j = i + 1; j < (*count); j++) {
					lines[j] = lines[j + 1];
				}
				(*count)--;
			}
		}
	}
	return (*count);
}
