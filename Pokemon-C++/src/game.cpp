#include "game.h"

#include "assets.h"
#include "mathh.h"
#include <stb_sprintf.h>
#include <string.h>

Game* game;

static void _battle_script(mco_coro* co) {
	game->battle_script(co);
}

void Game::init() {
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

	SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "system");

	SDL_Init(SDL_INIT_VIDEO
			 | SDL_INIT_AUDIO);

	window = SDL_CreateWindow("Pokemon-C++",
							  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
							  GAME_W * 2, GAME_H * 2,
							  SDL_WINDOW_RESIZABLE);

	renderer = SDL_CreateRenderer(window, -1, 0);

	SDL_RenderSetLogicalSize(renderer, GAME_W, GAME_H);
	SDL_RenderSetVSync(renderer, 1);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	// SDL_RenderSetIntegerScale(renderer, SDL_TRUE);

	game_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET,
									 GAME_W, GAME_H);

	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT,
				  MIX_DEFAULT_CHANNELS, 2048);

	Mix_Volume(-1, MIX_MAX_VOLUME * 0.0);

	load_assets();
	load_pokemon_data_from_file_txt("pokemon_data.txt");

	{
		mco_desc desc = mco_desc_init(_battle_script, 0);
		mco_create(&co, &desc);
	}

	font = fnt_ms_gothic;
	nineslice_texture = tex_nineslice2;

	slice_w = texture_get_width(nineslice_texture) / 3;
	slice_h = texture_get_height(nineslice_texture) / 3;

	main_panel_w = GAME_W;
	main_panel_h = 2 * slice_h + 6 * font->lineskip;
	main_panel_x = 0;
	main_panel_y = GAME_H - main_panel_h;

	{
		Pokemon p = {};
		p.id = POKEMON_PIKACHU;
		p.hp = get_pokemon_data(p.id)->max_hp;
		p.energy = get_pokemon_data(p.id)->max_energy;
		players[0].pokemon.push_back(p);
	}

	{
		Pokemon p = {};
		p.id = POKEMON_BULBASAUR;
		p.hp = get_pokemon_data(p.id)->max_hp;
		p.energy = get_pokemon_data(p.id)->max_energy;
		players[0].pokemon.push_back(p);
		players[1].pokemon.push_back(p);
	}

	{
		Pokemon p = {};
		p.id = POKEMON_CHARMANDER;
		p.hp = get_pokemon_data(p.id)->max_hp;
		p.energy = get_pokemon_data(p.id)->max_energy;
		players[1].pokemon.push_back(p);
	}

	pokemon_visual[0].x = 32;
	pokemon_visual[0].y = GAME_H - main_panel_h - slice_h - font->lineskip - 56;

	pokemon_visual[1].x = GAME_W - 56 - 32;
	pokemon_visual[1].y = 64;
	pokemon_visual[1].front = true;
}

void Game::destroy() {
	unload_assets();

	Mix_CloseAudio();

	SDL_DestroyTexture(game_texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
}

void Game::run() {
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

		update(1);

		draw(1);
	}
}

