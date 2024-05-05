#pragma once
#include <stdint.h>
#include <inttypes.h>
#include <vector>
#include <immintrin.h>
#include <sstream>
#include <span>
#include "Structures.h"

class MagmaAVX512 {
public:
	MagmaAVX512(const key& key);
	void encryptTextAVX512(std::span<const byteVectorMagma> src, std::span<byteVectorMagma> dest, bool en)  const;
};
