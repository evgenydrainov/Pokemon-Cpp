#pragma once

#include "common.h"
#include "GameData.h"

#include <SDL.h>
#include <vector>

#define GAME_W 256
#define GAME_H 224

#define PLAYER_COUNT 2

using std::vector;

struct Pokemon {
	PokemonType type;
	float hp;
};

enum GameState {
	GAME_STATE_SELECT_ACTION,
	GAME_STATE_SELECT_POKEMON,
	GAME_STATE_WATCHING_ATTACK_ANIM,
	GAME_STATE_WON,
};

struct Player {
	vector<Pokemon> pokemon;
	int pokemon_index = -1;
};

struct Game;
extern Game* game;

struct Game {
	Player players[PLAYER_COUNT];
	Player players_visual[PLAYER_COUNT];

	GameState state;
	int player_index;
	int temp_player_index = -1;
	int cursor;
	int submenu = -1;
	int submenu_cursor;
	int animation_timer;

	u32 font;

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* game_texture;

	u32 key_pressed[((SDL_SCANCODE_UP + 1) + 31) / 32];
	bool skip_draw;

	void Init();
	void Destroy();
	void Run();

	void Update(float delta);
	void PerformAttack();
	void Draw(float delta);
	void DrawUI(int ui_y);

	void SetState(GameState _state, int _player_index) {
		state = _state;
		player_index = _player_index;
		cursor = 0;
		submenu = -1;
		submenu_cursor = 0;
		skip_draw = true;
		animation_timer = 0;
	}
};
