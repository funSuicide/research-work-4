#include <iostream>
#include <vector>
#include <stdint.h>
#include <string>
#include "MagmaAVX2.hpp"
#include "testData.hpp"
#include "Kuznechik.h"
#include "MagmaAVX512.hpp"
#include "KuznechikAVX512.hpp"
#include <chrono>
#include <string_view>
#include <algorithm>
#include <numeric>
#include <cstring>
#include <fstream>
#include "Encryptor.h"
#include <filesystem>
#include "MagmaAVX512Reg.hpp"
#define GIGABYTE 1024*1024*1024

using duration_t = std::chrono::duration<float>;

template<typename typeVector>
std::string printVector (const typeVector& block) {
	std::string result;
	for (int i = sizeof(typeVector) - 1; i >= 0; --i) {
		uint8_t high, low;
		high = block.bytes[i] >> 4;
		low = block.bytes[i] & 0xf;
		result.push_back((char)(high < 10 ? ('0' + high) : ('A' + high - 10)));
		result.push_back((char)(low < 10 ? ('0' + low) : ('A' + low - 10)));
	}
	return result;
}

template<typename typeVector>
void generateTestVector(std::vector<typeVector>& dest)
{
	dest.resize(GIGABYTE / sizeof(typeVector));
	for (size_t i = 0; i < dest.size(); ++i)
	{
		dest[i].halfsData.lo = 17721245453 * i;
		dest[i].halfsData.hi = 17721245453 * i + 13243342;
	}
}

void magmaTestSpeed()
{
	byteVectorMagma testBlockMagma(testDataBytesMagma);
	key testKeyMagma(testKeyBytesMagma);

	MagmaAVX2 MAVX2(testKeyMagma);

	byteVectorMagma expectedBlockMagma(expectedBlockBytesMagma);

	std::vector<byteVectorMagma> testSrcMagma = { testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma };
	std::vector<byteVectorMagma> testDestMagma(8);

	std::vector<byteVectorMagma> testSrcMagma512 = { testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma };
	std::vector<byteVectorMagma> testDestMagma512(16);


	std::cout << "Тестовые данные: " << printVector(testBlockMagma) << std::endl;
	std::cout << "Тестовый ключ: " << printVector(testKeyMagma) << std::endl;
	std::cout << "Ожидаемый результат: " << printVector(expectedBlockMagma) << std::endl;

	MAVX2.processData(testSrcMagma, testDestMagma, 1);

	std::cout << "Реальный результат: " << printVector(testDestMagma[0]) << std::endl;

	std::cout << "----------------------------------------------" << std::endl;

	std::cout << "Запуск теста скорости..." << std::endl;
	
	std::vector<float> times2;
	std::vector<byteVectorMagma> vectorForMagma;
	generateTestVector(vectorForMagma);

	uint8_t c[32] = "asdafasdasdasfdasdasdakfksakfsa";
	key S(c);
	MagmaAVX2 M(S);

	std::span<byteVectorMagma, GIGABYTE / 8> tmpMagma(vectorForMagma);

	for (int j = 0; j < 5; ++j) {
		std::vector<byteVectorMagma> b(GIGABYTE / 8);
		std::span<byteVectorMagma, GIGABYTE / 8> dest(b);
		auto begin = std::chrono::steady_clock::now();
		M.processData(tmpMagma, dest, 1);
		auto end = std::chrono::steady_clock::now();
		auto time = std::chrono::duration_cast<duration_t>(end - begin);
		times2.push_back(time.count());
		std::cout << "Обработано: " << j+1 << " ГБ" << ": " << time.count() << std::endl;
	}

	double meanM2 = std::accumulate(times2.begin(), times2.end(), 0.0) / times2.size();
	std::cout << meanM2 << std::endl;
	std::cout << "Среднее значение скорости алгоритма (AVX-2): " << 1 / meanM2 << "ГБ/с" << std::endl;

	std::cout << "----------------------------------------------" << std::endl;
}

