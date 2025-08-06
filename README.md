# mycheat
cgi program to generate a cheat sheet for word scramble games

compile using gcc -o mycheat.fcgi -lfcgi -lm -laspell -O3 -Wall -Wextra -pedantic -std=c11 mycheat.c FindPermutations.c 

Note that the code uses the fast-cgi and aspell libraries. To install the necessary headers and libraries on ubuntu, use:

sudo apt install libfcgi-dev libaspell-dev

The POST and cgi output is optimized for use with a particular cell phone. You may need to tweak font and widget sizes for your own preferences.

