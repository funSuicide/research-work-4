#include "Magma.h"
#include <iostream>
#include "table4X256.hpp"
#include "table2X65536.hpp"

// Заменить на другую стуктуру или 8x8; //вместо broadcast +
halfVectorMagma roundKeys[8][8];

key::key(uint8_t* data)
{
	std::copy_n(data, 32, bytes);
}

halfVectorMagma::halfVectorMagma(const uint32_t src) : vector{ src } {}

byteVectorMagma::byteVectorMagma(const halfVectorMagma& left, const halfVectorMagma& right) : lo{ left }, hi{ right } {}

byteVectorMagma::byteVectorMagma(uint16_t l0, uint16_t l1, uint16_t l2, uint16_t l3) : l0{ l0 }, l1{ l1 }, l2{ l2 }, l3{ l3 } {}
byteVectorMagma::byteVectorMagma(uint8_t* data)
{
	std::copy_n(data, 8, bytes);
}

void expandKeys(const key& key);

Magma::Magma(const key& key) {
	expandKeys(key);
}

void expandKeys(const key& key)
{
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			roundKeys[i][j].vector = reinterpret_cast<const uint32_t*>(key.bytes)[i];
		}
	}
}

__m256i invBytes(__m256i data)
{
	uint32_t mask = 0x10203;
	__m256i mask2 = _mm256_set1_epi32(mask);
	return _mm256_shuffle_epi8(data, mask2);
}


__m256i tTransofrmAVX2(__m256i data)
{
	/*
	* Function transformation T.
	* don't work (hi halfs 16-bit), sTable2x65535 is not correct
	*/
	const int loMask = 0x2;
	const int hiMask = 0x1;

	//std::vector<byteVectorMagma> src = { {0, 1, 2, 3}, {4, 5, 6, 7}, {8,9,10,11}, {12, 13,14,15}, {16, 17, 18, 19}, {20, 21, 22, 23}, {24, 25, 26, 27}, {28, 29, 30, 31} };
	//__m256i test16 = _mm256_load_si256((const __m256i*)(src.data())); pizdec

	__m256i divTmp11 = _mm256_shufflehi_epi16(data, 0xD8);
	__m256i divTmp12 = _mm256_shufflelo_epi16(divTmp11, 0xD8);

	__m256i divTmp21 = _mm256_shuffle_epi32(divTmp12, 0xD8);
	__m256i divTmp22 = _mm256_shuffle_epi32(divTmp12, 0x8D);

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

	return result;
}

__m256i cyclicShift11(__m256i data)
{
	/*
	* Function cyclic shift left on 11
	*/
	__m256i resultShift = _mm256_slli_epi32(data, 11);
	__m256i resultShift2 = _mm256_srli_epi32(data, 21);
	return _mm256_xor_si256(resultShift, resultShift2);
}

__m256i gTransformationAVX(halfVectorMagma* roundKeyAddr, const __m256i data)
{
	__m256i vectorKey = _mm256_load_si256((const __m256i*)roundKeyAddr);

	__m256i invData = invBytes(data);
	vectorKey = invBytes(vectorKey);

	__m256i result = _mm256_add_epi32(vectorKey, invData);


	result = tTransofrmAVX2(result);

	//result = invBytes(result);

	result = cyclicShift11(result);

	return result;
}

inline void transformationGaVX(__m256i& loHalfs, __m256i& hiHalfs, halfVectorMagma* roundKeyAddr) //inline +
{
	__m256i gResult = gTransformationAVX(roundKeyAddr, loHalfs);
	__m256i tmp = _mm256_xor_si256(invBytes(gResult), hiHalfs);
	hiHalfs = loHalfs;
	loHalfs = tmp;
}

inline void encryptEightBlocks(__m256i& loHalfs, __m256i& hiHalfs)
{
	for (size_t i = 0; i < 24; i++)
	{
		transformationGaVX(loHalfs, hiHalfs, roundKeys[i % 8]);
	}
	for (size_t i = 0; i < 7; i++)
	{
		transformationGaVX(loHalfs, hiHalfs, roundKeys[7 - i]);
	}
	__m256i gResults = gTransformationAVX(roundKeys[0], loHalfs);
	__m256i tmp = _mm256_xor_si256(invBytes(gResults), hiHalfs);
	hiHalfs = tmp;
}

inline void decryptEightBlocks(__m256i& loHalfs, __m256i& hiHalfs)
{
	for (size_t i = 0; i < 8; i++)
	{
		transformationGaVX(loHalfs, hiHalfs, roundKeys[i]);
	}
	for (size_t i = 0; i < 23; i++)
	{
		transformationGaVX(loHalfs, hiHalfs, roundKeys[7 - (i % 8)]);
	}
	__m256i gResults = gTransformationAVX(roundKeys[0], loHalfs);
	__m256i tmp = _mm256_xor_si256(invBytes(gResults), hiHalfs);
	hiHalfs = tmp;
}

void Magma::encryptTextAVX2(std::span<const byteVectorMagma> src, std::span<byteVectorMagma> dest, bool en) const
{
	const int blockMask = 0xB1;
	const int hiHalfsMask = 0xAA;
	const int loHalfsMask = 0x55;
	for (size_t b = 0; b < src.size(); b += 8)
	{
		__m256i blocks1 = _mm256_load_si256((const __m256i*)(src.data() + b));
		__m256i blocks2 = _mm256_load_si256((const __m256i*)(src.data() + b + 4));

		__m256i blocks1Tmp = _mm256_shuffle_epi32(blocks1, blockMask);
		__m256i blocks2Tmp = _mm256_shuffle_epi32(blocks2, blockMask);

		__m256i loHalfs = _mm256_blend_epi32(blocks1, blocks2Tmp, loHalfsMask);
		__m256i hiHalfs = _mm256_blend_epi32(blocks2, blocks1Tmp, hiHalfsMask);

		if (en)
		{
			encryptEightBlocks(loHalfs, hiHalfs);
		}
		else {
			decryptEightBlocks(loHalfs, hiHalfs);
		}

		__m256i tmp = _mm256_shuffle_epi32(hiHalfs, blockMask);
		__m256i tmp2 = _mm256_shuffle_epi32(loHalfs, blockMask);

		blocks1 = _mm256_blend_epi32(loHalfs, tmp, loHalfsMask);
		blocks2 = _mm256_blend_epi32(tmp2, hiHalfs, loHalfsMask);

		_mm256_storeu_si256((__m256i*)(dest.data() + b), blocks1);
		_mm256_storeu_si256((__m256i*)(dest.data() + b + 4), blocks2);
	}
}

