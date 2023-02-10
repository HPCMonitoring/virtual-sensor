compile:
	mkdir -p build && cd build && cmake .. && cmake --build . && cp ./vsensor ../vsensor

check_memory_leak:
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes ./vsensor > result.txt