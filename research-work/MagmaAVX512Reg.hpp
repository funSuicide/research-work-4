#pragma once
#include <stdint.h>
#include <inttypes.h>
#include <immintrin.h>
#include <span>
#include "Structures.h"

class MagmaAVX512Reg {
private:
	halfVectorMagma roundKeys[8][16];
public:
	MagmaAVX512Reg(const key& key);
	void processData(std::span<const byteVectorMagma> src, std::span<byteVectorMagma> dest, bool en)  const;
	void processDataGamma(std::span<const byteVectorMagma> src, std::span<byteVectorMagma> dest, uint64_t iV) const;
};