void Game::update(float delta) {
	draw_list.clear();

	if (update_pokemon_visual) {
		for (int i = 0; i < MAX_PLAYERS; i++) {
			Player& player = players[i];

			if (player.pokemon_index == -1) {
				pokemon_visual[i].visible = false;
			} else {
				Pokemon& player_pokemon = player.pokemon[player.pokemon_index];
				const PokemonData* data = get_pokemon_data(player_pokemon.id);

				pokemon_visual[i].id = player_pokemon.id;
				pokemon_visual[i].hp_target = player_pokemon.hp;
				pokemon_visual[i].energy_target = player_pokemon.energy;
				pokemon_visual[i].visible = true;
			}
		}
	}

	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (!pokemon_visual[i].visible) {
			continue;
		}
		if (pokemon_visual[i].hp == -1) {
			pokemon_visual[i].hp = pokemon_visual[i].hp_target;
		}
		if (pokemon_visual[i].energy == -1) {
			pokemon_visual[i].energy = pokemon_visual[i].energy_target;
		}
		pokemon_visual[i].hp = approach(pokemon_visual[i].hp, pokemon_visual[i].hp_target, 1.0f * delta);
		pokemon_visual[i].energy = approach(pokemon_visual[i].energy, pokemon_visual[i].energy_target, 1.0f * delta);
	}

	for (int i = 0; i < MAX_PLAYERS; i++) {
		PokemonVisual& p = pokemon_visual[i];

		if (!p.visible) {
			continue;
		}

		const PokemonData* data = get_pokemon_data(p.id);

		SDL_Texture* texture = (p.front) ? tex_pokemon_front : tex_pokemon_back;

		SDL_Rect src;
		src.x = 56 * p.id;
		src.y = 0;
		src.w = 56;
		src.h = 56;

		SDL_Rect dest;
		dest.x = p.x;
		dest.y = p.y;
		dest.w = 56;
		dest.h = 56;

		draw_list_add_texture(draw_list, texture, src, dest);

		int x = dest.x;
		int y = dest.y - 3 * font->lineskip;
		const char* text = get_pokemon_data(p.id)->name;

		draw_list_add_text(draw_list, font, x, y, text);
		y += font->lineskip;
		{
			char buf[16];
			stb_snprintf(buf, sizeof(buf), "%.0f хп", p.hp);
			draw_list_add_text(draw_list, font, x, y, buf);
			y += font->lineskip;
		}
		{
			char buf[16];
			stb_snprintf(buf, sizeof(buf), "%.0f энергии", p.energy);
			draw_list_add_text(draw_list, font, x, y, buf);
			y += font->lineskip;
		}
	}

	{
		char buf[] = "Игрок .";
		buf[6] = '1' + player_index_visual;
		int panel_w = 2 * slice_w + measure_text(font, buf).x;
		int panel_h = 2 * slice_h + font->lineskip;
		int panel_x = 0;
		int panel_y = main_panel_y - panel_h + slice_h;

		draw_list_add_nineslice(draw_list, nineslice_texture,
								panel_x, panel_y, panel_w, panel_h);

		draw_list_add_text(draw_list, font,
							panel_x + slice_w, panel_y + slice_h,
							buf);
	}

	draw_list_add_nineslice(draw_list, nineslice_texture,
							main_panel_x, main_panel_y, main_panel_w, main_panel_h);

	for (int i = 0; i < dialogs.size(); i++) {
		Dialog& d = dialogs[i];
		bool dead = false;

		switch (d.type) {
			case DIALOG_TYPE_SELECTION: {
				if (!(d.flags & DIALOG_HIDE_PANEL)) {
					draw_list_add_nineslice(draw_list, nineslice_texture, d.x, d.y, d.w, d.h);
				}

				int x = d.x;
				int y = d.y;

				x += slice_w;
				y += slice_h;

				if (d.selection.label) {
					draw_list_add_text(draw_list, font, x, y, d.selection.label);
					y += font->lineskip;
				}

				x += 8;

				for (int i = 0; i < d.selection.item_count; i++) {
					if (d.selection.disabled_items & (1 << i)) {
						draw_list_add_set_color(draw_list, {255, 255, 255, 128});
					}

					draw_list_add_text(draw_list, font, x, y, d.selection.items[i]);

					if (d.selection.disabled_items & (1 << i)) {
						draw_list_add_set_color(draw_list, {255, 255, 255, 255});
					}

					if (i == d.selection.cursor) {
						int ch = 0x80 + (SDL_GetTicks() / 300) % 2;
						char buf[] = {(char)ch, 0};
						draw_list_add_text(draw_list, fnt_gameboy, x - 8, y, buf);
					}

					y += font->lineskip;
				}

				break;
			}

			case DIALOG_TYPE_TELETYPE: {
				if (!(d.flags & DIALOG_HIDE_PANEL)) {
					draw_list_add_nineslice(draw_list, nineslice_texture, d.x, d.y, d.w, d.h);
				}

				int x = d.x + slice_w;
				int y = d.y + slice_h;
				int line_w = d.w - 2 * slice_w;
				const char* text = d.teletype.text;
				int max_chars = d.teletype.count++;

				draw_list_add_text(draw_list, font, x, y, text, line_w, max_chars);

				break;
			}

			case DIALOG_TYPE_SPRITE: {

				int frame_index = clamp((int)d.sprite.frame_index, 0, d.sprite.frame_count - 1);

				SDL_Texture* texture = d.sprite.texture;

				SDL_Rect src;
				src.x = frame_index * d.w;
				src.y = 0;
				src.w = d.w;
				src.h = d.h;

				SDL_Rect dest;
				dest.x = d.x;
				dest.y = d.y;
				dest.w = d.w;
				dest.h = d.h;

				draw_list_add_texture(draw_list, texture, src, dest);

				d.sprite.frame_index += d.sprite.anim_spd * delta;

				if ((int)d.sprite.frame_index >= d.sprite.frame_count) {
					dead = true;
				}

				break;
			}
		}

		if (dead) {
			dialogs.erase(dialogs.begin() + i);
			i--;
			continue;
		}
	}

	if (mco_status(co) == MCO_SUSPENDED) {
		mco_resume(co);
	}
}

