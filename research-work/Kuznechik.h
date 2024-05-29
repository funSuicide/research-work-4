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

class Kuznechik {
public:
    Kuznechik(const key& key);
    void processData(std::span<const byteVectorKuznechik> src, std::span<byteVectorKuznechik> dest, bool en)  const;
    void processDataAVX512(std::span<const byteVectorKuznechik> src, std::span<byteVectorKuznechik> dest, bool en)  const;
};

