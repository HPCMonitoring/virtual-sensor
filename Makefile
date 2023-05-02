all:
	mkdir -p build && cd build && cmake .. && cmake --build . && cp ./vsensor ../vsensor
test_memory_leak:
	sudo valgrind -s --tool=memcheck --leak-check=full --show-leak-kinds=all ./vsensor
clean_git:
	git branch --merged >/tmp/merged-branches && nano /tmp/merged-branches && xargs git branch -D </tmp/merged-branches && git fetch --prune --all

start_consumer:
	cd checktools && ./consumer config.ini