Dialog* Game::get_dialog(u32 dialog_id) {
	for (Dialog& d : dialogs) {
		if (d.id == dialog_id) {
			return &d;
		}
	}
	return nullptr;
}

void Game::destroy_dialog(u32 dialog_id) {
	for (auto it = dialogs.begin(); it != dialogs.end();) {
		if (it->id == dialog_id) {
			it = dialogs.erase(it);
			break;
		}

		it++;
	}
}

#if 0
static void ask_are_you_sure(mco_coro* co, int x = 0, int y = 0) {
	const char* items[] = {"Nah, I'd win", "Nah, I'd win"};
	u32 dialog_id = game->create_selection_dialog("But would you lose?", items, ArrayLength(items), x, y, 0, 0, DIALOG_ALLOW_CANCEL);
	if (x > 200) {
		y += 32;
	} else {
		x += 32;
	}
	while (true) {
		int option = game->selection_dialog(co, dialog_id);
		if (option == 0) {
			ask_are_you_sure(co, x, y);
		} else if (option == 1) {
			break;
		}
	}
	game->destroy_dialog(dialog_id);
}
#endif

static void wait(mco_coro* co, int frames) {
	while (frames--) {
		mco_yield(co);
	}
}

void Game::battle_script(mco_coro* co) {
	int player_index = 0;
	int turn = 0;

	while (true) {

		for (int i = 0; i < MAX_PLAYERS; i++) {
			if (players[i].pokemon_index == -1) {
				if (players[i].pokemon.empty()) {
					player_index = 1 - i;
					goto l_player_won;
				}

				int x = main_panel_x;
				int y = main_panel_y;

				player_index_visual = i;
				players[i].pokemon_index = player_select_pokemon(co, i,
																 x, y,
																 DIALOG_HIDE_PANEL);
				
				Pokemon& p = players[i].pokemon[players[i].pokemon_index];
				pokemon_visual[player_index].id = p.id;
				pokemon_visual[player_index].hp = p.hp;
				pokemon_visual[player_index].hp_target = p.hp;
				pokemon_visual[player_index].energy = p.energy;
				pokemon_visual[player_index].energy_target = p.energy;
			}
		}

		{
			Player& player = players[player_index];
			for (int i = 0; i < player.pokemon.size(); i++) {
				Pokemon& player_pokemon = player.pokemon[i];
				const PokemonData* data = get_pokemon_data(player_pokemon.id);

				player_pokemon.energy += 10;
				player_pokemon.energy = min(player_pokemon.energy, data->max_energy);
			}
		}

		player_index_visual = player_index;

		int attack_index;
		int pokemon_index;
		int action = player_select_action(co, player_index, &attack_index, &pokemon_index);

		if (action == 0) {
			update_pokemon_visual = false;

			{
				Player& player = players[player_index];
				Pokemon& player_pokemon = player.pokemon[player.pokemon_index];
				Player& opponent = players[1 - player_index];
				Pokemon& opponent_pokemon = opponent.pokemon[opponent.pokemon_index];
				const PokemonData* data = get_pokemon_data(player_pokemon.id);

				opponent_pokemon.hp -= data->attacks[attack_index].damage;
				player_pokemon.energy -= data->attacks[attack_index].energy;

				pokemon_visual[player_index].energy_target = player_pokemon.energy;

				if (opponent_pokemon.hp <= 0) {
					opponent.pokemon.erase(opponent.pokemon.begin() + opponent.pokemon_index);
					opponent.pokemon_index = -1;
				}

				mco_yield(co);
			}

			char teletype_buf[64];
			u32 teletype_dialog_id;
			{
				Player& player = players[player_index];
				Pokemon& player_pokemon = player.pokemon[player.pokemon_index];
				const PokemonData* data = get_pokemon_data(player_pokemon.id);

				int x = main_panel_x;
				int y = main_panel_y;
				stb_snprintf(teletype_buf, sizeof(teletype_buf), "%s использует %s!", data->name, data->attacks[attack_index]);
				teletype_dialog_id = create_teletype_dialog(teletype_buf, x, y, GAME_W, 0, DIALOG_HIDE_PANEL);
			}

			wait(co, 60);

			{
				int x = pokemon_visual[1 - player_index].x;
				int y = pokemon_visual[1 - player_index].y;
				create_sprite(tex_attack_slash_right, x, y, 48, 48, 4, 0.1);
			}

			wait(co, 60);

			{
				Player& opponent = players[1 - player_index];
				if (opponent.pokemon_index == -1) {
					pokemon_visual[1 - player_index].hp_target = 0;
				} else {
					Pokemon& opponent_pokemon = opponent.pokemon[opponent.pokemon_index];
					pokemon_visual[1 - player_index].hp_target = opponent_pokemon.hp;
				}
			}

			wait(co, 60);

			destroy_dialog(teletype_dialog_id);
			update_pokemon_visual = true;

		} else if (action == 1) {
			players[player_index].pokemon_index = pokemon_index;

			Pokemon& p = players[player_index].pokemon[players[player_index].pokemon_index];
			pokemon_visual[player_index].id = p.id;
			pokemon_visual[player_index].hp = p.hp;
			pokemon_visual[player_index].hp_target = p.hp;
			pokemon_visual[player_index].energy = p.energy;
			pokemon_visual[player_index].energy_target = p.energy;
		}

		player_index++;
		player_index %= MAX_PLAYERS;
		turn++;

	}

l_player_won:

	while (true) {
		int x = main_panel_x + slice_w;
		int y = main_panel_y + slice_h;
		char buf[64];
		stb_snprintf(buf, sizeof(buf), "Игрок %d победил!", player_index + 1);
		int line_width = GAME_W - 2 * slice_w;

		draw_list_add_text(draw_list, font, x, y, buf, line_width);

		mco_yield(co);
	}

}

