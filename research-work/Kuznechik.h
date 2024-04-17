#pragma once
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <immintrin.h>
#include <span>

struct key {
    uint8_t bytes[32];
    explicit key(uint8_t* data);
};

union halfVectorKuznechik {
    uint8_t bytes[sizeof(uint64_t)];
    uint64_t halfVector;
    halfVectorKuznechik() = default;
    halfVectorKuznechik(const uint64_t src);
};

union byteVectorKuznechik {
    struct {
        halfVectorKuznechik lo, hi;
    };
    uint8_t bytes[sizeof(uint64_t)*2];
    byteVectorKuznechik() = default;
    constexpr byteVectorKuznechik(const halfVectorKuznechik& lo, const halfVectorKuznechik& hi);
    explicit byteVectorKuznechik(uint8_t* src);
};

class Kuznechik {
public:
    Kuznechik(const key& key);
    void encryptTextAVX2(std::span<const byteVectorKuznechik> src, std::span<byteVectorKuznechik> dest, bool en)  const;
};