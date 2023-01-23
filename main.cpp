#include <cstdio>
#include <cstring>
#include <memory>

#include "types.hpp"
#include "compress.hpp"
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
	printf("\n");
}

bool cmp_str(Str& a, Str& b) {
	for (size_t i = 0; i < b.size(); ++i) {
		if (a[i] != b[i]) {
			printf("\n%c!=%c\n", a[i], b[i]);
			throw std::exception();
		}
	}
	return true;
}

int main(int argc, char* argv[]) {
	if (argc == 2) {
		std::map<char, u32> chars;
		Str str;
		if (!load_file(argv[1], str)) printf("Failed!\n");
		Freq_Map freqs = get_char_freq(str);

		auto huff = huffman::create(freqs);
		encode(huff, str);
		save(huff);

		//auto str1 = decode(huff);
		return EXIT_SUCCESS;
	}
	// else
	auto huff = huffman::load();
	auto str = huffman::decode(huff);
	print_str(str);
	return EXIT_SUCCESS;
}
