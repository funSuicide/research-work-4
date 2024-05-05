#include "MagmaAVX512.h"
#include <iostream>
#include "table4X256.hpp"
#include "table2X65536.hpp"

halfVectorMagma roundKeys[8][16];

void expandKeysAVX512(const key& key);

MagmaAVX512::MagmaAVX512(const key& key) {
	expandKeysAVX512(key);
}

void expandKeysAVX512(const key& key)
{
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 16; ++j)
		{
			roundKeys[i][j].vector = reinterpret_cast<const uint32_t*>(key.bytes)[i];
		}
	}
}

__m512i invBytes(__m512i data)
{
	uint32_t mask[] = { 0x0010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F, 0x0010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F, 0x0010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F, 0x0010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F };
	__m512i mask2 = _mm512_load_epi32((const __m512i*)mask);
	return _mm512_shuffle_epi8(data, mask2);
}


__m512i tTransofrmAVX512(__m512i data)
{
	const int loMask = 0x2;
	const int hiMask = 0x1;

	std::vector<byteVectorMagma> src = { {0, 1, 2, 3}, {4, 5, 6, 7}, {8,9,10,11}, {12, 13,14,15}, {16, 17, 18, 19}, {20, 21, 22, 23}, {24, 25, 26, 27}, {28, 29, 30, 31}, {32, 33, 34, 35}, {36, 37, 38, 39}, {40, 41, 42, 43}, {44, 45, 46, 47}, {48, 49, 50, 51}, {52, 53, 54, 55}, {56, 57, 58, 59}, {60, 61, 62, 63} };
	__m512i test16 = _mm512_load_epi32((const __m512i*)(src.data())); 

	__m512i divTmp11 = _mm512_shufflehi_epi16(test16, 0xD8);
	__m512i divTmp12 = _mm512_shufflelo_epi16(divTmp11, 0xD8);

	__m512i divTmp21 = _mm512_shuffle_epi32(divTmp12, 0xD8);
	__m512i divTmp22 = _mm512_shuffle_epi32(divTmp12, 0x8D);

	/*
	__m128i divTmp31 = _mm256_extracti128_si256(divTmp21, hiMask);
	__m128i divTmp32 = _mm256_extracti128_si256(divTmp21, loMask);
	__m128i divTmp33 = _mm256_extracti128_si256(divTmp22, hiMask);
	__m128i divTmp34 = _mm256_extracti128_si256(divTmp22, loMask);

	__m128i hi = _mm_blend_epi32(divTmp31, divTmp34, 0x3);
	__m128i lo = _mm_blend_epi32(divTmp33, divTmp32, 0x3);

	__m256i expandedLo = _mm256_cvtepu16_epi32(lo);
	__m256i expandedHi = _mm256_cvtepu16_epi32(hi);

	__m256i resultLo = _mm256_i32gather_epi32((int const*)sTable2x65536[0], expandedLo, 2);
	__m256i resultHi = _mm256_i32gather_epi32((int const*)sTable2x65536[1], expandedHi, 2);

	__m256i recTmp11 = _mm256_shufflehi_epi16(resultHi, 0xB1);
	__m256i recTmp12 = _mm256_shufflelo_epi16(recTmp11, 0xB1);

	__m256i result = _mm256_blend_epi16(recTmp12, resultLo, 0x5555);
	*/
	return test16;
}

__m512i cyclicShift11(__m512i data)
{
	/*
	* Function cyclic shift left on 11
	*/
	__m512i resultShift = _mm512_slli_epi32(data, 11);
	__m512i resultShift2 = _mm512_srli_epi32(data, 21);
	return _mm512_xor_epi32(resultShift, resultShift2);
}

__m512i gTransformationAVX(halfVectorMagma* roundKeyAddr, const __m512i data)
{
	__m512i vectorKey = _mm512_load_epi32((const __m512i*)roundKeyAddr);

	__m512i invData = invBytes(data);
	vectorKey = invBytes(vectorKey);

	__m512i result = _mm512_add_epi32(vectorKey, invData);

	result = tTransofrmAVX512(result);

	result = cyclicShift11(result);

	return result;
}

inline void transformationGaVX(__m512i& loHalfs, __m512i& hiHalfs, halfVectorMagma* roundKeyAddr) //inline +
{
	__m512i gResult = gTransformationAVX(roundKeyAddr, loHalfs);
	__m512i tmp = _mm512_xor_epi32(invBytes(gResult), hiHalfs);
	hiHalfs = loHalfs;
	loHalfs = tmp;
}

inline void encryptEightBlocks(__m512i& loHalfs, __m512i& hiHalfs)
{
	for (size_t i = 0; i < 24; i++)
	{
		transformationGaVX(loHalfs, hiHalfs, roundKeys[i % 8]);
	}
	for (size_t i = 0; i < 7; i++)
	{
		transformationGaVX(loHalfs, hiHalfs, roundKeys[7 - i]);
	}
	__m512i gResults = gTransformationAVX(roundKeys[0], loHalfs);
	__m512i tmp = _mm512_xor_epi32(invBytes(gResults), hiHalfs);
	hiHalfs = tmp;
}

inline void decryptEightBlocks(__m512i& loHalfs, __m512i& hiHalfs)
{
	for (size_t i = 0; i < 8; i++)
	{
		transformationGaVX(loHalfs, hiHalfs, roundKeys[i]);
	}
	for (size_t i = 0; i < 23; i++)
	{
		transformationGaVX(loHalfs, hiHalfs, roundKeys[7 - (i % 8)]);
	}
	__m512i gResults = gTransformationAVX(roundKeys[0], loHalfs);
	__m512i tmp = _mm512_xor_epi32(invBytes(gResults), hiHalfs);
	hiHalfs = tmp;
}

void MagmaAVX512::encryptTextAVX512(std::span<const byteVectorMagma> src, std::span<byteVectorMagma> dest, bool en) const
{
	const int blockMask = 0xB1;
	const int hiHalfsMask = 0xAA; 
	const int loHalfsMask = 0x55;
	for (size_t b = 0; b < src.size(); b += 16)
	{
		__m512i blocks1 = _mm512_load_epi32((const __m512i*)(src.data() + b));
		__m512i blocks2 = _mm512_load_epi32((const __m512i*)(src.data() + b + 8));

		__m512i blocks1Tmp = _mm512_shuffle_epi32(blocks1, (_MM_PERM_ENUM)blockMask);
		__m512i blocks2Tmp = _mm512_shuffle_epi32(blocks2, (_MM_PERM_ENUM)blockMask);

		__m512i loHalfs = _mm512_mask_blend_epi32(loHalfsMask, blocks1, blocks2Tmp);
		__m512i hiHalfs = _mm512_mask_blend_epi32(hiHalfsMask, blocks1Tmp, blocks2);

		
		if (en)
		{
			encryptEightBlocks(loHalfs, hiHalfs);
		}
		else {
			decryptEightBlocks(loHalfs, hiHalfs);
		}

		__m512i tmp = _mm512_shuffle_epi32(hiHalfs, (_MM_PERM_ENUM)blockMask);
		__m512i tmp2 = _mm512_shuffle_epi32(loHalfs, (_MM_PERM_ENUM)blockMask);

		blocks1 = _mm512_mask_blend_epi32(loHalfsMask, loHalfs, tmp);
		blocks2 = _mm512_mask_blend_epi32(loHalfsMask, tmp2, hiHalfs);

		_mm512_storeu_epi32((__m512i*)(dest.data() + b), blocks1);
		_mm512_storeu_epi32((__m512i*)(dest.data() + b + 8), blocks2);
	}
}

