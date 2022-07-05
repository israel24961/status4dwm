# version
NAME= status4dwm
VERSION=0.1

# paths
PREFIX= /usr/local
MANPREFIX= ${PREFIX}/share/man
SRCDIR=src
BINDIR=bin

# includes and libs
CLIBS = -lcurl	\
				-I/usr/include/json-c -ljson-c
# flags
Os=-Os
CFLAGS   = -std=c17 -pedantic -Wall ${Os}

# compiler and linker
CC= cc -lm
VERBOSE= TRUE
