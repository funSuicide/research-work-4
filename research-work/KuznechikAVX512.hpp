#pragma once
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <immintrin.h>
#include <span>
#include "Structures.h"

class KuznechikAVX512 {
private:
    byteVectorKuznechik roundKeysKuznechik[10][2];
public:
    KuznechikAVX512(const key& key);
    void processDataGamma(std::span<const byteVectorKuznechik> src, std::span<byteVectorKuznechik> dest, uint64_t iV) const;
    void processData(std::span<const byteVectorKuznechik> src, std::span<byteVectorKuznechik> dest, bool en, int count)  const;
};

