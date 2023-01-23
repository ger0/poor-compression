#include <bitset>
#include <cstdio>
#include <tuple>
#include <memory>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <cstring>
#include <sstream>

#include "file_io.hpp"
#include "types.hpp"
#include "compress.hpp"

using namespace std;
namespace huffman {

using ch_freq = pair<char, u32>;
using bits = string;
using Code_Map = unordered_map<char, bits>;
using Char_Map = unordered_map<bits, char>;

struct Node;

struct HuffMap {
	u8 				last_bits;
	Ptr<Node> 		tree;
	vector<Node*> 	nodes;

	Code_Map 		codes;
	Char_Map 		chars;

	Bytes 			encoded;
};

// Drzewo
struct Node {
	char 		chr 	= 0;
	u32 		freq 	= 0;
	Ptr<Node> 	left;
	Ptr<Node> 	right;
};


// UZYWAC Z ROZWAGA 
#define OFFSET_OF(type, field) ((unsigned long) &(((type*) 0)->field))

bool freq_cmp(Node* a, Node* b) {
	return a->freq > b->freq;
}

Node* construct_tree(vector<Node*>& nodes) {
	// sortowanie 
	sort(begin(nodes), end(nodes), freq_cmp);

	Node *root, *left, *right;

	while (nodes.size() >= 2) {
		left 	= *(nodes.end() - 1);
		right 	= *(nodes.end() - 2);
		// kasowanie z tablicy
		nodes.pop_back(); nodes.pop_back();
		// scalanie lewego i prawego
		root 	= new Node{
			.freq  = left->freq + right->freq,
			.left  = Ptr<Node>(left),
			.right = Ptr<Node>(right)
		};
		// dodawanie kolejnego
		nodes.push_back(root);
		// sortowanie
		sort(begin(nodes), end(nodes), freq_cmp);
	}
	if (nodes.size() == 1) {
		root = nodes.back();
		nodes.pop_back();
		return root;
	}
	return nullptr;
}

void cache_chars(HuffMap* huff) {
	for (const auto& [chr, code] : huff->codes) {
		huff->chars[code] = chr;
	}
}

void cache_codes(Node* node, bits code, HuffMap* huff) {
	if (node == nullptr) {
		return;
	}
	// zapisujemy do cache
	if (node->chr != 0) {
		if (code == "") 	code = '0';
		huff->codes[node->chr] = code;
	}
	cache_codes(node->left.get(), 	code + '0', huff);
	cache_codes(node->right.get(), 	code + '1', huff);
}

HuffMap* create(Freq_Map& freqs) {
	HuffMap* huff = new HuffMap;
	// wypelniamy tablice
	for (const auto& [chr, freq]: freqs) {
		huff->nodes.push_back(
				new Node{.chr = chr, .freq = freq}
				);
	}
	// tworzymy drzewo
	huff->tree = Ptr<Node>(construct_tree(huff->nodes));
	// cache'ujemy wartosci znak - kod
	// wartosc poczatkowa kodu to 0
	cache_codes(huff->tree.get(), "", huff);
	return huff;
}
void print_codes(HuffMap* huff) {
	for (const auto& [chr, code]: huff->codes) {
		printf(":%c: %s\n", chr, code.c_str());
	}
}

bits get_code(HuffMap* huff, char chr) {
	const auto& code = huff->codes.find(chr);
	if (code == huff->codes.end()) return "";
	else return code->second;
}

char get_char(HuffMap* huff, bits code) {
	const auto& it = huff->chars.find(code);
	if (it == huff->chars.end()) return 0;
	else return it->second;
}

void remove_leading_zeros(bits& bit_str) {
	bit_str.erase(0, std::min(bit_str.find_first_not_of('0'), bit_str.size()-1));
}
	
Str decode(HuffMap* huff) {
	// ulatwienie przeszukiwania
	cache_chars(huff);
	// wektor charow
	Str str;

	string bit_code;
	for (const auto& it : huff->encoded) {
		auto bit_str = bitset<8>(it).to_string();
		bit_code += bit_str;
	}
	// kasujemy nadmiar bitow ktore nie sa wykorzystywane
	bit_code.assign(bit_code.begin(), bit_code.end() - huff->last_bits);

	bits encoded;
	for (const auto& it : bit_code) {
		encoded += it;
		char chr = get_char(huff, encoded);
		if (chr) {
			str.push_back(chr);
			encoded.clear();
		}
	}
	return str;
}

void encode(HuffMap* huff, Str& str) {
	// string zwierajacy zapis bitowy 
	bits bit_string;
	// zapisuje bity do stringa
	for (const auto& ch : str) {
		const auto code_str = get_code(huff, ch);
		bit_string += code_str;
	}
	huff->last_bits = BITS - bit_string.size() % BITS;

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
	// przesuwamy bity aby wypelnic pozycje o wiekszych wartosciach liczbowych
	enc_u8.back() <<= huff->last_bits;
	huff->encoded = enc_u8;
}

void save(HuffMap* huff, const char* outf, const char* codef) {
	if (huff->encoded.size() == 0) {
		throw new exception();
		return;
	}
	save_file(outf, huff->encoded);

	Bytes code_out;
	code_out.push_back(huff->last_bits);
    for (const auto& [ch, val] : huff->codes) {
    	// dlugosc kodu max 255?
    	u8 length = val.size();
		auto temp = Bytes(length);

    	code_out.push_back(ch); 		// znak
    	code_out.push_back(length); 	// liczba bajtow na kod
    	auto bit_set = bitset<256>(val.c_str());

    	ul bits_data = bit_set.to_ulong();
    	memcpy(temp.data(), &bits_data, length);

    	code_out.insert(code_out.end(), begin(temp), end(temp));
    }
	if (!save_file(codef, code_out)) printf("FAILED!\n");
}

HuffMap* load(const char* inf, const char* codef) {
	HuffMap* huff = new HuffMap;
	// wczytywanie zakodowanego pliku
	load_file(inf, huff->encoded);
 	
	// wczytywanie pliku z kodowaniem // znany jest rozmiar
 	Bytes encoded;
 	load_file(codef, encoded);

 	u64 i = 0;
 	huff->last_bits = encoded[i]; ++i;
 	while (i < encoded.size()) {
		char chr 	= encoded[i]; ++i;

		u8 length 	= encoded[i]; ++i;
		u8 byte_len = length / BITS + 1;

		auto temp 	= Bytes(byte_len);
		ul value 	= 0;
		memcpy(&value, encoded.data() + i, length); i += length;

		auto bit_code = bitset<256>(value); 
		bits bit_str = bit_code.to_string();

		// kasujemy leading zera
		//remove_leading_zeros(bit_str);
		bit_str = bit_str.assign(bit_str.end() - length, bit_str.end());
		huff->codes[chr] = bit_str;
 	}
 	return huff;
}

};
