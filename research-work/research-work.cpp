#include <iostream>
#include <vector>
#include <stdint.h>
#include <string>
#include "MagmaAVX2.h"
#include "testData.hpp"
#include "Kuznechik.h"
#include "MagmaAVX512.h"
#include <chrono>
#include <string_view>
#include <algorithm>
#include <numeric>
#include <cstring>
#include <fstream>
#include "Encryptor.h"
#include <filesystem>
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
	Kuznechik k2(S);

	std::span<byteVectorKuznechik, GIGABYTE / 16> aa2(a);
	for (int j = 0; j < 5; ++j) {
		std::vector<byteVectorKuznechik> b(GIGABYTE / 16);
		std::span<byteVectorKuznechik, GIGABYTE / 16> dest(b);
		auto begin = std::chrono::steady_clock::now();
		k2.processDataAVX512(aa2, dest, 1);
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

void magma512TestSpeed(bool reg)
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

	MAVX512.processData(testSrcMagma512, testDestMagma512, 1, reg);

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
		M.processData(tmpMagma, dest, 1, reg);
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

bool checkFile(const std::string& path)
{
	// std::filesystem::exists
	return std::filesystem::exists(path);
}

int main(int argc, char* argv[]) {
	int algorihm = 0;
	int mode = 0;


	// use this instead of constants
	enum Mode {
		ENCRYPT,
		DECRYPT,
		TEST
	};

	enum Algorithm {
		KUZNYECHIK_AVX2,
		KUZNYECHIK_AVX512,
		MAGMA_AV2,
		MAGMA_AV512,
		MAGMA_AV512_REG,
	};

	setlocale(LC_ALL, "");
	if (argc != 3) 
	{
		std::cout << "Некорректное количество параметров." << std::endl;
		std::cout << "Используйте: " << argv[0] << " -[алгоритм шифрования] -[шифрование/дешифрование/тест скорости]" << std::endl;
		exit(0);
	}
	if (strcmp(argv[1], "-mavx2") == 0)
	{
		algorihm = 1;
		std::cout << "Выбран алгоритм МАГМА для случая AVX2." << std::endl;
	}
	else if (strcmp(argv[1], "-kavx2") == 0)
	{
		algorihm = 2;
		std::cout << "Выбран алгоритм КУЗНЕЧИК для случая AVX2." << std::endl;
	}
	else if (strcmp(argv[1], "-kavx512") == 0)
	{
		algorihm = 3;
		std::cout << "Выбран алгоритм КУЗНЕЧИК для случая AVX512." << std::endl;
	}
	else if (strcmp(argv[1], "-mavx512") == 0)
	{
		algorihm = 4;
		std::cout << "Выбран алгоритм МАГМА для случая AVX512 (таблица замен в памяти)." << std::endl;
	}
	else if (strcmp(argv[1], "-mavx512reg") == 0)
	{
		algorihm = 5;
		std::cout << "Выбран алгоритм МАГМА для случая AVX512 (таблица замен в регистрах)." << std::endl;
	}
	else
	{
		std::cout << "Некорректно задан алгоритм шифрования. Используйте: -mavx2, -kavx2, -kavx512, -mavx512, -mavx512reg";
		exit(0);
	}
	if (strcmp(argv[2], "-e") == 0)
	{
		mode = 1;
	}
	else if (strcmp(argv[2], "-d") == 0)
	{
		mode = 0;
	}
	else if (strcmp(argv[2], "-test") == 0)
	{
		mode = 2;
	}
	else
	{
		std::cout << "Некорректно задан режим работы алгоритма. Используйте: -e, -d, -test" << std::endl;
		exit(0);
	}

	key key;
	std::string inputPath;
	std::string outputPath;
	uint32_t iV;

	if (mode == 1 || mode == 0)
	{
		while (true) {
			std::cout << "Введите путь до файла, в котором хранится ключ: ";
			std::string testPath;
			std::cin >> testPath;
			if (checkFile(testPath))
			{
				std::ifstream in(testPath, std::ios::binary);
				in.read((char*)key.bytes, 32);
				break;
			}
		}

		std::cout << "Введите вектор инициализации: ";
		std::cin >> iV;

		std::cout << "Введите путь до входного файла: ";

		while (true) {
			std::string testPath;
			std::cin >> testPath;
			if (checkFile(testPath)) {
				inputPath = testPath;
				break;
			}
			std::cout << "Заданный файл не найден. Повторите: ";
		}

		std::cout << "Введите путь до выходного файла: ";

		std::cin >> outputPath;

	}
	
	switch (algorihm)
	{
	case 1:
	{
		//Encryptor<Magma, byteVectorMagma> M2(key);
		// Make a template function
		Encryptor<MagmaAVX2, byteVectorMagma> MAVX2(key);
		if (mode == 1)
		{
			auto begin = std::chrono::steady_clock::now();
			MAVX2.encrypt(inputPath, outputPath, iV);
			auto end = std::chrono::steady_clock::now();
			auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
			std::cout << "Общее время шифрования: " << time.count() << " ms" << std::endl;
		}
		else if (mode == 0)
		{
			auto begin = std::chrono::steady_clock::now();
			MAVX2.decrypt(inputPath, outputPath, iV);
			auto end = std::chrono::steady_clock::now();
			auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
			std::cout << "Общее время дешифрования: " << time.count() << " ms" << std::endl;
		}
		else if (mode == 2)
		{
			magmaTestSpeed();
		}
		break;
	}
	case 2:
	{
		//Encryptor<Kuznechik, byteVectorKuznechik> K2(key);

		if (mode == 1)
		{
			auto begin = std::chrono::steady_clock::now();
			//K2.encrypt(inputPath, outputPath);
			auto end = std::chrono::steady_clock::now();
			auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
			std::cout << "Общее время шифрования: " << time.count() << " ms" << std::endl;
		}
		else if (mode == 0)
		{
			auto begin = std::chrono::steady_clock::now();
			//K2.decrypt(inputPath, outputPath);
			auto end = std::chrono::steady_clock::now();
			auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
			std::cout << "Общее время дешифрования: " << time.count() << " ms" << std::endl;
		}
		else if (mode == 2)
		{
			kuznechikTestSpeed();
		}
		break;
	}
	case 3: 
	{
		if (mode == 1)
		{
			auto begin = std::chrono::steady_clock::now();
			//K2.encrypt(inputPath, outputPath);
			auto end = std::chrono::steady_clock::now();
			auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
			std::cout << "Общее время шифрования: " << time.count() << " ms" << std::endl;
		}
		else if (mode == 0)
		{
			auto begin = std::chrono::steady_clock::now();
			//K2.decrypt(inputPath, outputPath);
			auto end = std::chrono::steady_clock::now();
			auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
			std::cout << "Общее время дешифрования: " << time.count() << " ms" << std::endl;
		}
		else if (mode == 2)
		{
			kuznechik512TestSpeed();
		}
		break;
	}
	case 4:
	{
		if (mode == 1)
		{
			auto begin = std::chrono::steady_clock::now();
			//K2.encrypt(inputPath, outputPath);
			auto end = std::chrono::steady_clock::now();
			auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
			std::cout << "Общее время шифрования: " << time.count() << " ms" << std::endl;
		}
		else if (mode == 0)
		{
			auto begin = std::chrono::steady_clock::now();
			//K2.decrypt(inputPath, outputPath);
			auto end = std::chrono::steady_clock::now();
			auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
			std::cout << "Общее время дешифрования: " << time.count() << " ms" << std::endl;
		}
		else if (mode == 2)
		{
			magma512TestSpeed(0);
		}
		break;
	}
	case 5:
	{
if (mode == 1)
		{
			auto begin = std::chrono::steady_clock::now();
			//K2.encrypt(inputPath, outputPath);
			auto end = std::chrono::steady_clock::now();
			auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
			std::cout << "Общее время шифрования: " << time.count() << " ms" << std::endl;
		}
		else if (mode == 0)
		{
			auto begin = std::chrono::steady_clock::now();
			//K2.decrypt(inputPath, outputPath);
			auto end = std::chrono::steady_clock::now();
			auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
			std::cout << "Общее время дешифрования: " << time.count() << " ms" << std::endl;
		}
		else if (mode == 2)
		{
			magma512TestSpeed(1);
		}
		break;
	}
	default:
		break;
	}
}