u32 Game::create_select_action_dialog(mco_coro* co, int player_index) {
	static const char* items[] = {
		"Атака",
		"Покемоны",
	};

	int x = main_panel_x;
	int y = main_panel_y;

	u32 result = create_selection_dialog(nullptr,
										 items, ArrayLength(items),
										 x, y, 0, 0,
										 DIALOG_HIDE_PANEL);
	return result;
}

int Game::player_select_action(mco_coro* co, int player_index,
							   int* out_attack_index, int* out_pokemon_index) {
	int result;

	u32 dialog_id = create_select_action_dialog(co, player_index);

	while (true) {
		int action = selection_dialog(co, dialog_id);

		if (action == 0) {
			Player& player = players[player_index];
			Pokemon& player_pokemon = player.pokemon[player.pokemon_index];
			const PokemonData* data = get_pokemon_data(player_pokemon.id);

			const char* items[4];
			int item_count = min(data->attack_count, (int)ArrayLength(items));
			u32 disabled_items = 0;
			for (int i = 0; i < item_count; i++) {
				items[i] = data->attacks[i].name;
				if (player_pokemon.energy < data->attacks[i].energy) {
					disabled_items |= 1 << i;
				}
			}
			int x = main_panel_x + 64;
			int y = main_panel_y;
			int w = GAME_W - x;
			int h = GAME_H - y;
			u32 dialog_id = create_selection_dialog(nullptr,
													items, item_count,
													x, y, w, h,
													DIALOG_ALLOW_CANCEL,
													disabled_items);
			int attack_index = selection_dialog(co, dialog_id);
			destroy_dialog(dialog_id);
			if (attack_index != -1) {
				*out_attack_index = attack_index;
				result = action;
				break;
			}
		} else if (action == 1) {
			int x = main_panel_x + 64;
			int y = main_panel_y;

			int pokemon_index = player_select_pokemon(co, player_index,
													  x, y,
													  DIALOG_ALLOW_CANCEL);
			if (pokemon_index != -1) {
				result = action;
				*out_pokemon_index = pokemon_index;
				break;
			}
		}
	}

	destroy_dialog(dialog_id);

	return result;
}

int Game::player_select_pokemon(mco_coro* co, int player_index,
								int x, int y,
								u32 flags) {
	std::vector<Pokemon>& pokemon = players[player_index].pokemon;

	const char* items[4];
	int item_count = min(pokemon.size(), ArrayLength(items));

	for (int i = 0; i < item_count; i++) {
		items[i] = get_pokemon_data(pokemon[i].id)->name;
	}

	u32 disabled_items = 0;
	if (players[player_index].pokemon_index != -1) {
		disabled_items |= 1 << players[player_index].pokemon_index;
	}

	int w = GAME_W - x;
	int h = GAME_H - y;

	u32 dialog_id = create_selection_dialog(nullptr, items, item_count,
											x, y, w, h,
											flags, disabled_items);

	int pokemon_index = selection_dialog(co, dialog_id);

	destroy_dialog(dialog_id);

	return pokemon_index;
}

