#include <iostream>
#include <vector>
#include <stdint.h>
#include <string>
#include "Magma.h"
#include <chrono>
#include <string_view>
#include <algorithm>
#include <numeric>
#define GIGABYTE 1024*1024*1024

void test(std::vector<byteVectorMagma>& dest) {
	dest.resize(GIGABYTE / 8);
	for (size_t i = 0; i < GIGABYTE / 8; ++i)
	{
		dest[i].halfs[0] = 17721245453 * i;
		dest[i].halfs[1] = 17721245453 * i + 13243342;
	}
}

void test2(uint8_t * dest)
{
	for (size_t i = 0; i < GIGABYTE; ++i)
	{
		dest[i] = 'a';
	}
}

std::string hexStr2(uint8_t* data, int len)
{
	std::stringstream ss;
	ss << std::hex;
	for (int i(0);i < len;++i)
		ss << (int)data[i];
	return ss.str();
}

int main()
{
	/*
	using duration_t = std::chrono::duration<float>;
	std::vector<float> times;
	std::vector<byteVectorMagma> a;
	test(a);

	std::span<byteVectorMagma, GIGABYTE / 8> aa(a);
	//uint8_t * data = new uint8_t[GIGABYTE];
	//uint8_t* dest = new uint8_t[GIGABYTE];
	//test2(data);
	for (int j = 0;j < 5; ++j) {
		std::vector<byteVectorMagma> b(GIGABYTE / 8);
		std::span<byteVectorMagma, GIGABYTE / 8> dest(b);
		uint8_t c[32] = "asdafasdasdasfdasdasdakfksakfsa";
		key S(c);
		Magma C(S);

		auto begin = std::chrono::steady_clock::now();
		//C.encryptTextAVX(aa, b);
		C.encryptTextAVX2(aa, dest);
		auto end = std::chrono::steady_clock::now();
		auto time = std::chrono::duration_cast<duration_t>(end - begin);
		times.push_back(time.count());
	}

	double mean = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
	std::cout << mean << std::endl;
	std::cout << "speedAlgorithm: " << 1 / mean << "GB/s" << std::endl;
	*/

	uint8_t testData[8] = {0x10, 0x32, 0x54, 0x76, 0x98, 0xBA, 0xDC, 0xFE};
	//uint8_t testData[8] = { 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10 };
	//uint8_t testKeyBytes[32] = { 0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff };
	uint8_t testKeyBytes[32] = {0xff, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8, 0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1, 0xf0, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};


	byteVectorMagma testBlock(testData);
	key testKey(testKeyBytes);

	std::vector<byteVectorMagma> src = { testBlock, testBlock, testBlock, testBlock, testBlock, testBlock, testBlock, testBlock };
	std::vector<byteVectorMagma> dest(8);

	Magma m(testKey);

	m.encryptTextAVX2(src, dest);

};


