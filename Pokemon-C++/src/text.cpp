#include "text.h"

#include "game.h"
#include "mathh.h"

void draw_list_add_text(DrawList& draw_list,
						const Font* font, int x, int y,
						const char* text,
						int line_width,
						int max_chars) {
	int ch_x = x;
	int ch_y = y;

	int ch;
	for (const char* ptr = text; (ch = (u8)*ptr) && max_chars--; ptr++) {
		if (33 <= ch && ch <= 255) {
			const Glyph& glyph = font->glyphs[ch - 32];

			SDL_Rect src = glyph.src;

			SDL_Rect dest;
			dest.x = ch_x + glyph.xoffset;
			dest.y = ch_y + glyph.yoffset;
			dest.w = glyph.src.w;
			dest.h = glyph.src.h;

			if (dest.x + dest.w > line_width) {
				ch_x = x;
				ch_y += font->lineskip;

				dest.x = ch_x + glyph.xoffset;
				dest.y = ch_y + glyph.yoffset;
				dest.w = glyph.src.w;
				dest.h = glyph.src.h;
			}

			SDL_Texture* texture = font->texture;
			draw_list_add_texture(draw_list, texture, src, dest);

			ch_x += glyph.advance;
		} else if (ch == ' ') {
			const Glyph& glyph = font->glyphs[ch - 32];

			ch_x += glyph.advance;
		} else if (ch == '\n') {
			ch_x = x;
			ch_y += font->lineskip;
		}
	}
}

SDL_Point measure_text(const Font* font,
					   const char* text,
					   bool only_one_line) {
	int ch_x = 0;
	int ch_y = 0;

	// 
	// Size of empty string is (0,lineskip).
	// 

	int w = 0;
	int h = font->lineskip;

	int ch;
	for (const char* ptr = text; ch = (u8)*ptr; ptr++) {
		if (33 <= ch && ch <= 255) {
			const Glyph& glyph = font->glyphs[ch - 32];

			SDL_Rect dest;
			dest.x = ch_x + glyph.xoffset;
			dest.y = ch_y + glyph.yoffset;
			dest.w = glyph.src.w;
			dest.h = glyph.src.h;

			// 
			// dest.x + dest.w is more "correct".
			// But ch_x + glyph.advance fits better for current usage?
			// 

			// w = max(w, dest.x + dest.w);
			w = max(w, ch_x + glyph.advance);
			h = max(h, ch_y + font->lineskip);

			ch_x += glyph.advance;
		} else if (ch == ' ') {
			const Glyph& glyph = font->glyphs[ch - 32];

			w = max(w, ch_x + glyph.advance);
			h = max(h, ch_y + font->lineskip);

			ch_x += glyph.advance;
		} else if (ch == '\n') {
			if (only_one_line) {
				return {w, h};
			}

			ch_x = 0;
			ch_y += font->lineskip;

			h = max(h, ch_y + font->lineskip);
		}
	}

	return {w, h};
}
