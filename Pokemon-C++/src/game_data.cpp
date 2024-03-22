#include "game_data.h"

#include "mathh.h"
#include <fstream>
#include <sstream>

static PokemonData pokemon_data[POKEMON_COUNT];

void load_pokemon_data_from_file_txt(const char* filepath) {
	std::ifstream f(filepath);
	std::string line;

	std::getline(f, line);
	int pokemon_count = std::stoi(line);

	for (int i = 0; i < pokemon_count; i++) {

		if (i >= POKEMON_COUNT) {
			break;
		}

		std::getline(f, line);
		strncpy(pokemon_data[i].name, line.c_str(), sizeof(pokemon_data[i].name));
		pokemon_data[i].name[sizeof(pokemon_data[i].name) - 1] = 0;

		std::getline(f, line);
		pokemon_data[i].max_hp = std::stof(line);

		std::getline(f, line);
		pokemon_data[i].max_energy = std::stof(line);

		std::getline(f, line);
		pokemon_data[i].element = (Element)std::stoi(line);

		std::getline(f, line);
		pokemon_data[i].attack_count = std::stoi(line);

		for (int j = 0; j < pokemon_data[i].attack_count; j++) {

			if (j >= ArrayLength(pokemon_data[i].attacks)) {
				break;
			}

			std::getline(f, line);
			strncpy(pokemon_data[i].attacks[j].name, line.c_str(), sizeof(pokemon_data[i].attacks[j].name));
			pokemon_data[i].attacks[j].name[sizeof(pokemon_data[i].attacks[j].name) - 1] = 0;

			std::getline(f, line);
			pokemon_data[i].attacks[j].damage = std::stof(line);

			std::getline(f, line);
			pokemon_data[i].attacks[j].energy = std::stof(line);

		}

	}

}

const PokemonData* get_pokemon_data(PokemonID id) {
	int index = clamp((int)id, 0, POKEMON_COUNT - 1);
	const PokemonData* result = &pokemon_data[index];
	return result;
}
