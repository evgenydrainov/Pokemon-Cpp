#include "Game.h"

#include "mathh.h"
#include "Global.h"
#include <stb_sprintf.h>
#include <stdio.h>
#include <string.h>

Game* game;

static void AddPokemon(vector<Pokemon>& pokemon,
					   PokemonType type) {
	const PokemonData* data = GetPokemonData(type);

	Pokemon p = {};
	p.type = type;
	p.hp = data->max_hp;
	p.energy = data->max_energy;

	pokemon.push_back(p);
}

/*
static void* malloc_print(size_t size) {
	printf("malloc(%zu)\n", size);
	return malloc(size);
}

static void* calloc_print(size_t count, size_t size) {
	printf("calloc(%zu, %zu)\n", count, size);
	return calloc(count, size);
}

static void* realloc_print(void* ptr, size_t size) {
	printf("realloc(%p, %zu)\n", ptr, size);
	return realloc(ptr, size);
}

static void free_print(void* ptr) {
	printf("free(%p)\n", ptr);
	free(ptr);
}
//*/

void Game::Init() {
	font = fnt_gameboy;

	// SDL_SetMemoryFunctions(malloc_print, calloc_print, realloc_print, free_print);

	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

	SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "system");
	// SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");

	SDL_Init(SDL_INIT_VIDEO
			 | SDL_INIT_AUDIO);

	// for (int i = 0; i < SDL_GetNumVideoDrivers(); i++) {
	// 	printf("%s\n", SDL_GetVideoDriver(i));
	// }

	// for (int i = 0; i < SDL_GetNumAudioDrivers(); i++) {
	// 	printf("%s\n", SDL_GetAudioDriver(i));
	// }

	window = SDL_CreateWindow("Pokemon-C++",
							  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
							  GAME_W * 2, GAME_H * 2,
							  SDL_WINDOW_RESIZABLE);

	renderer = SDL_CreateRenderer(window, -1,
								  SDL_RENDERER_PRESENTVSYNC
								  | SDL_RENDERER_TARGETTEXTURE);

	SDL_RenderSetLogicalSize(renderer, GAME_W, GAME_H);
	SDL_RenderSetVSync(renderer, 1);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	game_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET,
									 GAME_W, GAME_H);

	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT,
				  MIX_DEFAULT_CHANNELS, 2048);

	Mix_Volume(-1, MIX_MAX_VOLUME * 0.0);

	LoadAssets();

	AddPokemon(players[0].pokemon, POKEMON_PIKACHU);
	AddPokemon(players[0].pokemon, POKEMON_BULBASAUR);

	AddPokemon(players[1].pokemon, POKEMON_BULBASAUR);
	AddPokemon(players[1].pokemon, POKEMON_CHARMANDER);

	players_visual[0] = players[0];
	players_visual[1] = players[1];

	SetState(GAME_STATE_SELECT_ACTION, 0);
}

void Game::Destroy() {
	UnloadAssets();

	Mix_CloseAudio();

	SDL_DestroyTexture(game_texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
}

static double GetTime() {
	return (double)SDL_GetPerformanceCounter() / (double)SDL_GetPerformanceFrequency();
}

void Game::Run() {
	double prev_time = GetTime();

	bool quit = false;
	while (!quit) {
		memset(key_pressed, 0, sizeof(key_pressed));

		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			switch (ev.type) {
				case SDL_QUIT: quit = true; break;

				case SDL_KEYDOWN: {
					int scancode = ev.key.keysym.scancode;
					if (0 <= scancode && scancode < (SDL_SCANCODE_UP + 1)) {
						key_pressed[scancode / 32] |= 1 << (scancode % 32);
					}
					break;
				}
			}
		}

		double time = GetTime();

		float delta = (float) ((time - prev_time) * 60.0);
		delta = fminf(delta, 2);

		prev_time = time;

		Update(delta);

		Draw(delta);
	}
}

