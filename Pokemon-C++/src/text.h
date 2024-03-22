#pragma once

#include "common.h"
#include "drawing.h"
#include <SDL.h>

struct Glyph {
	SDL_Rect src;
	int xoffset;
	int yoffset;
	int advance;
};

struct Font {
	SDL_Texture* texture;
	int lineskip;
	const Glyph* glyphs; // 32..255
};

void draw_list_add_text(DrawList& draw_list,
						const Font* font, int x, int y,
						const char* text,
						int line_width = 10'000,
						int max_chars = 10'000);

SDL_Point measure_text(const Font* font,
					   const char* text,
					   bool only_one_line = false);
