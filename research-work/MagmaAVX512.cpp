#include "MagmaAVX512.h"
#include <iostream>
#include "table4X256.hpp"
#include "table2X65536.hpp"

MagmaAVX512::MagmaAVX512(const key& key) {
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 16; ++j)
		{
			this->roundKeys[i][j].vector = reinterpret_cast<const uint32_t*>(key.bytes)[7 - i];
		}
	}
}

static inline __m512i invBytes(__m512i data)
{
	uint32_t mask[] = { 0x0010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F, 0x0010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F, 0x0010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F, 0x0010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F };
	__m512i mask2 = _mm512_loadu_epi32((const __m512i*)mask);
	return _mm512_shuffle_epi8(data, mask2);
}


static inline __m512i tTransformInRegistersAVX512(const __m512i data) const
{
	__m512i result = _mm512_setzero_si512();

	const int maskByte1 = 0xFF000000;
	const int maskByte2 = 0x00FF0000;
	const int maskByte3 = 0x0000FF00;
	const int maskByte4 = 0x000000FF;

	
	const uint64_t tmpForMask1 = 0x1111111111111111;
	const uint64_t tmpForMask2 = 0x2222222222222222;
	const uint64_t tmpForMask3 = 0x4444444444444444;
	const uint64_t tmpForMask4 = 0x8888888888888888;

	// 0:

	
	__m512i tmp = _mm512_and_epi32(_mm512_set1_epi32(maskByte4), data);

	__m512i currentIndexes = _mm512_set1_epi8(32);
	__m512i tmpResult = _mm512_setzero_si512();

	__mmask64 currentMask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	__mmask64 mainMask = currentMask;

	__m512i reg1 = _mm512_loadu_epi32((const __m512i*)(sTable4x256[0]));
	__m512i reg2 = _mm512_loadu_epi32((const __m512i*)(sTable4x256[0]) + 1);
	__m512i reg3 = _mm512_loadu_epi32((const __m512i*)(sTable4x256[0]) + 2);
	__m512i reg4 = _mm512_loadu_epi32((const __m512i*)(sTable4x256[0]) + 3);

	// 0
	__m512i tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(0));
	__m512i tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg1);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	// 1

	currentIndexes = _mm512_set1_epi8((1 + 1) * 32);
	__mmask64 tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(0));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg1);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	// 2

	currentIndexes = _mm512_set1_epi8((2 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(64));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg2);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	// 3

	currentIndexes = _mm512_set1_epi8((3 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(64));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg2);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	// 4

	currentIndexes = _mm512_set1_epi8(5 * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(128));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg3);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	// 5

	currentIndexes = _mm512_set1_epi8((5 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(128));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg3);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	// 6

	currentIndexes = _mm512_set1_epi8((6 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(192));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg4);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	// 7

	currentIndexes = _mm512_set1_epi8((7 + 1) * 32 - 1);
	tmpmask = _mm512_cmple_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(192));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg4);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	tmp = _mm512_and_epi32(_mm512_set1_epi32(maskByte4), tmpResult);
	result = _mm512_xor_epi32(result, tmp);

	
	// 1: 


	tmp = _mm512_and_epi32(_mm512_set1_epi32(maskByte3), data);

	currentIndexes = _mm512_set1_epi8(32);
	tmpResult = _mm512_setzero_si512();

	currentMask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	mainMask = currentMask;

	
	tmp = tmp;

	reg1 = _mm512_loadu_epi32((const __m512i*)(sTable4x256[1]));
	reg2 = _mm512_loadu_epi32((const __m512i*)(sTable4x256[1]) + 1);
	reg3 = _mm512_loadu_epi32((const __m512i*)(sTable4x256[1]) + 2);
	reg4 = _mm512_loadu_epi32((const __m512i*)(sTable4x256[1]) + 3);

	// 0
	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(0));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg1);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	// 1

	currentIndexes = _mm512_set1_epi8((1 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(0));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg1);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	// 2

	currentIndexes = _mm512_set1_epi8((2 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(64));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg2);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	// 3

	currentIndexes = _mm512_set1_epi8((3 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(64));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg2);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	// 4

	currentIndexes = _mm512_set1_epi8(5 * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(128));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg3);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	// 5

	currentIndexes = _mm512_set1_epi8((5 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(128));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg3);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	// 6

	currentIndexes = _mm512_set1_epi8((6 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(192));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg4);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	// 7

	currentIndexes = _mm512_set1_epi8((7 + 1) * 32 - 1);
	tmpmask = _mm512_cmple_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(192));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg4);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	tmp = _mm512_and_epi32(_mm512_set1_epi32(maskByte3), tmpResult);
	result = _mm512_xor_epi32(result, tmp);

	// 2: 

	tmp = _mm512_and_epi32(_mm512_set1_epi32(maskByte2), data);

	currentIndexes = _mm512_set1_epi8(32);
	tmpResult = _mm512_setzero_si512();

	currentMask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	mainMask = currentMask;

	tmp = tmp;

	reg1 = _mm512_loadu_epi32((const __m512i*)(sTable4x256[2]));
	reg2 = _mm512_loadu_epi32((const __m512i*)(sTable4x256[2]) + 1);
	reg3 = _mm512_loadu_epi32((const __m512i*)(sTable4x256[2]) + 2);
	reg4 = _mm512_loadu_epi32((const __m512i*)(sTable4x256[2]) + 3);

	// 0
	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(0));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg1);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	// 1

	currentIndexes = _mm512_set1_epi8((1 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(0));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg1);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	// 2

	currentIndexes = _mm512_set1_epi8((2 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(64));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg2);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	// 3

	currentIndexes = _mm512_set1_epi8((3 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(64));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg2);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	// 4

	currentIndexes = _mm512_set1_epi8(5 * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(128));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg3);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	// 5

	currentIndexes = _mm512_set1_epi8((5 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(128));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg3);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	// 6

	currentIndexes = _mm512_set1_epi8((6 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(192));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg4);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	// 7

	currentIndexes = _mm512_set1_epi8((7 + 1) * 32 - 1);
	tmpmask = _mm512_cmple_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(192));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg4);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	tmp = _mm512_and_epi32(_mm512_set1_epi32(maskByte2), tmpResult);
	result = _mm512_xor_epi32(result, tmp);

	// 3: 

	tmp = _mm512_and_epi32(_mm512_set1_epi32(maskByte1), data);

	currentIndexes = _mm512_set1_epi8(32);
	tmpResult = _mm512_setzero_si512();

	currentMask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	mainMask = currentMask;

	tmp = tmp;

	reg1 = _mm512_loadu_epi32((const __m512i*)(sTable4x256[3]));
	reg2 = _mm512_loadu_epi32((const __m512i*)(sTable4x256[3]) + 1);
	reg3 = _mm512_loadu_epi32((const __m512i*)(sTable4x256[3]) + 2);
	reg4 = _mm512_loadu_epi32((const __m512i*)(sTable4x256[3]) + 3);

	// 0
	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(0));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg1);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	// 1

	currentIndexes = _mm512_set1_epi8((1 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(0));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg1);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	// 2

	currentIndexes = _mm512_set1_epi8((2 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(64));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg2);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	// 3

	currentIndexes = _mm512_set1_epi8((3 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(64));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg2);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	// 4

	currentIndexes = _mm512_set1_epi8(5 * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(128));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg3);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	// 5

	currentIndexes = _mm512_set1_epi8((5 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(128));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg3);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	// 6

	currentIndexes = _mm512_set1_epi8((6 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(192));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg4);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	// 7

	currentIndexes = _mm512_set1_epi8((7 + 1) * 32 - 1);
	tmpmask = _mm512_cmple_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(192));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, reg4);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	tmp = _mm512_and_epi32(_mm512_set1_epi32(maskByte1), tmpResult);
	result = _mm512_xor_epi32(result, tmp);
	return result;
}


