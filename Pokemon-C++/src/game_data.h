#pragma once

#include "common.h"

enum PokemonID {
	POKEMON_PIKACHU,
	POKEMON_BULBASAUR,
	POKEMON_CHARMANDER,

	POKEMON_COUNT,
};

enum Element {
	ELEMENT_LIGHNING,
	ELEMENT_WATER,
	ELEMENT_FIRE,

	ELEMENT_COUNT,
};

struct AttackData {
	char name[16];
	float damage;
	float energy;
};

struct PokemonData {
	char name[16];
	float max_hp;
	float max_energy;
	AttackData attacks[3];
	int attack_count;
	Element element;
};

void load_pokemon_data_from_file_txt(const char* filepath);

const PokemonData* get_pokemon_data(PokemonID id);
