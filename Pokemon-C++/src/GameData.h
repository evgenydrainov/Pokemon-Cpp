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

enum Element {
	ELEMENT_LIGHNING,
	ELEMENT_WATER,
	ELEMENT_FIRE,

	ELEMENT_COUNT,
};

struct PokemonData {
	const char* name;
	float max_hp;
	float max_energy;
	const AttackData* attacks;
	int attack_count;
	Element element;
};

const PokemonData* GetPokemonData(PokemonType type);
