#include <bitset>
#include <cstdio>
#include <tuple>
#include <memory>
#include <vector>
#include <map>
#include <cmath>
#include <iostream>
#include <cstring>

#include "types.hpp"
#include "fixed_length.hpp"

using std::map;

void error(const char* str = "") {
	fprintf(stderr, "%s\n", str);
	exit(EXIT_FAILURE);
}

// zwraca listę wszystkich znaków
Str load_file(const char* filename) {
	FILE* f = fopen(filename, "r");
	if (f == nullptr) error("Nullptr");
	if (fseek(f, 0, SEEK_END) != 0) error("Seeking failure");
	const size_t size = ftell(f);
	fseek(f, 0, SEEK_SET);
	Str str(size);
	size_t read_size = fread(str.data(), sizeof(char), size, f);
	if (read_size != size) error("Incorrect size");
	fclose(f);
	return str;
}

// zwraca mapę [znak, ilosc wystapien]
Freq_Map get_char_freq(Str& str) {
	// zliczanie ilosci wystapien poszczegolnych charow
	Freq_Map chars;
	for (char &ch: str) {
		chars[ch];
		chars[ch]++;
	}
	return chars;
}

// wypisuje znaki wraz z ilocią ich wystapień
void print_freqs(Freq_Map& map) {
	for (auto &[ch, freq] : map) {
		printf("%c: %u\n", ch, freq);
	}
}

void print_bits(u8 bits, u8 max) {
	for (u8 i = 0; i < max; ++i) {
		printf("%u", (bits >> i) & 1);
	}
}

void print_str(Str& str) {
	for (const auto& chr: str) {
		printf("%c", chr);
	}
}

int main(int argc, char* argv[]) {
	fixed_len::CodeMap code;
	if (argc == 2) {
		map<char, u32> chars;
		auto str = load_file(argv[1]);
		Freq_Map freqs = get_char_freq(str);
		code = fixed_len::create(freqs);
		encode(code, str);
		save(code);
		print_freqs(freqs);
		print_codes(code);
		str = decode(code);
		print_str(str);
		return EXIT_SUCCESS;
	}
	// else
	load(code);
	print_codes(code);
	auto str = decode(code);
	print_str(str);

	return EXIT_SUCCESS;
}
