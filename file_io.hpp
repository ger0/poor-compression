#pragma once
#include "types.hpp"

static auto file_destroy = [](FILE* file) {
	fclose(file);
};

template<typename T>
bool save_file(const char* filename, std::vector<T>& buff) {
	Ptr<FILE, decltype(file_destroy)> f(fopen(filename, "wb"), file_destroy);
	if (f.get() == nullptr) return false;
	size_t write_size = fwrite(buff.data(), sizeof(T), buff.size(), f.get());
	if (write_size != buff.size()) return false;
	return true;
}

template<typename T>
bool load_file(const char* filename, std::vector<T>& buff) {
	std::unique_ptr<FILE, decltype(file_destroy)> f(fopen(filename, "rb"), file_destroy);
	if (f.get() == nullptr) return false;
	if (fseek(f.get(), 0, SEEK_END) != 0) return false;
	const size_t size = ftell(f.get());
	fseek(f.get(), 0, SEEK_SET);
	buff.resize(size);
	size_t read_size = fread(buff.data(), sizeof(T), buff.size(), f.get());
	if (read_size != size) return false;
	return true;
}
