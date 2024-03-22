#include "game.h"

static Game game_instance;

int main(int argc, char* argv[]) {
	game_instance = {};
	game = &game_instance;

	game->init();
	game->run();
	game->destroy();

	return 0;
}

/*
void* operator new(size_t size) {
	printf("new %zu\n", size);
	return malloc(size);
}

void* operator new[](size_t size) {
	printf("new[] %zu\n", size);
	return malloc(size);
}
//*/
