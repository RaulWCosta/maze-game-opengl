run:
	cmake -S . -B build
	make -C ./build all
	./build/maze_game
 