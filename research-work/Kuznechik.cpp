#include "Kuznechik.h"

key::key(uint8_t* data)
{
	std::copy_n(data, 32, bytes);
}

halfVectorKuznechik::halfVectorKuznechik(const uint64_t src) : halfVector(src) {}

byteVectorKuznechik::byteVectorKuznechik(const halfVectorKuznechik& lo, const halfVectorKuznechik& hi): lo (lo), hi(hi) {}

byteVectorKuznechik::byteVectorKuznechik(uint8_t* data)
{
	std::copy_n(data, 16, bytes);
}

void Kuznechik::encryptTextAVX2(std::span<const byteVectorKuznechik> src, std::span<byteVectorKuznechik> dest, bool en) const
{
	for (size_t b = 0; b < src.size(); b += 4)
	{
		__m256i blocks1 = _mm256_load_si256((const __m256i*)(src.data() + b));
		__m256i blocks2 = _mm256_load_si256((const __m256i*)(src.data() + b + 2));
	}
}