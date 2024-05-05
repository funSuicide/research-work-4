#pragma once
#include <cstdint>
#include <algorithm>

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
    uint8_t bytes[sizeof(uint64_t) * 2];
    byteVectorKuznechik() = default;
    byteVectorKuznechik(const halfVectorKuznechik& lo, const halfVectorKuznechik& hi);
    explicit byteVectorKuznechik(uint8_t* src);
};

union halfVectorMagma {
    uint8_t bytes[sizeof(uint32_t)];
    uint32_t vector;
    halfVectorMagma() = default;
    halfVectorMagma(const uint32_t src);
};

union byteVectorMagma {
    struct {
        halfVectorMagma lo, hi;
    };
    struct {
        uint16_t l0, l1, l2, l3;
    };
    halfVectorMagma halfs[2];
    uint8_t bytes[8];
    uint64_t ull;
    byteVectorMagma() = default;
    byteVectorMagma(const halfVectorMagma& left, const halfVectorMagma& right); //+
    byteVectorMagma(uint16_t l0, uint16_t l1, uint16_t l2, uint16_t l3);
    explicit byteVectorMagma(uint8_t* data); // +
};