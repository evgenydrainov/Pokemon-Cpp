#include "assets.h"

#include "game.h"
#include <SDL_image.h>

#include "font_data.h"

SDL_Texture* tex_font_gameboy;
SDL_Texture* tex_nineslice_gameboy;
SDL_Texture* tex_pokemon_back;
SDL_Texture* tex_pokemon_front;
SDL_Texture* tex_ms_gothic;
SDL_Texture* tex_nineslice2;
SDL_Texture* tex_attack_slash_right;
SDL_Texture* tex_element_icon;

const Font* fnt_gameboy = &_fnt_gameboy;
const Font* fnt_ms_gothic = &_fnt_ms_gothic;

void load_assets() {
	{
		IMG_Init(IMG_INIT_PNG);

		tex_font_gameboy = IMG_LoadTexture(game->renderer, "textures/font_gameboy.png");
		tex_nineslice_gameboy = IMG_LoadTexture(game->renderer, "textures/nineslice_gameboy.png");
		tex_pokemon_back = IMG_LoadTexture(game->renderer, "textures/pokemon_back.png");
		tex_pokemon_front = IMG_LoadTexture(game->renderer, "textures/pokemon_front.png");
		tex_ms_gothic = IMG_LoadTexture(game->renderer, "textures/ms_gothic.png");
		tex_nineslice2 = IMG_LoadTexture(game->renderer, "textures/nineslice2.png");
		tex_attack_slash_right = IMG_LoadTexture(game->renderer, "textures/attack_slash_right.png");
		tex_element_icon = IMG_LoadTexture(game->renderer, "textures/element_icon.png");

		IMG_Quit();
	}

	_fnt_gameboy.texture = tex_font_gameboy;
	_fnt_ms_gothic.texture = tex_ms_gothic;
}

void unload_assets() {
	SDL_DestroyTexture(tex_element_icon);
	SDL_DestroyTexture(tex_attack_slash_right);
	SDL_DestroyTexture(tex_nineslice2);
	SDL_DestroyTexture(tex_ms_gothic);
	SDL_DestroyTexture(tex_pokemon_front);
	SDL_DestroyTexture(tex_pokemon_back);
	SDL_DestroyTexture(tex_nineslice_gameboy);
	SDL_DestroyTexture(tex_font_gameboy);
}
