#pragma once
#include "types.hpp"

namespace fixed_len {

struct CodeMap {
	std::map<char, Bit_Set> map;
	u8 bit_width 	= 0; // rozmiar kodu w bitach
	u8 bit_remain 	= 0; // ile bitow z ostatniego bajtu zostalo uzyte 
	std::vector<u8> encoded; // zakodowane bajty
};

void 	print_codes(CodeMap& vars);

CodeMap create(Freq_Map& freqs);
Str 	decode(CodeMap& vars);
void 	encode(CodeMap& vars, Str& str);

void 	save(CodeMap& vars, const char* out = "output.bin", const char* code = "codes.bin");
void 	load(CodeMap& vars, const char* in  = "output.bin", const char* code = "codes.bin");

};
