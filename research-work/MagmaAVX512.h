#pragma once
#include <stdint.h>
#include <inttypes.h>
#include <vector>
#include <immintrin.h>
#include <sstream>
#include <span>
#include "Structures.h"

class MagmaAVX512 {
private:
	halfVectorMagma roundKeys[8][16];
public:
	MagmaAVX512(const key& key);
	void processData(std::span<const byteVectorMagma> src, std::span<byteVectorMagma> dest, bool en, bool reg)  const;
};