void kuznechikTestSpeed() 
{
	byteVectorKuznechik testBlockKuz(testDataBytesKuz);

	byteVectorKuznechik expectedBlockKuz(expectedBlockBytesKuz);
	key testKeyKuz(testKeyBytesKuz);

	std::vector<byteVectorKuznechik> testSrcKuz = { testBlockKuz, testBlockKuz, testBlockKuz, testBlockKuz };
	std::vector<byteVectorKuznechik> testDestKuz(4);

	Kuznechik KAVX2(testKeyKuz);

	std::cout << "Тестовые данные: " << printVector(testBlockKuz) << std::endl;
	std::cout << "Тестовый ключ: " << printVector(testKeyKuz) << std::endl;
	std::cout << "Ожидаемый результат: " << printVector(expectedBlockKuz) << std::endl;

	KAVX2.processData(testSrcKuz, testDestKuz, 1);

	std::cout << "Реальный результат: " << printVector(testDestKuz[0]) << std::endl;

	std::cout << "----------------------------------------------" << std::endl;

	std::cout << "Запуск теста скорости..." << std::endl;

	std::vector<float> timesKuz2;

	std::vector<byteVectorKuznechik> a;
	generateTestVector(a);

	uint8_t c[32] = "asdafasdasdasfdasdasdakfksakfsa";
	key S(c);
	Kuznechik k2(S);

	std::span<byteVectorKuznechik, GIGABYTE / 16> aa2(a);
	for (int j = 0; j < 5; ++j) {
		std::vector<byteVectorKuznechik> b(GIGABYTE / 16);
		std::span<byteVectorKuznechik, GIGABYTE / 16> dest(b);
		auto begin = std::chrono::steady_clock::now();
		k2.processData(aa2, dest, 1);
		auto end = std::chrono::steady_clock::now();
		auto time = std::chrono::duration_cast<duration_t>(end - begin);
		timesKuz2.push_back(time.count());
		std::cout << "Обработано: " << j+1 << " ГБ" << ": " << time.count() << std::endl;
	}

	double meanKuz2 = std::accumulate(timesKuz2.begin(), timesKuz2.end(), 0.0) / timesKuz2.size();
	std::cout << meanKuz2 << std::endl;
	std::cout << "Среднее значение скорости алгоритма (AVX-2): " << 1 / meanKuz2 << "ГБ/с" << std::endl;

	std::cout << "----------------------------------------------" << std::endl;
}


void kuznechik512TestSpeed()
{
	std::vector<float> timesKuz2;

	std::vector<byteVectorKuznechik> a;
	generateTestVector(a);

	uint8_t c[32] = "asdafasdasdasfdasdasdakfksakfsa";
	key S(c);
	KuznechikAVX512 K512(S);

	std::span<byteVectorKuznechik, GIGABYTE / 16> aa2(a);
	for (int j = 0; j < 5; ++j) {
		std::vector<byteVectorKuznechik> b(GIGABYTE / 16);
		std::span<byteVectorKuznechik, GIGABYTE / 16> dest(b);
		auto begin = std::chrono::steady_clock::now();
		K512.processData(aa2, dest, 1);
		auto end = std::chrono::steady_clock::now();
		auto time = std::chrono::duration_cast<duration_t>(end - begin);
		timesKuz2.push_back(time.count());
		std::cout << "Обработано: " << j+1 << " ГБ" << ": " << time.count() << std::endl;
	}

	double meanKuz2 = std::accumulate(timesKuz2.begin(), timesKuz2.end(), 0.0) / timesKuz2.size();
	std::cout << meanKuz2 << std::endl;
	std::cout << "Среднее значение скорости алгоритма (AVX-512): " << 1 / meanKuz2 << "ГБ/с" << std::endl;

	std::cout << "----------------------------------------------" << std::endl;
}

