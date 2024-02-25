#pragma once

#include "common.h"
#include <SDL.h>
#include <SDL_mixer.h>

enum {
	tex_font,
	tex_nineslice,
	tex_pokemon_back,
	tex_pokemon_front,
	tex_ms_gothic,

	TEXTURE_COUNT,
};

enum {
	fnt_gameboy,
	fnt_ms_gothic,

	FONT_COUNT,
};

enum {
	snd_slash,
	snd_confirm,
	snd_select,
	snd_hurt,

	SOUND_COUNT,
};

struct Glyph {
	SDL_Rect src;
	int xoffset;
	int yoffset;
	int advance;
};

struct Font {
	u32 texture_index;
	int lineskip;
	const Glyph* glyphs; // 32..255
};

void LoadAssets();
void UnloadAssets();

SDL_Texture* GetTexture(u32 texture_index);
const Font* GetFont(u32 font_index);
Mix_Chunk* GetSound(u32 sound_index);
