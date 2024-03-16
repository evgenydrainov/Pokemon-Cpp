#include "GameData.h"

static const AttackData pikachu_attacks[] = {
	{"����� 1", 25},
	{"����� 2", 25},
	{"����� 3", 25},
};

static const AttackData bulbasaur_attacks[] = {
	{"����� 4", 25},
	{"����� 5", 25},
};

static const AttackData charmander_attacks[] = {
	{"����� 6", 25},
};

static const PokemonData pokemon_data[POKEMON_COUNT] = {
	{"������",     100, 100, pikachu_attacks,    ArrayLength(pikachu_attacks),    ELEMENT_LIGHNING},
	{"����������", 100, 100, bulbasaur_attacks,  ArrayLength(bulbasaur_attacks),  ELEMENT_WATER},
	{"���������",  100, 100, charmander_attacks, ArrayLength(charmander_attacks), ELEMENT_FIRE},
};

const PokemonData* GetPokemonData(PokemonType type) {
	if (0 <= type && type < POKEMON_COUNT) {
		return &pokemon_data[type];
	} else {
		return &pokemon_data[0];
	}
}
