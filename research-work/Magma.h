#pragma once
#include <stdint.h>
#include <inttypes.h>
#include <vector>
#include <immintrin.h>
#include <sstream>
#include <span>

__m256i tTransofrmAVX2(__m256i data);

struct key {
	uint8_t bytes[32];
	explicit key(uint8_t* data);
};

union halfVectorMagma {
	uint8_t bytes[sizeof(uint32_t)];
	uint32_t vector;
	halfVectorMagma() = default;
	halfVectorMagma(const uint32_t src);
};

union byteVectorMagma {
	struct {
		halfVectorMagma lo, hi;
	};
	struct {
		uint16_t l0, l1, l2, l3;
	};
	halfVectorMagma halfs[2];
	uint8_t bytes[8];
	uint64_t ull;
	byteVectorMagma() = default;
	byteVectorMagma(const halfVectorMagma& left, const halfVectorMagma& right); //+
	byteVectorMagma(uint16_t l0, uint16_t l1, uint16_t l2, uint16_t l3);
	explicit byteVectorMagma(uint8_t* data); // +
};

class Magma {
public:
	Magma(const key& key);
	void encryptTextAVX2(std::span<const byteVectorMagma> src, std::span<byteVectorMagma> dest, bool en)  const;
};
