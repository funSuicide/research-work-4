#pragma once
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <immintrin.h>
#include <span>
#include "Magma.h"
#include "Structures.h"

class Kuznechik {
public:
    Kuznechik(const key& key);
    void encryptTextAVX2(std::span<const byteVectorKuznechik> src, std::span<byteVectorKuznechik> dest, bool en)  const;
};

static std::array<byteVectorKuznechik, 32> getConstTable();
