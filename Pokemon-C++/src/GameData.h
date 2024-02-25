#pragma once

#include "common.h"

enum PokemonType {
	POKEMON_PIKACHU,
	POKEMON_BULBASAUR,
	POKEMON_CHARMANDER,

	POKEMON_COUNT,
};

struct AttackData {
	const char* name;
	float damage;
	u32 sound_index;
};

struct PokemonData {
	const char* name;
	float max_hp;
	const AttackData* attacks;
	int attack_count;
};

const PokemonData* GetPokemonData(PokemonType type);
