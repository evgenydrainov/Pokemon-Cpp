#pragma once

#include "common.h"
#include "GameData.h"

#include <minicoro.h>
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

	mco_coro* coroutine;

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

	void Coroutine(mco_coro* co);
	int CoroutineSelectPokemon(mco_coro* co, int player_index);
	int CoroutineSelectAction(mco_coro* co, int player_index);

	void SetState(GameState state, int player_index) {
		// next_state = state;
		// next_player_index = player_index;

		this->state = state;
		this->player_index = player_index;
		this->cursor = 0;
		this->submenu = -1;
		this->submenu_cursor = 0;
		this->skip_draw = true;
		this->animation_timer = 0;
	}
};
