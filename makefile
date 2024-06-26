# Makefile to (1) compile and (2) clean
#  - Rule for compilation should create solution.exe
#     - Should only recompile code when any source code changed
#  - Rule for clean should remove solution.exe
#     - Should not show error if the file does not exist
#  - Use GCC compiler toolchain
#  - Specify the following options:
#     - Disable optimization
#     - Disable generation of debug information
#     - Enable all general warnings
#     - Enable conversion warnings

all: compile

CFLAGS = -s -Wall -Wconversion -O0

compile:
	@gcc $(CFLAGS) -o solution_v3 solution_v3.c
	solution_v3

clean:
	@del -f *.exe