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

template <typename T, typename typeVector>
class Encryptor {
	T cryptoAlgorithm;
	static constexpr size_t BLOCK_BUFFER_SIZE = MEGABYTE / sizeof(typeVector);
public:
	Encryptor(const key& key) : cryptoAlgorithm(key) {};

	size_t paddingPKCS(std::vector<typeVector>& src, size_t gCount) const {
		size_t blockIndex = gCount / sizeof(typeVector);
		size_t shiftIndex = gCount % sizeof(typeVector);

		if (shiftIndex == 0) ++blockIndex;

		for (size_t i = shiftIndex; i < sizeof(typeVector); ++i) {
			src[blockIndex].bytes[i] = sizeof(typeVector) - shiftIndex;
		}
		return blockIndex;
	}

	void encrypt(const std::string& pathOne, const std::string& pathTwo) const {
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

				/*
				if (newSize % 2 != 0)
				{
					buffer.resize(BLOCK_BUFFER_SIZE + 2);
					result.resize(BLOCK_BUFFER_SIZE + 2);

					for (size_t i = 0; i < sizeof(typeVector); ++i) 
					{
						buffer[buffer.size() - 1] = typeVector(2 * sizeof(typeVector) - readSize % sizeof(typeVector));
					}

				}*/

				cryptoAlgorithm.processData(buffer, result, 1);
				if (sizeFile % sizeof(typeVector) == 0) {
					out.write((const char*)&result[0], (newSize) * sizeof(typeVector));
				}
				else {
					out.write((const char*)&result[0], (newSize + 1) * sizeof(typeVector));
				}
			}
			else {
				cryptoAlgorithm.processData(buffer, result, 1);
				out.write((const char*)&result[0], BLOCK_BUFFER_SIZE * sizeof(typeVector));
			}
		}
		in.close();
		out.close();
	}

	void decrypt(const std::string& pathOne, const std::string& pathTwo) const {
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
			auto readSize = in.gcount();

			size_t readBlocks = (readSize + sizeof(typeVector) - 1) / sizeof(typeVector);
			size_t writeSizeBytes = readSize;

			if (in.eof()) {
				cryptoAlgorithm.processData(buffer, result, 0);

				writeSizeBytes -= result[readBlocks - 1].bytes[sizeof(typeVector) - 1];
				if (result[readBlocks - 1].bytes[sizeof(typeVector) - 1] == 0) {
					writeSizeBytes -= sizeof(typeVector);
				}

				out.write((const char*)&result[0], writeSizeBytes);
			}
			else
			{
				cryptoAlgorithm.processData(buffer, result, 0);
				out.write((const char*)&result[0], BLOCK_BUFFER_SIZE * sizeof(typeVector));
			}
		}
		in.close();
		out.close();
	}
};





