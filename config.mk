# version
NAME= status4dwm
VERSION=0.1

# paths
PREFIX= /usr/local
MANPREFIX= ${PREFIX}/share/man
SRCDIR=src
BINDIR=bin

# includes and libs
CLIBS =  -lm

# flags
CFLAGS   = -std=gnu23 -pedantic -Wall -Os

# compiler and linker
CC= cc 
VERBOSE= TRUE
