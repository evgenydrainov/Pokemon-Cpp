#pragma once

#include "text.h"
#include <SDL.h>
#include <SDL_mixer.h>

extern SDL_Texture* tex_font_gameboy;
extern SDL_Texture* tex_nineslice_gameboy;
extern SDL_Texture* tex_pokemon_back;
extern SDL_Texture* tex_pokemon_front;
extern SDL_Texture* tex_ms_gothic;
extern SDL_Texture* tex_nineslice2;
extern SDL_Texture* tex_attack_slash_right;

extern const Font* fnt_gameboy;
extern const Font* fnt_ms_gothic;

void load_assets();
void unload_assets();
