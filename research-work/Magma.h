#pragma once
#include <stdint.h>
#include <inttypes.h>
#include <algorithm>      // In a header file only the headers necessary to DECLARE the structures and functions must be present
#include <vector>
#include <immintrin.h>
#include <sstream>
#include <span>
 
struct key {
	uint8_t bytes[32];
	explicit key(uint8_t* data) {              // 1. Why the function body is placed in the header file? 
		std::copy_n(data, 32, bytes);      // 2. And for this simple action you included the entire <algorithm>? 
	}
};

union halfVectorMagma {
	uint8_t bytes[sizeof(uint32_t)];
	uint32_t vector;
	halfVectorMagma() = default;
	halfVectorMagma(const uint32_t src) : vector{ src } {} //  Why the function body is placed in header file? 
};

union byteVectorMagma {
	struct {
		halfVectorMagma hi, lo;
	};

	halfVectorMagma halfs[2];
	uint8_t bytes[8];
	byteVectorMagma() = default;
	byteVectorMagma(const halfVectorMagma& left, const halfVectorMagma& right) : lo{ left }, hi{ right } {} //  Why the function body is placed in header file? 
	explicit byteVectorMagma(uint8_t* data) {  // 1.Why the function body is placed in header file? 
		std::copy_n(data, 8, bytes);       // 2.And for this simple action you included the entire <algorithm>? 
	}
};

class Magma {
public:
	Magma(const key& key);
	void encryptTextAVX(const std::vector<byteVectorMagma>& data, std::vector<byteVectorMagma>& dest) const;
	void encryptTextAVX2(std::span<byteVectorMagma> src, std::span<byteVectorMagma> dest)  const;   // better define src  as std::span<const byteVectorMagma>
};