static bool NavigateMenuAndSelect(int& cursor,
								  int nitems,
								  int horizontal = 0) {
	if (IsKeyPressed(SDL_SCANCODE_UP)) {
		if (cursor - 1 >= 0) {
			cursor--;
			PlaySound(snd_select);
		}
	}

	if (IsKeyPressed(SDL_SCANCODE_DOWN)) {
		if (cursor + 1 < nitems) {
			cursor++;
			PlaySound(snd_select);
		}
	}

	if (horizontal != 0) {
		if (IsKeyPressed(SDL_SCANCODE_LEFT)) {
			if (cursor - horizontal >= 0) {
				cursor -= horizontal;
				PlaySound(snd_select);
			}
		}

		if (IsKeyPressed(SDL_SCANCODE_RIGHT)) {
			int columns = (nitems + horizontal - 1) / horizontal;
			int column = cursor / horizontal;

			if (column < columns - 1) {
				cursor += horizontal;
				cursor = min(cursor, nitems - 1);
				PlaySound(snd_select);
			}
		}
	}

	if (IsKeyPressed(SDL_SCANCODE_Z)) {
		PlaySound(snd_confirm);
		return true;
	}

	return false;
}

void Game::PerformAttack() {
	Player& p = players[player_index];
	Pokemon& pokemon = p.pokemon[p.pokemon_index];

	Player& opponent = (player_index == 0) ? players[1] : players[0];
	Pokemon& opponent_pokemon = opponent.pokemon[opponent.pokemon_index];

	const AttackData& attack = GetPokemonData(pokemon.type)->attacks[submenu_cursor];

	opponent_pokemon.hp -= attack.damage;
}

void Game::Update(float delta) {
	skip_draw = false;

	//*
	switch (state) {
		case GAME_STATE_SELECT_ACTION: {
			for (int i = 0; i < PLAYER_COUNT; i++) {
				if (players[i].pokemon.empty()) {
					int winner = 1 - i;
					SetState(GAME_STATE_WON, winner);
					goto state_select_action_out;
				}

				if (players[i].pokemon_index == -1) {
					if (temp_player_index == -1) temp_player_index = player_index;
					SetState(GAME_STATE_SELECT_POKEMON, i);
					goto state_select_action_out;
				}
			}

			if (temp_player_index != -1) {
				player_index = temp_player_index;
				temp_player_index = -1;
			}

			if (submenu == -1) {
				if (NavigateMenuAndSelect(cursor, 4, 3)) {
					SetSubState(cursor);
					goto state_select_action_out;
				}
			} else {
				switch (submenu) {
					case 0: {
						Player& p = players[player_index];
						Pokemon& pokemon = p.pokemon[p.pokemon_index];
						const PokemonData* data = GetPokemonData(pokemon.type);

						if (NavigateMenuAndSelect(submenu_cursor, data->attack_count)) {
							PerformAttack();
							Player& opponent = (player_index == 0) ? players[1] : players[0];
							if (opponent.pokemon[opponent.pokemon_index].hp <= 0) {

								opponent.pokemon.erase(opponent.pokemon.begin() + opponent.pokemon_index);
								opponent.pokemon_index = -1;

								animation_attack_index = submenu_cursor;
								// SetState(GAME_STATE_WATCHING_DEATH_ANIM, player_index);
								SetState(GAME_STATE_WATCHING_ATTACK_ANIM, player_index);
							} else {
								animation_attack_index = submenu_cursor;
								SetState(GAME_STATE_WATCHING_ATTACK_ANIM, player_index);
							}
							goto state_select_action_out;
						}
						break;
					}

					case 1: {
						break;
					}

					case 2: {
						Player& p = players[player_index];

						if (NavigateMenuAndSelect(submenu_cursor, p.pokemon.size())) {
							
						}
						break;
					}
				}

				if (IsKeyPressed(SDL_SCANCODE_X)) {
					SetSubState(-1);
					goto state_select_action_out;
				}
			}

			players_visual[0] = players[0];
			players_visual[1] = players[1];

		state_select_action_out:
			break;
		}

		case GAME_STATE_SELECT_POKEMON: {
			Player& p = players[player_index];

			if (NavigateMenuAndSelect(cursor, p.pokemon.size())) {
				p.pokemon_index = cursor;

				// if (player_index + 1 < PLAYER_COUNT) {
				// 	if (players[player_index + 1].pokemon_index == -1) {
				// 		SetState(GAME_STATE_SELECT_POKEMON, player_index + 1);
				// 		break;
				// 	}
				// }

				SetState(GAME_STATE_SELECT_ACTION, player_index);
				break;
			}

			players_visual[0] = players[0];
			players_visual[1] = players[1];

			break;
		}

		case GAME_STATE_WATCHING_ATTACK_ANIM: {
			switch (animation_timer) {
				case 60:
					PlaySound(snd_slash);
					break;

				case 2 * 60:
					PlaySound(snd_hurt);
					break;
			}

			if (animation_timer >= 3 * 60) {
				int player = player_index;
				player++;
				player %= PLAYER_COUNT;

				SetState(GAME_STATE_SELECT_ACTION, player);
				break;
			}

			if (animation_timer >= 2 * 60) {
				for (int i = 0; i < PLAYER_COUNT; i++) {
					Player& player = players[i];
					Player& player_visual = players_visual[i];

					Pokemon& pokemon_visual = player_visual.pokemon[player_visual.pokemon_index];

					if (player.pokemon_index == -1) {
						pokemon_visual.hp = approach(pokemon_visual.hp, 0.0f, 0.9f * delta);
					} else {
						Pokemon& pokemon = player.pokemon[player.pokemon_index];
						pokemon_visual.hp = approach(pokemon_visual.hp, pokemon.hp, 0.9f * delta);
					}
				}
			}

			animation_timer++;
			text_teletype++;
			break;
		}

		case GAME_STATE_WON: {

			players_visual[0] = players[0];
			players_visual[1] = players[1];

			text_teletype++;

			break;
		}
	}
	//*/
}

