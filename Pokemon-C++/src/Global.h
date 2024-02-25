#pragma once

#include "common.h"
#include "Assets.h"
#include <SDL.h>

bool IsKeyPressed(int scancode);

void DrawText(u32 font_index, int x, int y,
			  const char* text,
			  SDL_Color color = {0, 0, 0, 255});

void DrawNineslice(int x, int y, int w, int h);

void PlaySound(u32 sound_index);

void StopSound(u32 sound_index);
