#pragma once
#include "types.hpp"

namespace fixed_len {

struct FixMap;

void 	 print_codes(FixMap* vars);

FixMap*  create(Freq_Map& freqs);
Str 	 decode(FixMap* vars);
void 	 encode(FixMap* vars, Str& str);

void 	 save(FixMap* vars, const char* out = "output.bin", const char* code = "codes.bin");
FixMap*  load(const char* in  = "output.bin", const char* code = "codes.bin");

};

namespace huffman {

struct HuffMap;

void 	 print_codes(HuffMap* huff);

HuffMap* create(Freq_Map& freqs);
Str 	 decode(HuffMap* huff);
void 	 encode(HuffMap* huff, Str& str);

void 	 save(HuffMap* huff, const char* outf = "huff.bin", const char* codef = "huffc.bin");
HuffMap* load(const char* inf  = "huff.bin", const char* codef = "huffc.bin");

};
