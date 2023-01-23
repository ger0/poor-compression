#pragma once
#include <cstdint>
#include <memory>
#include <vector>
#include <bitset>
#include <map>

using u8 	= uint_fast8_t;
using u16 	= uint_fast16_t;
using u32 	= uint32_t;
using u64 	= uint64_t;
using i8 	= int_fast8_t;
using i16 	= int_fast16_t;
using i32 	= int32_t;
using i64 	= int64_t;

using ul 	= unsigned long;
using Str 	= std::vector<char>;

constexpr u32 		BITS = 8;
constexpr size_t 	MAX = sizeof(ul) * 8;

using Freq_Map 	= std::map<char, u32>;
using Bit_Set 	= std::bitset<BITS>;
using Code_Map 	= std::map<char, Bit_Set>;
using Bytes 	= std::vector<u8>;

template<typename... T>
using Ptr 		= std::unique_ptr<T...>;
