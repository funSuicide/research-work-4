#pragma once
#include <iostream>
#include <fstream>
#include <chrono>
#include <time.h>
#include <vector>
#include <numeric> 
#include <string>
#include "Structures.h"

#define MEGABYTE 1024*1024

uint64_t getNewIv(Algorithm alg, uint64_t old)
{
	uint64_t iV = old;
	switch(alg)
	{
	case MAGMA_AVX2: 
		{
			iV+=0x4000;
			break;
		}
	case MAGMA_AVX512:
		{
			iV+=0x2000;
			break;
		}
	case MAGMA_AVX512_REG: 
		{
			iV+=0x2000;
			break;
		}
	case KUZNECHIK_AVX2:
		{
			iV+=0x8000;
			break;
		}
	case KUZNECHIK_AVX512:
		{
			iV+=0x4000;
			break;
		}
	default:
		{
			break;
		}
	}
	return iV;

}

template <typename T, typename typeVector>
class Encryptor {
private:
	T cryptoAlgorithm;
	static constexpr size_t BLOCK_BUFFER_SIZE = MEGABYTE / sizeof(typeVector);

	size_t paddingPKCS(std::vector<typeVector>& src, size_t gCount) const {
		size_t blockIndex = gCount / sizeof(typeVector);
		size_t shiftIndex = gCount % sizeof(typeVector);

		for (size_t i = shiftIndex; i < sizeof(typeVector); ++i) {
			src[blockIndex].bytes[i] = sizeof(typeVector) - shiftIndex;
		}
		return blockIndex;
	}
public:
	Encryptor(const key& key) : cryptoAlgorithm(key) {};

	void encrypt(const std::string& pathOne, const std::string& pathTwo, uint64_t parIV, Algorithm alg) const {
		uint64_t iV = parIV;
		std::ifstream in(pathOne, std::ios::binary);
		in.seekg(0, std::ios::end);
		size_t sizeFile = in.tellg();
		size_t countMegabytes = sizeFile / MEGABYTE;
		size_t countBlocks = (sizeFile + sizeof(typeVector) - 1) / sizeof(typeVector); 

		in.seekg(0, std::ios::beg);
		std::ofstream out(pathTwo, std::ios::binary);
		std::vector<typeVector> buffer(BLOCK_BUFFER_SIZE + 1);
		std::vector<typeVector> result(BLOCK_BUFFER_SIZE + 1);

		while (!in.eof()) {
			in.read((char*)&buffer[0], (BLOCK_BUFFER_SIZE) * sizeof(typeVector));
			size_t readSize = in.gcount();

			size_t readBlocks = (readSize + sizeof(typeVector) - 1) / sizeof(typeVector);

			if (in.eof()) {
				size_t newSize = paddingPKCS(buffer, readSize);

				cryptoAlgorithm.processDataGamma(buffer, result, iV);
				
				

				out.write((const char*)&result[0], (newSize + 1) * sizeof(typeVector));
			}
			else {
				cryptoAlgorithm.processDataGamma(buffer, result, iV);
				iV = getNewIv(alg, iV);
				out.write((const char*)&result[0], BLOCK_BUFFER_SIZE * sizeof(typeVector));
			}
		}
		in.close();
		out.close();
	}

	void decrypt(const std::string& pathOne, const std::string& pathTwo, uint64_t parIV, Algorithm alg) const {
		std::ifstream in(pathOne, std::ios::binary);
		uint64_t iV = parIV;
		in.seekg(0, std::ios::end);
		size_t sizeFile = in.tellg();
		size_t countMegabytes = sizeFile / MEGABYTE;
		size_t countBlocks = (sizeFile + sizeof(typeVector) - 1) / sizeof(typeVector);

		in.seekg(0, std::ios::beg);
		std::ofstream out(pathTwo, std::ios::binary);
		std::vector<typeVector> buffer(BLOCK_BUFFER_SIZE + 1);
		std::vector<typeVector> result(BLOCK_BUFFER_SIZE + 1);

		while (!in.eof()) {
			in.read((char*)&buffer[0], (BLOCK_BUFFER_SIZE) * sizeof(typeVector));
			auto readSize = in.gcount();

			size_t readBlocks = (readSize + sizeof(typeVector) - 1) / sizeof(typeVector);
			size_t writeSizeBytes = readSize;

			if (in.eof()) {
				cryptoAlgorithm.processDataGamma(buffer, result, iV);


				writeSizeBytes -= result[readBlocks - 1].bytes[sizeof(typeVector) - 1];

				out.write((const char*)&result[0], writeSizeBytes);
			}
			else
			{
				cryptoAlgorithm.processDataGamma(buffer, result, iV);
				iV = getNewIv(alg, iV);
				out.write((const char*)&result[0], BLOCK_BUFFER_SIZE * sizeof(typeVector));
			}
		}
		in.close();
		out.close();
	}
};