__m512i tTransofrmAVX512(__m512i data)
{
	const int loMask = 0x0;
	const int hiMask = 0x1;

	__m512i divTmp11 = _mm512_shufflehi_epi16(data, 0xD8);
	__m512i divTmp12 = _mm512_shufflelo_epi16(divTmp11, 0xD8);

	__m512i divTmp21 = _mm512_shuffle_epi32(divTmp12, (_MM_PERM_ENUM)0xD8);
	__m512i divTmp22 = _mm512_shuffle_epi32(divTmp12, (_MM_PERM_ENUM)0x8D);

	__m256i divTmp31 = _mm512_extracti64x4_epi64(divTmp21, hiMask);
	__m256i divTmp32 = _mm512_extracti64x4_epi64(divTmp21, loMask);
	__m256i divTmp33 = _mm512_extracti64x4_epi64(divTmp22, hiMask);
	__m256i divTmp34 = _mm512_extracti64x4_epi64(divTmp22, loMask);

	__m256i hi = _mm256_blend_epi32(divTmp31, divTmp34, 0x33);
	__m256i lo = _mm256_blend_epi32(divTmp33, divTmp32, 0x33);

	__m256i sLo = _mm256_shuffle_epi32(lo, 0x4E);
	__m256i ssLo = _mm256_permute4x64_epi64(sLo, 0x4E);
	__m256i sssLo = _mm256_blend_epi32(lo, ssLo, 0x3C);

	__m256i sHi = _mm256_shuffle_epi32(hi, 0x4E);
	__m256i ssHi = _mm256_permute4x64_epi64(sHi, 0x4E);
	__m256i sssHi = _mm256_blend_epi32(hi, ssHi, 0x3C);
	
	__m512i expandedLo = _mm512_cvtepu16_epi32(sssLo);
	__m512i expandedHi = _mm512_cvtepu16_epi32(sssHi);

	__m512i resultLo = _mm512_i32gather_epi32(expandedLo, (int const*)sTable2x65536[0], 2);
	__m512i resultHi = _mm512_i32gather_epi32(expandedHi, (int const*)sTable2x65536[1], 2);

	__m512i recTmp11 = _mm512_shufflehi_epi16(resultHi, 0xB1);
	__m512i recTmp12 = _mm512_shufflelo_epi16(recTmp11, 0xB1);

	__m512i result = _mm512_mask_blend_epi16(0x55555555, recTmp12, resultLo);

	return result;
}