static void DrawPokemonLabel(int x, int y,
							 const Pokemon& pokemon) {
	y -= 2 * 8;

	const PokemonData* data = GetPokemonData(pokemon.type);
	DrawText(game->font, x, y, data->name);

	y += 8;

	char buf[10];
	stb_snprintf(buf, sizeof(buf), "%.0f/%.0f", pokemon.hp, data->max_hp);
	DrawText(game->font, x, y, buf);
}

static void DrawPokemon(int player_index,
						u32 texture_index,
						int x, int y) {
	const Player* players = game->players_visual;

	const Player& p = players[player_index];
	const Pokemon& pokemon = p.pokemon[p.pokemon_index];

	SDL_Rect src;
	src.x = 56 * pokemon.type;
	src.y = 0;
	src.w = 56;
	src.h = 56;

	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
	dest.w = 56;
	dest.h = 56;

	bool flash = false;
	if (game->state == GAME_STATE_WATCHING_ATTACK_ANIM && game->player_index == (1 - player_index)) {
		if (game->animation_timer >= 2 * 60) {
			flash = true;
		}
	}

	if (!flash || SDL_GetTicks() / 100 % 2) {
		SDL_Texture* texture = GetTexture(texture_index);
		SDL_RenderCopy(game->renderer, texture, &src, &dest);
	}

	DrawPokemonLabel(dest.x, dest.y, pokemon);
}

void Game::Draw(float delta) {
	const Player* players = players_visual;

	SDL_SetRenderTarget(renderer, game_texture);

	if (!skip_draw) {
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);

		int ui_y = GAME_H - 64;

		if (players[1].pokemon_index != -1) {
			// draw pokemon front
			int x = GAME_W - 56 - 5 * 8;
			int y = 6 * 8;
			DrawPokemon(1, tex_pokemon_front, x, y);
		}

		if (players[0].pokemon_index != -1) {
			// draw pokemon back
			int x = 3 * 8;
			int y = ui_y - 56 - 16;
			DrawPokemon(0, tex_pokemon_back, x, y);
		}

		DrawUI(ui_y);
	}

	SDL_SetRenderTarget(renderer, nullptr);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	{
		SDL_Rect src = {0, 0, GAME_W, GAME_H};
		SDL_Rect dest = {0, 0, GAME_W, GAME_H};
		SDL_RenderCopy(renderer, game_texture, &src, &dest);
	}

	SDL_RenderPresent(renderer);
}