void magma512TestSpeed()
{
	byteVectorMagma testBlockMagma(testDataBytesMagma);
	key testKeyMagma(testKeyBytesMagma);

	MagmaAVX512 MAVX512(testKeyMagma);

	byteVectorMagma expectedBlockMagma(expectedBlockBytesMagma);

	std::vector<byteVectorMagma> testSrcMagma512 = { testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma };
	std::vector<byteVectorMagma> testDestMagma512(16);


	std::cout << "Тестовые данные: " << printVector(testBlockMagma) << std::endl;
	std::cout << "Тестовый ключ: " << printVector(testKeyMagma) << std::endl;
	std::cout << "Ожидаемый результат: " << printVector(expectedBlockMagma) << std::endl;

	MAVX512.processData(testSrcMagma512, testDestMagma512, 1);

	std::cout << "Реальный результат: " << printVector(testDestMagma512[0]) << std::endl;

	std::cout << "----------------------------------------------" << std::endl;

	std::cout << "Запуск теста скорости..." << std::endl;
	
	std::vector<float> times2;
	std::vector<byteVectorMagma> vectorForMagma;
	generateTestVector(vectorForMagma);

	uint8_t c[32] = "asdafasdasdasfdasdasdakfksakfsa";
	key S(c);
	MagmaAVX512 M(S);

	std::span<byteVectorMagma, GIGABYTE / 8> tmpMagma(vectorForMagma);

	for (int j = 0; j < 5; ++j) {
		std::vector<byteVectorMagma> b(GIGABYTE / 8);
		std::span<byteVectorMagma, GIGABYTE / 8> dest(b);
		auto begin = std::chrono::steady_clock::now();
		M.processData(tmpMagma, dest, 1);
		auto end = std::chrono::steady_clock::now();
		auto time = std::chrono::duration_cast<duration_t>(end - begin);
		times2.push_back(time.count());
		std::cout << "Обработано: " << j+1 << " ГБ" << ": " << time.count() << std::endl;
	}

	double meanM2 = std::accumulate(times2.begin(), times2.end(), 0.0) / times2.size();
	std::cout << meanM2 << std::endl;
	std::cout << "Среднее значение скорости алгоритма (AVX-512): " << 1 / meanM2 << "ГБ/с" << std::endl;

	std::cout << "----------------------------------------------" << std::endl;
}

void magma512RegTestSpeed()
{
	byteVectorMagma testBlockMagma(testDataBytesMagma);
	key testKeyMagma(testKeyBytesMagma);

	MagmaAVX512Reg MAVX512Reg(testKeyMagma);

	byteVectorMagma expectedBlockMagma(expectedBlockBytesMagma);

	std::vector<byteVectorMagma> testSrcMagma512 = { testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma, testBlockMagma };
	std::vector<byteVectorMagma> testDestMagma512(16);


	std::cout << "Тестовые данные: " << printVector(testBlockMagma) << std::endl;
	std::cout << "Тестовый ключ: " << printVector(testKeyMagma) << std::endl;
	std::cout << "Ожидаемый результат: " << printVector(expectedBlockMagma) << std::endl;

	MAVX512Reg.processData(testSrcMagma512, testDestMagma512, 1);

	std::cout << "Реальный результат: " << printVector(testDestMagma512[0]) << std::endl;

	std::cout << "----------------------------------------------" << std::endl;

	std::cout << "Запуск теста скорости..." << std::endl;
	
	std::vector<float> times2;
	std::vector<byteVectorMagma> vectorForMagma;
	generateTestVector(vectorForMagma);

	uint8_t c[32] = "asdafasdasdasfdasdasdakfksakfsa";
	key S(c);
	MagmaAVX512Reg M(S);

	std::span<byteVectorMagma, GIGABYTE / 8> tmpMagma(vectorForMagma);

	for (int j = 0; j < 5; ++j) {
		std::vector<byteVectorMagma> b(GIGABYTE / 8);
		std::span<byteVectorMagma, GIGABYTE / 8> dest(b);
		auto begin = std::chrono::steady_clock::now();
		M.processData(tmpMagma, dest, 1);
		auto end = std::chrono::steady_clock::now();
		auto time = std::chrono::duration_cast<duration_t>(end - begin);
		times2.push_back(time.count());
		std::cout << "Обработано: " << j+1 << " ГБ" << ": " << time.count() << std::endl;
	}

	double meanM2 = std::accumulate(times2.begin(), times2.end(), 0.0) / times2.size();
	std::cout << meanM2 << std::endl;
	std::cout << "Среднее значение скорости алгоритма (AVX-512): " << 1 / meanM2 << "ГБ/с" << std::endl;

	std::cout << "----------------------------------------------" << std::endl;
}

int main()
{
    std::cout << "Тестирование алгоритма Магма AVX-2" << std::endl;
    magmaTestSpeed();
    std::cout << "Тестирование алгоритма Кузнечик AVX-2" << std::endl;
    kuznechikTestSpeed();
    std::cout << "Тестирование алгоритма Кузнечие AVX-512" << std::endl;
    kuznechik512TestSpeed();

}