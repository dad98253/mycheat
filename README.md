# mycheat
cgi program to generate a cheat sheet for word scramble games

compile using gcc mycheat.c -o mycheat.fcgi -lfcgi -O3 -Wall -Wextra -pedantic -std=c11

Note that the code uses the fast-cgi library. To install the necessary headers on ubuntu, use:

sudo apt install libfcgi-dev

It also depends on the permute command. It is available here:

https://github.com/dad98253/permute

The POST and cgi output is optimized for use with a particular cell phone. You may need to tweak font and widget sizes for your own preferences.

This version uses a very primitive approach to performing spell checking and will have a noticeably slow response time on older server hardware. In my case, I have limited the number of characters that can be input to a maximum of 8. If you have a nice fast server, you can likely increase this.
