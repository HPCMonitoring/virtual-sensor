compile:
	mkdir -p build && cd build && cmake .. && cmake --build . && cp ./vsensor ../vsensor

test_memory_leak:
	valgrind -s --tool=memcheck ./vsensor
clean_git:
	git branch --merged >/tmp/merged-branches && nano /tmp/merged-branches && xargs git branch -D </tmp/merged-branches && git fetch --prune --all

start_consumer:
	cd checktools && ./consumer getting-started.ini