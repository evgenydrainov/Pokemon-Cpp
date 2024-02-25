#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

struct Glyph {
	SDL_Rect src;
	int xoffset;
	int yoffset;
	int advance;
};

struct Font {
	SDL_Texture* texture;
	int ptsize;
	int height;
	int ascent;
	int descent;
	int lineskip;
	Glyph* glyphs; // 32..255
};

bool LoadFontFromFileTTF(SDL_Renderer* renderer,
						 Font* font, const char* fname,
						 int ptsize, int style) {
	bool error = false;

	TTF_Font* ttf_font = nullptr;
	SDL_Surface* atlas_surf = nullptr;

	{
		ttf_font = TTF_OpenFont(fname, ptsize);

		if (!ttf_font) {
			error = true;
			goto out;
		}

		TTF_SetFontStyle(ttf_font, style);
		// TTF_SetFontHinting(ttf_font, TTF_HINTING_NONE);

		font->ptsize = ptsize;
		font->glyphs = (Glyph*) calloc(95, sizeof(*font->glyphs));

		if (!font->glyphs) {
			error = true;
			goto out;
		}

		int atlas_width = 256;
		int atlas_height = 256;

		atlas_surf = SDL_CreateRGBSurfaceWithFormat(0, atlas_width, atlas_height, 32, SDL_PIXELFORMAT_ARGB8888);

		if (!atlas_surf) {
			error = true;
			goto out;
		}

		font->height = TTF_FontHeight(ttf_font);
		font->ascent = TTF_FontAscent(ttf_font);
		font->descent = TTF_FontDescent(ttf_font);
		font->lineskip = TTF_FontLineSkip(ttf_font);

		{
			int minx;
			int maxx;
			int miny;
			int maxy;
			int advance;
			TTF_GlyphMetrics(ttf_font, ' ', &minx, &maxx, &miny, &maxy, &advance);

			font->glyphs[0].advance = advance;
		}

		int x = 0;
		int y = 0;

		for (int ch = 33; ch <= 126; ch++) {
			SDL_Surface* glyph_surf = TTF_RenderGlyph_Blended(ttf_font, ch, {255, 255, 255, 255});

			//SDL_GetPixelFormatName(glyph_surf->format->format);

			int minx;
			int maxx;
			int miny;
			int maxy;
			int advance;
			TTF_GlyphMetrics(ttf_font, ch, &minx, &maxx, &miny, &maxy, &advance);

			if (x + maxx - minx > atlas_surf->w) {
				y += font->lineskip;
				x = 0;
			}

			SDL_Rect src{minx, font->ascent - maxy, maxx - minx, maxy - miny};
			SDL_Rect dest{x, y, maxx - minx, maxy - miny};
			SDL_BlitSurface(glyph_surf, &src, atlas_surf, &dest);

			int w = maxx - minx;
			int h = maxy - miny;
			int xoffset = minx;
			int yoffset = font->ascent - maxy;

			font->glyphs[ch - 32].src = {x, y, w, h};
			font->glyphs[ch - 32].xoffset = xoffset;
			font->glyphs[ch - 32].yoffset = yoffset;
			font->glyphs[ch - 32].advance = advance;

			x += maxx - minx;

			SDL_FreeSurface(glyph_surf);
		}

		// font->texture = SDL_CreateTextureFromSurface(renderer, atlas_surf);
		// 
		// if (!font->texture) {
		// 	error = true;
		// 	goto out;
		// }
	}

	IMG_SavePNG(atlas_surf, "out.png");

out:
	if (atlas_surf) SDL_FreeSurface(atlas_surf);
	if (ttf_font) TTF_CloseFont(ttf_font);

	return !error;
}

Glyph glyphs[224];

int main2(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();

	// SDL_Window* window = SDL_CreateWindow("", 0, 0, 640, 480, 0);
	// SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	// Font font;
	// LoadFontFromFileTTF(nullptr, &font, "msgothic.ttc", 9, 0);

	//*
	std::ifstream f("out.fnt");
	std::string line;
	while (std::getline(f, line)) {
		std::stringstream stream(line);

		std::string a;
		stream >> a;

		if (a == "char") {
			stream >> a;
			int id = std::stoi(a.substr(sizeof("id")));

			// unicode to codepage 1251

			if (1040 <= id && id <= 1103) {
				id = 0xC0 + (id - 1040);
			} else if (id == 1025) {
				id = 0xA8;
			} else if (id == 1105) {
				id = 0xB8;
			}

			stream >> a;
			int x = std::stoi(a.substr(sizeof("x")));

			stream >> a;
			int y = std::stoi(a.substr(sizeof("y")));

			stream >> a;
			int width = std::stoi(a.substr(sizeof("width")));

			stream >> a;
			int height = std::stoi(a.substr(sizeof("height")));

			stream >> a;
			int xoffset = std::stoi(a.substr(sizeof("xoffset")));

			stream >> a;
			int yoffset = std::stoi(a.substr(sizeof("yoffset")));

			stream >> a;
			int xadvance = std::stoi(a.substr(sizeof("xadvance")));

			glyphs[id - 32].src = {x, y, width, height};
			glyphs[id - 32].xoffset = xoffset;
			glyphs[id - 32].yoffset = yoffset;
			glyphs[id - 32].advance = xadvance;

		}
	}
	//*/

	/*
	for (int ch = 32; ch <= 255; ch++) {
		Glyph& g = glyphs[ch - 32];
		g.src = {((ch - 32) % 16) * 8, ((ch - 32) / 16) * 8, 8, 8};
		g.xoffset = 0;
		g.yoffset = 0;
		g.advance = 8;
	}
	//*/

	std::cout << "Glyph glyphs[224] = {\n";
	for (int ch = 32; ch <= 255; ch++) {
		Glyph& g = glyphs[ch - 32];
		printf("\t{{%4d, %4d, %4d, %4d}, %4d, %4d, %4d},\n",
			   g.src.x, g.src.y, g.src.w, g.src.h, g.xoffset, g.yoffset, g.advance);
	}
	std::cout << "};\n";

	return 0;
}
