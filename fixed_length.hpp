#pragma once
#include "types.hpp"

namespace fixed_len {

struct CodeMap {
	std::map<char, Bit_Set> map;
	// bit size
	u32 max_code 	= 0;
	// rozmiar kodu w bitach
	u8 bit_width 	= 0;
	// ile bitow ostatniego bajtu zostalo wykorzystane
	u8 bit_remain 	= 0;
	// rozmiar calosciowy
	size_t size 	= 0;
	// zakodowane bajty
	std::vector<u8> encoded;
};

void 	print_codes(CodeMap& vars);

CodeMap create(Freq_Map& freqs);
Str 	decode(CodeMap& vars);
void 	encode(CodeMap& vars, Str& str);

void 	save(CodeMap& vars, const char* out = "output.bin", const char* code = "codes.bin");
void 	load(CodeMap& vars, const char* in  = "output.bin", const char* code = "codes.bin");

};
