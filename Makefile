BTree.exe: BTree.cpp
	g++ -g -fpermissive $< -o $@

.PHONY: run

run: BTree.exe
	valgrind ./BTree.exe 
