#include <cstdio>
#include <cstring>
#include <memory>

#include "types.hpp"
#include "fixed_length.hpp"
#include "file_io.hpp"

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

bool cmp_str(Str& a, Str& b) {
	for (size_t i = 0; i < b.size(); ++i) {
		if (a[i] != b[i]) throw std::exception();
	}
	return true;
}

int main(int argc, char* argv[]) {
	fixed_len::CodeMap code;
	if (argc == 2) {
		std::map<char, u32> chars;
		Str str;
		if (!load_file(argv[1], str)) printf("Failed!\n");
		Freq_Map freqs = get_char_freq(str);
		code = fixed_len::create(freqs);
		encode(code, str);
		save(code);
		Str str1 = decode(code);
		//cmp_str(str, str1);
		//print_str(str);
		return EXIT_SUCCESS;
	}
	// else
	load(code);
	auto str = decode(code);
	print_str(str);
	return EXIT_SUCCESS;
}
