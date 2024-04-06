#include "Magma.h"
#include <iostream>
#include <omp.h>
#include "table4X256.hpp"
#include "table2X65536.hpp"

// Заменить на другую стуктуру или 8x8; //вместо broadcast +
halfVectorMagma roundKeys[8][8];

void expandKeys(const key& key);

Magma::Magma(const key& key) {
	expandKeys(key);
}

void expandKeys(const key& key)
{
	for (int i = 7; i >= 0; --i)
	{
		for (size_t j = 0; j < 8; ++j)
		{
			std::copy(key.bytes + 4 * (7-i), key.bytes + 4 * (7 - i) + 4, roundKeys[i][j].bytes);
		}
	}
}

__m256i gTransformationAVX(float const* roundKeyAddr, const __m256i data)
{	
	const int loMask = 0x2;
	const int hiMask = 0x1;
	const int blendMask = 0x5555; //5555

	__m256i vectorKey = _mm256_castps_si256(_mm256_load_ps(roundKeyAddr));
	__m256i result = _mm256_add_epi32(vectorKey, data);

	__m128i lo = _mm256_extracti128_si256(result, loMask);
	__m128i hi = _mm256_extracti128_si256(result, hiMask);

	__m256i expandedLo = _mm256_cvtepu16_epi32(lo);
	__m256i expandedHi = _mm256_cvtepu16_epi32(hi);
	// _mm256_extracti128_si256, cvt, blend + ???
	//__m256i tmp1 = _mm256_setr_epi16(result.m256i_i16[0], 0, result.m256i_i16[2], 0, result.m256i_i16[4], 0, result.m256i_i16[6], 0, result.m256i_i16[8], 0, result.m256i_i16[10], 0, result.m256i_i16[12], 0, result.m256i_i16[14], 0);
	//__m256i tmp2 = _mm256_setr_epi16(result.m256i_i16[1], 0, result.m256i_i16[3], 0, result.m256i_i16[5], 0, result.m256i_i16[7], 0, result.m256i_i16[9], 0, result.m256i_i16[11], 0, result.m256i_i16[13], 0, result.m256i_i16[15], 0);

	__m256i resultLo = _mm256_castps_si256(_mm256_i32gather_ps((float const*)sTable2x65536, expandedLo, 1));
	__m256i resultHi = _mm256_castps_si256(_mm256_i32gather_ps((float const*)sTable2x65536, expandedHi, 1));
	
	result = _mm256_blend_epi16(resultHi, resultLo, blendMask);
	//result = _mm256_set_m128i(resultHi, resultLo);
	//result = _mm256_setr_epi16(result2.m256i_i16[0], result1.m256i_i16[0], result2.m256i_i16[2], result1.m256i_i16[2], result2.m256i_i16[4], result1.m256i_i16[4], result2.m256i_i16[6], result1.m256i_i16[6], result2.m256i_i16[8], result1.m256i_i16[8], result2.m256i_i16[10], result1.m256i_i16[10], result2.m256i_i16[12], result1.m256i_i16[12], result2.m256i_i16[14], result1.m256i_i16[14]);

	__m128i shift;
	shift.m128i_u64[0] = 11;
	__m256i resultShift = _mm256_sll_epi32(result, shift);
	shift.m128i_u64[0] = 21;
	__m256i resultShift2 = _mm256_srl_epi32(result, shift);
	result = _mm256_xor_si256(resultShift, resultShift2);
	return result;
}

inline void transformationGaVX(__m256i& leftHalfs, __m256i& rightHalfs, float const* roundKeyAddr) //inline +
{
	__m256i gResult = gTransformationAVX((float const*)roundKeyAddr, leftHalfs);
	__m256i tmp = _mm256_xor_si256(gResult, rightHalfs);
	leftHalfs = rightHalfs;
	rightHalfs = tmp;
}

inline void encryptEightBlocks(__m256i& leftHalfs, __m256i& rightHalfs) //inline +
{
	for (size_t i = 0; i < 24; i++)
	{
		transformationGaVX(leftHalfs, rightHalfs, (float const*)roundKeys[i % 8]);
	}
	for (size_t i = 0; i < 7; i++)
	{
		transformationGaVX(leftHalfs, rightHalfs, (float const*)roundKeys[7 - i]);
	}
	__m256i gResults = gTransformationAVX((float const*)roundKeys[0], leftHalfs);
	__m256i tmp = _mm256_xor_si256(gResults, rightHalfs);
	leftHalfs = rightHalfs;
	rightHalfs = tmp;
}

void Magma::encryptTextAVX2(std::span<byteVectorMagma> src, std::span<byteVectorMagma> dest) const
{	
	const int blockMask = 0xB1;
	const int rightHalfsMask = 0x55;
	const int leftHalfsMask = 0xAA;
	for (size_t b = 0; b < src.size_bytes()/8; b += 8) //?
	{
		__m256i blocks1 = _mm256_castps_si256(_mm256_loadu_ps((const float*)(src.data()+b))); //+
		__m256i blocks2 = _mm256_castps_si256(_mm256_loadu_ps((const float*)(src.data() + b + 4))); //+
		
		__m256i blocks1Tmp = _mm256_shuffle_epi32(blocks1, blockMask); //+
		__m256i blocks2Tmp = _mm256_shuffle_epi32(blocks2, blockMask); //+

		__m256i leftHalfs = _mm256_blend_epi32(blocks1, blocks2Tmp, leftHalfsMask); //+
		__m256i rightHalfs = _mm256_blend_epi32(blocks2, blocks1Tmp, rightHalfsMask); //+
		
		encryptEightBlocks(leftHalfs, rightHalfs);

		blocks1 = _mm256_blend_epi32(leftHalfs, rightHalfs, rightHalfsMask);
		blocks2 = _mm256_blend_epi32(leftHalfs, rightHalfs, rightHalfsMask);

		_mm256_storeu_ps((float*)dest.data(), _mm256_cvtepi32_ps(blocks1));
		_mm256_storeu_ps((float*)dest.data()+32, _mm256_cvtepi32_ps(blocks2));
	}
}

