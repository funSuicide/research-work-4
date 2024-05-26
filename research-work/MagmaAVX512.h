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
	//__m512i tableByteT[4][4];
	halfVectorMagma roundKeys[8][16];

	

	inline void transformationGaVX(__m512i& loHalfs, __m512i& hiHalfs, const halfVectorMagma* roundKeyAddr) const;
	inline __m512i gTransformationAVX(const halfVectorMagma* roundKeyAddr, const __m512i data) const;
	inline __m512i tTransformInRegistersAVX512(__m512i data) const;
	inline void encryptEightBlocks(__m512i& loHalfs, __m512i& hiHalfs) const;
	inline void decryptEightBlocks(__m512i& loHalfs, __m512i& hiHalfs) const;
	inline __m512i test(__m512i data) const;
public:
	MagmaAVX512(const key& key);
	void encryptTextAVX512(std::span<const byteVectorMagma> src, std::span<byteVectorMagma> dest, bool en)  const;
};
