#include "Global.h"

#include "Game.h"

bool IsKeyPressed(int scancode) {
	bool result = false;
	if (0 <= scancode && scancode < (SDL_SCANCODE_UP + 1)) {
		result = game->key_pressed[scancode / 32] & (1 << (scancode % 32));
	}
	return result;
}

/*
void DrawText(int x, int y,
			  const char* text,
			  SDL_Color color) {
	int ch_x = x;
	int ch_y = y;

	int ch;
	for (const char* ptr = text; ch = (unsigned char)*ptr; ptr++) {
		const int chars_in_row = 16;

		SDL_Rect src;
		src.x = ((ch - 32) % chars_in_row) * 8;
		src.y = ((ch - 32) / chars_in_row) * 8;
		src.w = 8;
		src.h = 8;

		SDL_Rect dest;
		dest.x = ch_x;
		dest.y = ch_y;
		dest.w = 8;
		dest.h = 8;

		if (ch != ' ') {
			SDL_SetTextureColorMod(game->tex_font, color.r, color.g, color.b);
			SDL_SetTextureAlphaMod(game->tex_font, color.a);
			SDL_RenderCopy(game->renderer, game->tex_font, &src, &dest);
		}

		ch_x += 8;
		if (ch == '\n') {
			ch_y += 8;
			ch_x = x;
		}
	}
}
//*/

void DrawText(u32 font_index, int x, int y,
			  const char* text,
			  SDL_Color color) {
	const Font& font = *GetFont(font_index);

	int ch_x = x;
	int ch_y = y;

	int ch;
	for (const char* ptr = text; ch = (unsigned char)*ptr; ptr++) {
		if (32 <= ch) {
			const Glyph& glyph = font.glyphs[ch - 32];

			SDL_Rect src = glyph.src;

			SDL_Rect dest;
			dest.x = ch_x + glyph.xoffset;
			dest.y = ch_y + glyph.yoffset;
			dest.w = glyph.src.w;
			dest.h = glyph.src.h;

			if (ch != ' ') {
				SDL_SetTextureColorMod(GetTexture(font.texture_index), color.r, color.g, color.b);
				SDL_SetTextureAlphaMod(GetTexture(font.texture_index), color.a);
				SDL_RenderCopy(game->renderer, GetTexture(font.texture_index), &src, &dest);
			}

			ch_x += glyph.advance;
		}

		if (ch == '\n') {
			ch_y += font.lineskip;
			ch_x = x;
		}
	}
}

void DrawNineslice(int x, int y, int w, int h) {
	// top left
	{
		SDL_Rect src = {0, 0, 8, 8};
		SDL_Rect dest = {x, y, 8, 8};
		SDL_RenderCopy(game->renderer, GetTexture(tex_nineslice), &src, &dest);
	}
	// top
	{
		SDL_Rect src = {8, 0, 8, 8};
		SDL_Rect dest = {x + 8, y, w - 16, 8};
		SDL_RenderCopy(game->renderer, GetTexture(tex_nineslice), &src, &dest);
	}
	// top right
	{
		SDL_Rect src = {16, 0, 8, 8};
		SDL_Rect dest = {x + w - 8, y, 8, 8};
		SDL_RenderCopy(game->renderer, GetTexture(tex_nineslice), &src, &dest);
	}
	// left
	{
		SDL_Rect src = {0, 8, 8, 8};
		SDL_Rect dest = {x, y + 8, 8, h - 16};
		SDL_RenderCopy(game->renderer, GetTexture(tex_nineslice), &src, &dest);
	}
	// middle
	{
		SDL_Rect src = {8, 8, 8, 8};
		SDL_Rect dest = {x + 8, y + 8, w - 16, h - 16};
		SDL_RenderCopy(game->renderer, GetTexture(tex_nineslice), &src, &dest);
	}
	// right
	{
		SDL_Rect src = {16, 8, 8, 8};
		SDL_Rect dest = {x + w - 8, y + 8, 8, h - 16};
		SDL_RenderCopy(game->renderer, GetTexture(tex_nineslice), &src, &dest);
	}
	// bottom left
	{
		SDL_Rect src = {0, 16, 8, 8};
		SDL_Rect dest = {x, y + h - 8, 8, 8};
		SDL_RenderCopy(game->renderer, GetTexture(tex_nineslice), &src, &dest);
	}
	// bottom
	{
		SDL_Rect src = {8, 16, 8, 8};
		SDL_Rect dest = {x + 8, y + h - 8, w - 16, 8};
		SDL_RenderCopy(game->renderer, GetTexture(tex_nineslice), &src, &dest);
	}
	// bottom right
	{
		SDL_Rect src = {16, 16, 8, 8};
		SDL_Rect dest = {x + w - 8, y + h - 8, 8, 8};
		SDL_RenderCopy(game->renderer, GetTexture(tex_nineslice), &src, &dest);
	}
}

void PlaySound(u32 sound_index) {
	StopSound(sound_index);

	Mix_Chunk* sound = GetSound(sound_index);
	Mix_PlayChannel(-1, sound, 0);
}

void StopSound(u32 sound_index) {
	Mix_Chunk* sound = GetSound(sound_index);
	int nchannels = Mix_AllocateChannels(-1);
	for (int i = 0; i < nchannels; i++) {
		if (Mix_Playing(i) && Mix_GetChunk(i) == sound) {
			Mix_HaltChannel(i);
		}
	}
}
