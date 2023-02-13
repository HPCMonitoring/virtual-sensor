compile:
	mkdir -p build && cd build && cmake .. && cmake --build . && cp ./vsensor ../vsensor

test_memory_leak:
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes ./vsensor
clean_git:
	git branch --merged >/tmp/merged-branches && nano /tmp/merged-branches && xargs git branch -D </tmp/merged-branches && git fetch --prune --all