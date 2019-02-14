BTree.exe: BTree.cpp
	g++ -g -fpermissive $< -o $@

.PHONY: run

# valgrind needed to catch memory errors
run: BTree.exe
	valgrind ./BTree.exe 
