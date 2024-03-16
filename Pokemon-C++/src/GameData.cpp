#include "GameData.h"

static const AttackData pikachu_attacks[] = {
	{"Атака 1", 25},
	{"Атака 2", 25},
	{"Атака 3", 25},
};

static const AttackData bulbasaur_attacks[] = {
	{"Атака 4", 25},
	{"Атака 5", 25},
};

static const AttackData charmander_attacks[] = {
	{"Атака 6", 25},
};

static const PokemonData pokemon_data[POKEMON_COUNT] = {
	{"Пикачу",     100, 100, pikachu_attacks,    ArrayLength(pikachu_attacks),    ELEMENT_LIGHNING},
	{"Бульбазавр", 100, 100, bulbasaur_attacks,  ArrayLength(bulbasaur_attacks),  ELEMENT_WATER},
	{"Чармандер",  100, 100, charmander_attacks, ArrayLength(charmander_attacks), ELEMENT_FIRE},
};

const PokemonData* GetPokemonData(PokemonType type) {
	if (0 <= type && type < POKEMON_COUNT) {
		return &pokemon_data[type];
	} else {
		return &pokemon_data[0];
	}
}
