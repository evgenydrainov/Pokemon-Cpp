#include "GameData.h"

static const AttackData pikachu_attacks[] = {
	{"����� 1", 25},
};

static const AttackData bulbasaur_attacks[] = {
	{"����� 1", 25},
};

static const AttackData charmander_attacks[] = {
	{"����� 1", 25},
};

static const PokemonData pokemon_data[POKEMON_COUNT] = {
	{"������",     100, pikachu_attacks,    ArrayLength(pikachu_attacks)},
	{"����������", 100, bulbasaur_attacks,  ArrayLength(bulbasaur_attacks)},
	{"���������",  100, charmander_attacks, ArrayLength(charmander_attacks)},
};

const PokemonData* GetPokemonData(PokemonType type) {
	if (0 <= type && type < POKEMON_COUNT) {
		return &pokemon_data[type];
	} else {
		return &pokemon_data[0];
	}
}
