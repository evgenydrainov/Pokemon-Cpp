#include "drawing.h"

int texture_get_width(SDL_Texture* texture) {
	int result;
	SDL_QueryTexture(texture, nullptr, nullptr, &result, nullptr);
	return result;
}

int texture_get_height(SDL_Texture* texture) {
	int result;
	SDL_QueryTexture(texture, nullptr, nullptr, nullptr, &result);
	return result;
}

void draw_list_add_texture(DrawList& draw_list, SDL_Texture* texture,
						   SDL_Rect src, SDL_Rect dest) {
	DrawCmd cmd = {};
	cmd.type = DRAW_CMD_TEXTURE;
	cmd.texture.texture = texture;
	cmd.texture.src = src;
	cmd.texture.dest = dest;

	draw_list.push_back(cmd);
}

void draw_list_add_nineslice(DrawList& draw_list, SDL_Texture* texture,
							 int x, int y, int w, int h) {
	int slice_w = texture_get_width(texture) / 3;
	int slice_h = texture_get_height(texture) / 3;

	// top left
	{
		SDL_Rect src = {0, 0, slice_w, slice_h};
		SDL_Rect dest = {x, y, slice_w, slice_h};
		draw_list_add_texture(draw_list, texture, src, dest);
	}
	// top
	{
		SDL_Rect src = {slice_w, 0, slice_w, slice_h};
		SDL_Rect dest = {x + slice_w, y, w - 2 * slice_w, slice_h};
		draw_list_add_texture(draw_list, texture, src, dest);
	}
	// top right
	{
		SDL_Rect src = {2 * slice_w, 0, slice_w, slice_h};
		SDL_Rect dest = {x + w - slice_w, y, slice_w, slice_h};
		draw_list_add_texture(draw_list, texture, src, dest);
	}
	// left
	{
		SDL_Rect src = {0, slice_h, slice_w, slice_h};
		SDL_Rect dest = {x, y + slice_h, slice_w, h - 2 * slice_h};
		draw_list_add_texture(draw_list, texture, src, dest);
	}
	// middle
	{
		SDL_Rect src = {slice_w, slice_h, slice_w, slice_h};
		SDL_Rect dest = {x + slice_w, y + slice_w, w - 2 * slice_w, h - 2 * slice_h};
		draw_list_add_texture(draw_list, texture, src, dest);
	}
	// right
	{
		SDL_Rect src = {2 * slice_w, slice_h, slice_w, slice_h};
		SDL_Rect dest = {x + w - slice_w, y + slice_h, slice_w, h - 2 * slice_h};
		draw_list_add_texture(draw_list, texture, src, dest);
	}
	// bottom left
	{
		SDL_Rect src = {0, 2 * slice_h, slice_w, slice_h};
		SDL_Rect dest = {x, y + h - slice_h, slice_w, slice_h};
		draw_list_add_texture(draw_list, texture, src, dest);
	}
	// bottom
	{
		SDL_Rect src = {slice_w, 2 * slice_h, slice_w, slice_h};
		SDL_Rect dest = {x + slice_w, y + h - slice_h, w - 2 * slice_w, slice_h};
		draw_list_add_texture(draw_list, texture, src, dest);
	}
	// bottom right
	{
		SDL_Rect src = {2 * slice_w, 2 * slice_h, slice_w, slice_h};
		SDL_Rect dest = {x + w - slice_w, y + h - slice_h, slice_w, slice_h};
		draw_list_add_texture(draw_list, texture, src, dest);
	}
}

void draw_list_add_set_color(DrawList& draw_list, SDL_Color color) {
	DrawCmd cmd = {};
	cmd.type = DRAW_CMD_SET_COLOR;
	cmd.set_color.color = color;

	draw_list.push_back(cmd);
}
