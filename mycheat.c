// Compile with: gcc mycheat.c -o mycheat.fcgi -lfcgi -O3 -Wall -Wextra -pedantic -std=c11
#include <fcgi_stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 100000
#define COMMAND_SIZE 1000

int main(void) {
	char *data;
	char letters[256]; // Buffer to store the extracted string of letters
	FILE *fp;
	char command[COMMAND_SIZE]; // Define COMMAND_SIZE appropriately
	// Construct your command string, e.g., sprintf(command, "ls -la %s", path);
	char buffer[BUFFER_SIZE]; // Buffer to read each line
	char *words[BUFFER_SIZE]; // pointer array to hold word strings
	char *output_string = NULL; // Dynamically allocated string to store total output
	size_t output_len = 0;

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

					// Parse the data (simple example for "letters=value")
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

						sprintf(command,
								"permute -m 3 %s 2>/dev/null |sort|uniq",
								letters);
						//                    sprintf(command, "permute -m 3 %s 2>/dev/null |sort|uniq>in.txt; aspell list < in.txt > out.txt; comm -23 in.txt out.txt 2>/dev/null; rm in.txt out.txt", letters);
						//                    sprintf(command, "whoami");
						fp = popen(command, "r"); // "r" for reading the command's output
						if (fp == NULL) {
							perror("Failed to run command");
							FCGI_printf("<h1>Failed to run command</h1>\n");
							exit(EXIT_FAILURE);
						} else {
							//                        FCGI_printf("<h1>COMMAND: %s</h1>\n", command);
						}
						int i = 0;
						while (fgets(buffer, sizeof(buffer), fp) != NULL) {
							size_t line_len = strlen(buffer);
							if (line_len > 1) {
								if (*(buffer + line_len - 1) == 0xa)
									*(buffer + line_len - 1) = 0;
							} // strip off the line feed
							// Reallocate output_string to accommodate new line
							//        output_string = realloc(output_string, output_len + line_len + 1);
							output_string = (char*) malloc(line_len + 1);
							if (output_string == NULL) {
								perror("Memory allocation failed");
								FCGI_printf(
										"<h1>Memory allocation failed</h1>\n");
								exit(EXIT_FAILURE);
							}
							//        strcpy(output_string + output_len, buffer); // Append the line
							strcpy(output_string, buffer); // Append the line
							output_len += line_len;
							words[i] = output_string;
							i++;
							//        FCGI_printf("<h1>read %i characters: \"%s\"</h1>\n", line_len, output_string);
						}
						pclose(fp);

						// Print the captured output
						if (i) {
							for (int j = 0; j < i; j++) {
								output_string = words[j];
								sprintf(command, "echo \"%s\" | aspell list",
										output_string);
								fp = popen(command, "r"); // "r" for reading the command's output
								if (fp == NULL) {
									perror("Failed to run command");
									FCGI_printf(
											"<h1>Failed to run %s command</h1>\n",
											command);
									exit(EXIT_FAILURE);
								} else {
									//                        FCGI_printf("<h1>COMMAND: %s</h1>\n", command);
								}

								/*while (fgets(buffer, sizeof(buffer), fp) != NULL) {
								 size_t line_len = strlen(buffer);
								 FCGI_printf("<h1>returned %li characters: %s</h1>\n", line_len, buffer);
								 }*/

								if (fgets(buffer, sizeof(buffer), fp) == NULL) {
									//                        FCGI_printf("<h1>%s is a word</h1>\n", output_string);
									FCGI_printf("<h1>%s</h1>\n", output_string);
								} else {
									//                        FCGI_printf("<h1>%s is a not word</h1>\n", output_string);
								}
								pclose(fp);

								//           FCGI_printf("<h1>%s</h1>", output_string);
								free(output_string); // Free allocated memory
							}
						} else {
							FCGI_printf("<h1>No output captured.</h1>\n");
						}

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

		//        FCGI_printf("<!doctype><html><body>Hola mundo from fastcgi with lighttpd!</body></html>\n");
	}
	return EXIT_SUCCESS;
}

