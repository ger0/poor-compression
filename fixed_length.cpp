#include <bitset>
#include <cstdio>
#include <tuple>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <cstring>
#include <sstream>

#include "types.hpp"
#include "fixed_length.hpp"

using namespace std;
namespace fixed_len {

char get_char(CodeMap& vars, const Bit_Set& set) {
	for (const auto& [chr, bit_val] : vars.map) {
		if (bit_val == set) return chr;
	}
	throw invalid_argument("Code does not exist!");
}

Bit_Set get_code(CodeMap& vars, const char& chr) {
	return vars.map.at(chr);
}

CodeMap create(Freq_Map& freqs) {
	CodeMap vars;
	// kod ostateczny
	vars.max_code = freqs.size();
	u32 val = vars.max_code;

	// sortowanie wzgledem liczby wystapien
	using entry = pair<char, u32>;
	vector<entry> sorted;
	
	for (const auto& it : freqs) {
		sorted.push_back(it);
	}

	sort(sorted.begin(), sorted.end(), [&](entry& a, entry& b) {
			return a.second > b.second;
			});

	for (const auto& [chr, btst] : sorted) {
		vars.map[chr] = Bit_Set(val);
		val--;
	}
	return vars;
}

void print_codes(CodeMap& vars) {
	for (auto &it : vars.map) {
		printf("%c: %s\n", it.first, it.second.to_string().c_str());
	}
}

Str decode(CodeMap& vars) {
	// wektor charow
	Str str;
	// wektor pojedynczych bitow (jako bajty zeby bylo latwiej iterowac)
	vector<u8> bit_vec;
	for (const auto &it : vars.encoded) {
		for (u8 i = 1; i <= BITS; ++i) {
			auto val = (it >> (BITS - i)) & 1;
			bit_vec.push_back(val);
		}
	}
	Bit_Set byte;
	auto encoded_bits = bit_vec.size() - 8 + vars.bit_remain;
	// offset - przesuniecie wzgledem bajtu, idziemy od srodka do konca 
	// tak aby zaczac od najwiekszej wartosci
	int offset = vars.bit_width - 1;
	for (u32 i = 0; i < encoded_bits; ++i) {
		// caly kod zostal wczytany 
		if (offset < 0) {
			str.push_back(get_char(vars, byte));	
			byte.reset();
			offset = vars.bit_width - 1;
		}
		byte.set(offset, bit_vec[i]);	
		--offset;
	}
	return str;
}

void encode(CodeMap& vars, Str& str) {
	vars.bit_width = log2(vars.max_code) + 1;
	// ktory bit z kolei
	size_t iter = 0;
	// string zwierajacy zapis bitowy 
	string bit_string;
	// zapisuje bity do stringa
	for (const auto& ch : str) {
		const auto code_str = get_code(vars, ch).to_string();
		bit_string += code_str.substr(BITS - vars.bit_width);
		iter += vars.bit_width;
	}
	// odczytuje bity ze stringa do bufora
	vector<u8> enc_u8;
	istringstream bit_stream(bit_string);
	Bit_Set byte;
	while (!bit_stream.eof()) {
		bit_stream >> byte;	
		u8 val = byte.to_ulong();
		enc_u8.push_back(val);
		byte.reset();
	}
	vars.bit_remain = iter % 8;
	enc_u8.back() <<= vars.bit_remain;
	vars.encoded = enc_u8;
}

void save(CodeMap& vars, const char* outname, const char* codename) {
	if (vars.encoded.size() == 0) {
		throw new exception();
		return;
	}
	FILE* f = fopen(outname, "wb");
	// zapisujemy zakodowany tekst
	fwrite(vars.encoded.data(), sizeof(u8), vars.encoded.size(), f);
	fclose(f);

	f = fopen(codename, "wb");
	// bit_width
	fwrite(&vars.bit_width,  sizeof(vars.bit_width), 1, f);
	// zapisujemy jaka czesc ostatniego bajtu zostala wykorzystana
	fwrite(&vars.bit_remain,  sizeof(vars.bit_remain), 1, f);
	for (const auto& it : vars.map) {
		fwrite(&it.first,  sizeof(char), 1, f);
		fwrite(&it.second, ceil(BITS / 8.f), 1, f);
	}
	fclose(f);
}

void load(CodeMap& vars, const char* filename, const char* codename) {
	// wczytywanie zakodowanego pliku
	FILE* f = fopen(filename, "r");
	if (f == nullptr) throw "File does not exist!";

	if (fseek(f, 0, SEEK_END) != 0) throw exception();
	vars.size = ftell(f);
	fseek(f, 0, SEEK_SET);
	vars.encoded = vector<u8>(vars.size);
	size_t read_size = fread(vars.encoded.data(), sizeof(u8), vars.size, f);
	if (read_size != vars.size) throw "Incorrect size loaded!";
	fclose(f);

	// wczytywanie pliku z kodowaniem
	f = fopen(codename, "rb");
	if (f == nullptr) throw "File does not exist";
	if (fseek(f, 0, SEEK_END) != 0) throw exception();
	auto code_size = ftell(f);
	fseek(f, 0, SEEK_SET);
	// wczytywanie wykorzystywanej czesci ostatniego bajtu 
	fread(&vars.bit_width, sizeof(vars.bit_width), 1, f);
	fread(&vars.bit_remain, sizeof(vars.bit_remain), 1, f);
	while (ftell(f) < code_size) {
		char chr; Bit_Set bit_code;
		fread(&chr, sizeof(chr), 1, f);
		fread(&bit_code, ceil(BITS / 8.f), 1, f);
		vars.map[chr] = bit_code;
	}
	fclose(f);
}

};
