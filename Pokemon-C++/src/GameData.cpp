#include "GameData.h"

static const AttackData pikachu_attacks[] = {
	{"Атака 1", 25},
};

static const AttackData bulbasaur_attacks[] = {
	{"Атака 1", 25},
};

static const AttackData charmander_attacks[] = {
	{"Атака 1", 25},
};

static const PokemonData pokemon_data[POKEMON_COUNT] = {
	{"Пикачу",     100, pikachu_attacks,    ArrayLength(pikachu_attacks)},
	{"Бульбазавр", 100, bulbasaur_attacks,  ArrayLength(bulbasaur_attacks)},
	{"Чармандер",  100, charmander_attacks, ArrayLength(charmander_attacks)},
};

const PokemonData* GetPokemonData(PokemonType type) {
	if (0 <= type && type < POKEMON_COUNT) {
		return &pokemon_data[type];
	} else {
		return &pokemon_data[0];
	}
}
