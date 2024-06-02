#pragma once
#include <stdint.h>
#include <inttypes.h>
#include <vector>
#include <immintrin.h>
#include <sstream>
#include <span>
#include "Structures.h"

class MagmaAVX2 {
public:
	MagmaAVX2(const key& key);
	void processData(std::span<const byteVectorMagma> src, std::span<byteVectorMagma> dest, bool en, const byteVectorMagma& initVector)  const;
	void processDataGamma(std::span<const byteVectorMagma> src, std::span<byteVectorMagma> dest, bool en, const byteVectorMagma& initVector) const;
};
