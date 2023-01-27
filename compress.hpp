#pragma once
#include "types.hpp"
#define _generic_ template<typename T>

struct HuffMap;
struct FixMap;

_generic_
void print_codes(T*);

_generic_
T*  create(Freq_Map&);

_generic_
Str 	 decode(T*);

_generic_
void 	 encode(T*, Str&);

_generic_
void 	 save(T*, const char*, const char*);

_generic_
T*  load(const char*, const char*);
