#pragma once
#include <stdint.h>
#include <inttypes.h>
#include <immintrin.h>
#include <span>
#include "Structures.h"
#include "table4X256.hpp"
#include "table2X65536.hpp"

class MagmaAVX2 {
private:
	halfVectorMagma roundKeys[8][8];
public:
	MagmaAVX2(const key& key);
	void processData(std::span<const byteVectorMagma> src, std::span<byteVectorMagma> dest, bool en)  const;
	void processDataGamma(std::span<const byteVectorMagma> src, std::span<byteVectorMagma> dest, uint64_t iV) const;
};
