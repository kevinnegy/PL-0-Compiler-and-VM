# Make sure the indented line is a tab and not a space filled indent
# Two options for code generation with makefile
# 1) In eustis type command "make" to compile and run with the all:... lines uncommented.  
# 2) Comment out the two all:... lines, type "make" in Eustis, then type "./compile input.txt output.txt"

# Optional compile lines: runs compile.exe with the input "input.txt" and creates the code generation file "output.txt"
# comment out next two lines to only create compile.exe

all: compile.exe vm.exe
#	./compile input.txt output.txt

compile.exe: lexer.c compile.c
	gcc lexer.c compile.c -o compile

vm.exe: vm.c
	gcc vm.c -o vm

clean:
	rm vm compile
