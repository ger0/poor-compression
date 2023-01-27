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

#include "file_io.hpp"
#include "types.hpp"
#include "compress.hpp"

using namespace std;
using Code_Map 	= std::map<char, Bit_Set>;

// UZYWAC Z ROZWAGA 
#define OFFSET_OF(type, field) ((unsigned long) &(((type*) 0)->field))

struct FixMap {
	u8 bit_width 	= 0; // rozmiar kodu w bitach
	u8 bit_remain 	= 0; // ile bitow z ostatniego bajtu zostalo uzyte 
	Code_Map map; 		 // kodowanie 
	Bytes encoded; 		 // zakodowane bajty
};

char get_char(FixMap* vars, const Bit_Set& set) {
	for (const auto& [chr, bit_val] : vars->map) {
		if (bit_val == set) return chr;
	}
	throw invalid_argument("Code does not exist!");
}

Bit_Set get_code(FixMap* vars, const char& chr) {
	return vars->map.at(chr);
}

template<> FixMap* create(Freq_Map& freqs) {
	FixMap* vars = new FixMap;
	// kod ostateczny
	u32 max_code = freqs.size();
	u32 val = max_code;
	vars->bit_width = ceil(log2(max_code));

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
		vars->map[chr] = Bit_Set(val);
		val--;
	}
	return vars;
}

template<> void print_codes(FixMap* vars) {
	for (auto &it : vars->map) {
		printf("%c: %s\n", it.first, it.second.to_string().c_str());
	}
}

template<> Str decode(FixMap* vars) {
	// wektor charow
	Str str;
	// wektor pojedynczych bitow (jako bajty zeby bylo latwiej iterowac)
	Bytes bit_vec;
	for (const auto &it : vars->encoded) {
		for (u8 i = 1; i <= BITS; ++i) {
			auto val = (it >> (BITS - i)) & 1;
			bit_vec.push_back(val);
		}
	}
	Bit_Set byte;
	auto encoded_bits = bit_vec.size() - 8 + vars->bit_remain;
	// offset - przesuniecie wzgledem bajtu, idziemy od srodka do konca 
	// tak aby zaczac od najwiekszej wartosci
	i32 offset = vars->bit_width - 1;
	for (u32 i = 0; i < encoded_bits; ++i) {
		// caly kod zostal wczytany 
		if (offset < 0) {
			str.push_back(get_char(vars, byte));	
			byte.reset();
			offset = vars->bit_width - 1;
		}
		byte.set(offset, bit_vec[i]);	
		--offset;
	}
	return str;
}

template<> void encode(FixMap* vars, Str& str) {
	// ktory bit z kolei
	size_t iter = 0;
	// string zwierajacy zapis bitowy 
	string bit_string;
	// zapisuje bity do stringa
	for (const auto& ch : str) {
		const auto code_str = get_code(vars, ch).to_string();
		bit_string += code_str.substr(BITS - vars->bit_width);
		iter += vars->bit_width;
	}
	// odczytuje bity ze stringa do bufora
	Bytes enc_u8;
	istringstream bit_stream(bit_string);
	Bit_Set byte;
	while (!bit_stream.eof()) {
		bit_stream >> byte;	
		u8 val = byte.to_ulong();
		enc_u8.push_back(val);
		byte.reset();
	}
	vars->bit_remain = iter % 8;
	enc_u8.back() <<= vars->bit_remain;
	vars->encoded = enc_u8;
}

template<> void save(FixMap* vars, const char* outname, const char* codename) {
	if (vars->encoded.size() == 0) {
		throw new exception();
		return;
	}
	save_file(outname, vars->encoded);

	Bytes code_out;
	// rozmiar kodu w bitach
	code_out.push_back(vars->bit_width);
	// zapisujemy jaka czesc ostatniego bajtu zostala wykorzystana
	code_out.push_back(vars->bit_remain);

    for (const auto& [ch, val] : vars->map) {
    	code_out.push_back(ch);
    	code_out.push_back(val.to_ullong());
    }
	if (!save_file(codename, code_out)) printf("FAILED!\n");
}

template<> FixMap* load(const char* filename, const char* codename) {
	FixMap* vars = new FixMap;
	// wczytywanie zakodowanego pliku
	load_file(filename, vars->encoded);
 	
	// wczytywanie pliku z kodowaniem // znany jest rozmiar
 	Bytes encoded;
 	load_file(codename, encoded);
 	vars->bit_width  = encoded[OFFSET_OF(FixMap, bit_width)];
 	vars->bit_remain = encoded[OFFSET_OF(FixMap, bit_remain)];
 	auto offset = sizeof(vars->bit_width) + sizeof(vars->bit_remain);

 	for (u32 i = offset; i < encoded.size();) {
 		char chr; Bit_Set bit_code;
		chr = encoded[i]; 		++i;
		bit_code = encoded[i]; 	++i;
		vars->map[chr] = bit_code;
 	}
 	return vars;
}