static void DrawCursor(int x, int y, int cursor,
					   int ysep = 8, int xsep = 12 * 8,
					   int items_in_column = 100) {
	int ch = 0x80 + (SDL_GetTicks() / 300) % 2;
	char buf[] = {ch, 0};
	DrawText(fnt_gameboy,
			 x + (cursor / items_in_column) * xsep,
			 y + (cursor % items_in_column) * ysep,
			 buf);
}

static void DrawPokemonMenu(int x, int y, int w, int h,
							int cursor,
							const Player& p,
							bool no_panel = false) {
	if (!no_panel) {
		DrawNineslice(x, y, w, h);
	}

	x += 8;
	y += 8;

	DrawCursor(x, y, cursor);

	x += 8;

	for (int i = 0; i < p.pokemon.size(); i++) {
		const PokemonData* data = GetPokemonData(p.pokemon[i].type);
		SDL_Color color = (i == p.pokemon_index) ? SDL_Color{128, 128, 128, 255} : SDL_Color{0, 0, 0, 255};
		DrawText(game->font, x, y + i * 8, data->name, color);
	}
}

void Game::DrawUI(int ui_y) {
	// draw ui

	const Player* players = players_visual;

	int _x = 0;
	int _y = ui_y;

	{
		int x = _x;
		int y = _y;

		y -= 2 * 8;

		DrawNineslice(x, y, 9 * 8, 3 * 8);

		x += 8;
		y += 8;

		char buf[10];
		stb_snprintf(buf, sizeof(buf), "Игрок %d", player_index + 1);
		DrawText(font, x, y, buf);
	}

	DrawNineslice(_x, _y, GAME_W, GAME_H - _y);

	switch (state) {
		case GAME_STATE_SELECT_POKEMON: {
			int x = _x;
			int y = _y;

			const Player& p = players[player_index];

			DrawPokemonMenu(x, y, GAME_W, GAME_H - y, cursor, p, true);
			break;
		}

		case GAME_STATE_SELECT_ACTION: {
			int x = _x;
			int y = _y;

			x += 8;
			y += 8;

			DrawCursor(x, y, cursor, 16, 12 * 8, 3);

			x += 8;

			const char* strings[] = {
				"Атака",
				"Предметы",
				"Покемоны",
				"Информация о\nсопернике",
			};
			for (int i = 0; i < ArrayLength(strings); i++) {
				DrawText(font, x + (i / 3) * 12 * 8, y + (i % 3) * 16, strings[i]);
			}

			if (submenu != -1) {
				int off = 6 * 8;

				int x = _x + off;
				int y = _y;

				DrawNineslice(x, y, GAME_W - x, GAME_H - y);

				if (submenu == 0) {
					x += 8;
					y += 8;

					DrawCursor(x, y, submenu_cursor);

					x += 8;

					const Player& p = players[player_index];
					const Pokemon& pokemon = p.pokemon[p.pokemon_index];
					const PokemonData* data = GetPokemonData(pokemon.type);

					for (int i = 0; i < data->attack_count; i++) {
						DrawText(font, x, y + 8 * i, data->attacks[i].name);
					}
				} else if (submenu == 2) {
					const Player& p = players[player_index];
					DrawPokemonMenu(x, y, GAME_W - x, GAME_H - y, submenu_cursor, p, true);
				}
			}
			break;
		}

		case GAME_STATE_WATCHING_ATTACK_ANIM: {
			const Player& p = players[player_index];
			const Pokemon& pokemon = p.pokemon[p.pokemon_index];
			const PokemonData* data = GetPokemonData(pokemon.type);
			const AttackData& attack = data->attacks[animation_attack_index];

			int x = _x + 8;
			int y = _y + 8;

			char buf[100];
			stb_snprintf(buf, min(text_teletype + 1, (int)sizeof(buf)), "%s использует %s!", data->name, attack.name);
			DrawText(font, x, y, buf);
			break;
		}

		case GAME_STATE_WON: {
			int x = _x + 8;
			int y = _y + 8;

			char buf[100];
			stb_snprintf(buf, min(text_teletype + 1, (int)sizeof(buf)), "Игрок %d победил!", player_index + 1);
			DrawText(font, x, y, buf);
			break;
		}
	}
}
