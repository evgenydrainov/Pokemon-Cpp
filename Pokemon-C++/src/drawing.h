#pragma once

#include <SDL.h>
#include <vector>

enum DrawCmdType {
	DRAW_CMD_TEXTURE,
	DRAW_CMD_SET_COLOR,
};

struct DrawCmd {
	DrawCmdType type;

	union {
		struct {
			SDL_Texture* texture;
			SDL_Rect src;
			SDL_Rect dest;
		} texture;

		struct {
			SDL_Color color;
		} set_color;
	};
};

using DrawList = std::vector<DrawCmd>;

int texture_get_width(SDL_Texture* texture);
int texture_get_height(SDL_Texture* texture);

void draw_list_add_texture(DrawList& draw_list, SDL_Texture* texture,
						   SDL_Rect src, SDL_Rect dest);

void draw_list_add_nineslice(DrawList& draw_list, SDL_Texture* texture,
							 int x, int y, int w, int h);

void draw_list_add_set_color(DrawList& draw_list, SDL_Color color);