u32 Game::create_selection_dialog(const char* label,
								  const char* items[], int item_count,
								  int x, int y, int w, int h,
								  u32 flags,
								  u32 disabled_items) {
	Dialog d = {};
	d.type = DIALOG_TYPE_SELECTION;
	d.id = next_dialog_id++;
	d.flags = flags;
	d.x = x;
	d.y = y;
	d.w = w;
	d.h = h;
	d.selection.label = label;
	d.selection.items = items;
	d.selection.item_count = item_count;
	d.selection.disabled_items = disabled_items;

	if (d.w == 0 || d.h == 0) {
		if (d.selection.label) {
			SDL_Point size = measure_text(font, d.selection.label);
			d.w = max(d.w, size.x);
			d.h += font->lineskip;
		}

		for (int i = 0; i < d.selection.item_count; i++) {
			SDL_Point size = measure_text(font, d.selection.items[i]);
			d.w = max(d.w, size.x + 8);
			d.h += font->lineskip;
		}

		d.w += 2 * slice_w;
		d.h += 2 * slice_h;
	}

	dialogs.push_back(d);

	return d.id;
}

u32 Game::create_teletype_dialog(const char* text,
								 int x, int y, int w, int h,
								 u32 flags) {
	Dialog d = {};
	d.type = DIALOG_TYPE_TELETYPE;
	d.id = next_dialog_id++;
	d.flags = flags;
	d.x = x;
	d.y = y;
	d.w = w;
	d.h = h;
	d.teletype.text = text;
	dialogs.push_back(d);
	return d.id;
}

u32 Game::create_sprite(SDL_Texture* texture,
						int x, int y, int w, int h,
						int frame_count, float anim_spd,
						u32 flags) {
	Dialog d = {};
	d.type = DIALOG_TYPE_SPRITE;
	d.id = next_dialog_id++;
	d.flags = flags;
	d.x = x;
	d.y = y;
	d.w = w;
	d.h = h;
	d.sprite.texture = texture;
	d.sprite.frame_count = frame_count;
	d.sprite.anim_spd = anim_spd;
	dialogs.push_back(d);
	return d.id;
}

int Game::selection_dialog(mco_coro* co, u32 dialog_id) {

#define d (*get_dialog(dialog_id))

	while (true) {
		if (is_key_pressed(SDL_SCANCODE_UP)) {
			d.selection.cursor--;
			d.selection.cursor = max(d.selection.cursor, 0);
		}

		if (is_key_pressed(SDL_SCANCODE_DOWN)) {
			d.selection.cursor++;
			d.selection.cursor = min(d.selection.cursor, d.selection.item_count - 1);
		}

		mco_yield(co);

		if (is_key_pressed(SDL_SCANCODE_Z)) {
			if (!(d.selection.disabled_items & (1 << d.selection.cursor))) {
				return d.selection.cursor;
			}
		}

		if (d.flags & DIALOG_ALLOW_CANCEL) {
			if (is_key_pressed(SDL_SCANCODE_X)) {
				return -1;
			}
		}
	}

#undef d

	return 0;
}

int Game::selection_dialog(mco_coro* co,
						   const char* label,
						   const char* items[], int item_count,
						   int x, int y, int w, int h,
						   u32 flags,
						   u32 disabled_items) {
	u32 dialog_id = create_selection_dialog(label, items, item_count, x, y, w, h, flags, disabled_items);
	int result = selection_dialog(co, dialog_id);
	destroy_dialog(dialog_id);
	return result;
}

void Game::draw(float delta) {
	SDL_SetRenderTarget(renderer, game_texture);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);

	SDL_Color color = {255, 255, 255, 255};

	for (const DrawCmd& cmd : draw_list) {
		switch (cmd.type) {
			case DRAW_CMD_TEXTURE: {
				SDL_Texture* texture = cmd.texture.texture;
				SDL_Rect src = cmd.texture.src;
				SDL_Rect dest = cmd.texture.dest;

				SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
				SDL_SetTextureAlphaMod(texture, color.a);
				SDL_RenderCopy(renderer, texture, &src, &dest);
				break;
			}
			case DRAW_CMD_SET_COLOR: {
				color = cmd.set_color.color;
				SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
				break;
			}
		}
	}

	// printf("%zu draw commands\n", draw_list.size());

	SDL_SetRenderTarget(renderer, nullptr);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	SDL_RenderCopy(renderer, game_texture, nullptr, nullptr);

	SDL_RenderPresent(renderer);
}
