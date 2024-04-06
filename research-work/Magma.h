#pragma once
#include <stdint.h>
#include <inttypes.h>
#include <algorithm>
#include <vector>
#include <immintrin.h>
#include <sstream>
#include <span>

struct key {
	uint8_t bytes[32];
	explicit key(uint8_t* data) {
		std::copy_n(data, 32, bytes);
	}
};

union halfVectorMagma {
	uint8_t bytes[sizeof(uint32_t)];
	uint32_t vector;
	halfVectorMagma() = default;
	halfVectorMagma(const uint32_t src) : vector{ src } {}
};

union byteVectorMagma {
	struct {
		halfVectorMagma hi, lo;
	};

	halfVectorMagma halfs[2];
	uint8_t bytes[8];
	byteVectorMagma() = default;
	byteVectorMagma(const halfVectorMagma& left, const halfVectorMagma& right) : lo{ left }, hi{ right } {}
	explicit byteVectorMagma(uint8_t* data) {
		std::copy_n(data, 8, bytes);
	}
};

class Magma {
public:
	Magma(const key& key);
	void encryptTextAVX(const std::vector<byteVectorMagma>& data, std::vector<byteVectorMagma>& dest) const;
	void encryptTextAVX2(std::span<byteVectorMagma> src, std::span<byteVectorMagma> dest)  const; 
};
