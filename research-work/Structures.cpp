#include "Structures.h"

halfVectorKuznechik::halfVectorKuznechik(const uint64_t src) : halfVector(src) {}

byteVectorKuznechik::byteVectorKuznechik(const halfVectorKuznechik& lo, const halfVectorKuznechik& hi) : lo(lo), hi(hi) {}

byteVectorKuznechik::byteVectorKuznechik(uint8_t* data)
{
	std::copy_n(data, 16, bytes);
}

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