#pragma once

#include "common.h"
#include "drawing.h"
#include "game_data.h"

#include <SDL.h>
#include <minicoro.h>
#include <vector>

#define GAME_W 320
#define GAME_H 240

#define MAX_PLAYERS 2

struct Font;
struct Game;

extern Game* game;

struct Pokemon {
	PokemonID id;
	float hp;
	float energy;
};

struct PokemonVisual {
	PokemonID id;
	float hp = -1;
	float energy = -1;
	float hp_target;
	float energy_target;
	int x;
	int y;
	bool front;
	bool visible;
};

struct Player {
	std::vector<Pokemon> pokemon;
	int pokemon_index = -1;
};

enum {
	DIALOG_HIDE_PANEL = 1,
	DIALOG_ALLOW_CANCEL = 1 << 1,
	DIALOG_SPRITE_LOOP = 1 << 2,
	DIALOG_SPRITE_DONT_DESTROY_WHEN_OVER = 1 << 3,

	DIALOG_FLAGS_DEFAULT = 0,
};

enum {
	DIALOG_TYPE_SELECTION,
	DIALOG_TYPE_TELETYPE,
	DIALOG_TYPE_SPRITE,
};

struct Dialog {
	u32 type;
	u32 id;
	u32 flags;

	int x;
	int y;
	int w;
	int h;

	union {
		struct {
			const char* label;
			const char** items;
			int item_count;
			int cursor;
			u32 disabled_items;
		} selection;
		struct {
			const char* text;
			int count;
		} teletype;
		struct {
			SDL_Texture* texture;
			int frame_count;
			float anim_spd;
			float frame_index;
		} sprite;
	};
};

struct Game {
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* game_texture;

	u32 key_pressed[((SDL_SCANCODE_UP + 1) + 31) / 32];
	mco_coro* co;
	DrawList draw_list;

	const Font* font;
	SDL_Texture* nineslice_texture;
	int main_panel_x;
	int main_panel_y;
	int main_panel_w;
	int main_panel_h;
	int slice_w;
	int slice_h;

	Player players[MAX_PLAYERS];

	std::vector<Dialog> dialogs;
	u32 next_dialog_id;

	int player_index_visual;
	PokemonVisual pokemon_visual[MAX_PLAYERS];
	bool update_pokemon_visual = true;

	void init();
	void run();
	void destroy();

	void update(float delta);
	void draw(float delta);

	Dialog* get_dialog(u32 dialog_id);
	void destroy_dialog(u32 dialog_id);

	void battle_script(mco_coro* co);

	u32 create_select_action_dialog(mco_coro* co, int player_index);

	int player_select_action(mco_coro* co, int player_index,
							 int* out_attack_index, int* out_pokemon_index);

	int player_select_pokemon(mco_coro* co, int player_index,
							  int x, int y,
							  u32 flags);

	u32 create_selection_dialog(const char* label,
								const char* items[], int item_count,
								int x, int y, int w = 0, int h = 0,
								u32 flags = DIALOG_FLAGS_DEFAULT,
								u32 disabled_items = 0);

	u32 create_teletype_dialog(const char* text,
							   int x, int y, int w = 0, int h = 0,
							   u32 flags = DIALOG_FLAGS_DEFAULT);

	u32 create_sprite(SDL_Texture* texture,
					  int x, int y, int w, int h,
					  int frame_count, float anim_spd,
					  u32 flags = DIALOG_FLAGS_DEFAULT);

	int selection_dialog(mco_coro* co, u32 dialog_id);

	int selection_dialog(mco_coro* co,
						 const char* label,
						 const char* items[], int item_count,
						 int x, int y, int w = 0, int h = 0,
						 u32 flags = DIALOG_FLAGS_DEFAULT,
						 u32 disabled_items = 0);

	bool is_key_pressed(int scancode) {
		bool result = false;
		if (0 <= scancode && scancode < (SDL_SCANCODE_UP + 1)) {
			result = key_pressed[scancode / 32] & (1 << (scancode % 32));
		}
		return result;
	}

};