static inline __m512i cyclicShift11(__m512i data)
{
	__m512i resultShift = _mm512_slli_epi32(data, 11);
	__m512i resultShift2 = _mm512_srli_epi32(data, 21);
	return _mm512_xor_epi32(resultShift, resultShift2);
}

static inline gTransformationAVX(const halfVectorMagma* roundKeyAddr, const __m512i data, bool reg) const
{
	__m512i vectorKey = _mm512_loadu_epi32((const __m512i*)roundKeyAddr);
	__m512i result = _mm512_add_epi32(vectorKey, data);
	if (reg)
	{
		result = tTransformInRegistersAVX512(result);
	}
	else 
	{
		result = tTransofrmAVX512(result);
	}
	result = cyclicShift11(result);
	result = invBytes(result);
	return result;
}

static inline void transformationGaVX(__m512i& loHalfs, __m512i& hiHalfs, const halfVectorMagma* roundKeyAddr, bool reg) const
{
	__m512i gResult = gTransformationAVX(roundKeyAddr, loHalfs, reg);
	__m512i tmp = _mm512_xor_epi32(invBytes(gResult), hiHalfs);
	hiHalfs = loHalfs;
	loHalfs = tmp;
}

static inline void  encryptEightBlocks(__m512i& loHalfs, __m512i& hiHalfs, bool reg, const halfVectorMagma** roundKeys) const
{
	for (size_t i = 0; i < 24; i++)
	{
		transformationGaVX(loHalfs, hiHalfs, roundKeys[i % 8], reg);
	}
	for (size_t i = 0; i < 7; i++)
	{
		transformationGaVX(loHalfs, hiHalfs, roundKeys[7 - i], reg);
	}
	__m512i gResults = gTransformationAVX(roundKeys[0], loHalfs, reg);
	__m512i tmp = _mm512_xor_epi32(invBytes(gResults), hiHalfs);
	hiHalfs = tmp;
}

static inline void MagmaAVX512::decryptEightBlocks(__m512i& loHalfs, __m512i& hiHalfs, bool reg, const halfVectorMagma** roundKeys) const
{
	for (size_t i = 0; i < 8; i++)
	{
		transformationGaVX(loHalfs, hiHalfs, roundKeys[i], reg);
	}
	for (size_t i = 0; i < 23; i++)
	{
		transformationGaVX(loHalfs, hiHalfs, roundKeys[7 - (i % 8)], reg);
	}
	__m512i gResults = gTransformationAVX(roundKeys[0], loHalfs, reg);
	__m512i tmp = _mm512_xor_epi32(invBytes(gResults), hiHalfs);
	hiHalfs = tmp;
}


void MagmaAVX512::processData(std::span<const byteVectorMagma> src, std::span<byteVectorMagma> dest, bool en, bool reg) const
{
	const int blockMask = 0xB1;
	const int hiHalfsMask = 0x5555; 
	const int loHalfsMask = 0xAAAA;
	for (size_t b = 0; b < src.size(); b += 16)
	{
		__m512i blocks1 = _mm512_loadu_epi32((const __m512i*)(src.data() + b));
		__m512i blocks2 = _mm512_loadu_epi32((const __m512i*)(src.data() + b + 8));

		__m512i blocksTmp = _mm512_shuffle_epi32(blocks1, (_MM_PERM_ENUM)blockMask);

		__m512i loHalfs = _mm512_mask_blend_epi32(loHalfsMask, blocks1, blocksTmp);
		__m512i hiHalfs = _mm512_mask_blend_epi32(hiHalfsMask, blocks2, blocksTmp);

		
		if (en)
		{
			encryptEightBlocks(loHalfs, hiHalfs, reg, this->roundKeys);
		}
		else {
			decryptEightBlocks(loHalfs, hiHalfs, reg, this->roundKeys);
		}

		__m512i tmp = _mm512_shuffle_epi32(hiHalfs, (_MM_PERM_ENUM)blockMask);
		__m512i tmp2 = _mm512_shuffle_epi32(loHalfs, (_MM_PERM_ENUM)blockMask);

		blocks1 = _mm512_mask_blend_epi32(loHalfsMask, loHalfs, tmp);
		blocks2 = _mm512_mask_blend_epi32(loHalfsMask, tmp2, hiHalfs);

		_mm512_storeu_epi32((__m512i*)(dest.data() + b), blocks1);
		_mm512_storeu_epi32((__m512i*)(dest.data() + b + 8), blocks2);
	}
}

