#pragma once
#include <cstdint>
#include <vector>
#include <bitset>
#include <map>

using u32 	= uint32_t;
using u8 	= uint_fast8_t;
using ul 	= unsigned long;
using Str 	= std::vector<char>;

constexpr u32 		BITS = 8;
constexpr size_t 	MAX = sizeof(ul) * 8;

using Freq_Map 	= std::map<char, u32>;
using Bit_Set 	= std::bitset<BITS>;
using Code_Map 	= std::map<char, Bit_Set>;
