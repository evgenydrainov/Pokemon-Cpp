#include "Game.h"

int main(int argc, char* argv[]) {
	Game game_instance = {};
	game = &game_instance;

	game->Init();
	game->Run();
	game->Destroy();

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

#define STB_SPRINTF_IMPLEMENTATION
#include <stb_sprintf.h>
#undef STB_SPRINTF_IMPLEMENTATION
