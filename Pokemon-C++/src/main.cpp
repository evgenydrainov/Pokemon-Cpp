#include "Game.h"

int main(int argc, char* argv[]) {
	Game game_instance = {};
	game = &game_instance;

	game->Init();
	game->Run();
	game->Destroy();

	return 0;
}

#define STB_SPRINTF_IMPLEMENTATION
#include <stb_sprintf.h>
#undef STB_SPRINTF_IMPLEMENTATION
