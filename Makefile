compile:
	mkdir -p build && cd build && cmake .. && cmake --build . && cp ./vsensor ../vsensor
