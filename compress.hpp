#pragma once
#include "types.hpp"
#define _generic_ template<typename T>

struct HuffMap;
struct FixMap;

// prints all codes used for encoding various characters
_generic_
void print_codes(T*);

// creates a structure used to encrypt the data
// takes character frequency map as an argument
// template function: use either <HuffMap> or <FixMap>
_generic_
T*  create(Freq_Map& freqs);

// returns decoded Str
_generic_
Str 	 decode(T*);

// encodes data from the Str passed as an argument
_generic_
void 	 encode(T*, Str& str);

// saves encoded data from into outputfile and the header from headerfile
_generic_
void 	 save(T*, const char* outputfile, const char* codename);

// loads encoded data from inputfile and the header from headerfile
// template function: use either <HuffMap> or <FixMap>
_generic_
T*  load(const char* inputfile, const char* headerfile);
