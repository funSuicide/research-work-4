#include <iostream>
#include <vector>
#include <stdint.h>
#include <string>
#include "MagmaAVX2.hpp"
#include "testData.hpp"
#include "Kuznechik.h"
#include "MagmaAVX512.hpp"
#include "KuznechikAVX512.hpp"
#include "MagmaAVX512Reg.hpp"
#include <chrono>
#include <string_view>
#include <algorithm>
#include <numeric>
#include <cstring>
#include <fstream>
#include "Encryptor.h"
#include <filesystem>
#define GIGABYTE 1024*1024*1024

bool checkFile(const std::string& path)
{
	return std::filesystem::exists(path);
}

enum Mode {
	ENCRYPT,
	DECRYPT,
};

enum Algorithm {
	KUZNECHIK_AVX2,
	KUZNECHIK_AVX512,
	MAGMA_AVX2,
	MAGMA_AVX512,
	MAGMA_AVX512_REG,
};

template <typename alg, typename typeVector>
void fileOperation(const std::string& inputPath, const std::string& outputPath, Mode mode, uint32_t iV, const Encryptor<alg, typeVector>& E)
{
	if (mode == ENCRYPT)
	{
		auto begin = std::chrono::steady_clock::now();
		E.encrypt(inputPath, outputPath, iV);
		auto end = std::chrono::steady_clock::now();
		auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
		std::cout << "Общее время шифрования: " << time.count() << " ms" << std::endl;
	}
	else 
	{
		auto begin = std::chrono::steady_clock::now();
		E.decrypt(inputPath, outputPath, iV);
		auto end = std::chrono::steady_clock::now();
		auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
		std::cout << "Общее время шифрования: " << time.count() << " ms" << std::endl;
	}
}

int main(int argc, char* argv[]) {
	Mode mode;
	Algorithm alg;

	setlocale(LC_ALL, "");
	if (argc != 3) 
	{
		std::cout << "Некорректное количество параметров." << std::endl;
		std::cout << "Используйте: " << argv[0] << " -[алгоритм шифрования] -[шифрование/дешифрование]" << std::endl;
		exit(0);
	}
	if (strcmp(argv[1], "-mavx2") == 0)
	{
		alg = MAGMA_AVX2;
		std::cout << "Выбран алгоритм Магма для случая AVX2." << std::endl;
	}
	else if (strcmp(argv[1], "-kavx2") == 0)
	{
		alg = KUZNECHIK_AVX2;
		std::cout << "Выбран алгоритм Кузнечик для случая AVX2." << std::endl;
	}
	else if (strcmp(argv[1], "-kavx512") == 0)
	{
		alg = KUZNECHIK_AVX512;
		std::cout << "Выбран алгоритм Кузнечик для случая AVX512." << std::endl;
	}
	else if (strcmp(argv[1], "-mavx512") == 0)
	{
		alg = MAGMA_AVX512;
		std::cout << "Выбран алгоритм Магма для случая AVX512 (таблица замен в памяти)." << std::endl;
	}
	else if (strcmp(argv[1], "-mavx512reg") == 0)
	{
		alg = MAGMA_AVX512_REG;
		std::cout << "Выбран алгоритм Магма для случая AVX512 (таблица замен в регистрах)." << std::endl;
	}
	else
	{
		std::cout << "Некорректно задан алгоритм шифрования. Используйте: -mavx2, -kavx2, -kavx512, -mavx512, -mavx512reg";
		exit(0);
	}
	if (strcmp(argv[2], "-e") == 0)
	{
		mode = ENCRYPT;
	}
	else if (strcmp(argv[2], "-d") == 0)
	{
		mode = DECRYPT;
	}
	else
	{
		std::cout << "Некорректно задан режим работы алгоритма. Используйте: -e, -d" << std::endl;
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
	
	switch (alg)
	{
	case MAGMA_AVX2:
	{
		Encryptor<MagmaAVX2, byteVectorMagma> MAVX2(key);
		fileOperation(inputPath, outputPath, mode, iV, MAVX2);
		break;
	}
	case KUZNECHIK_AVX2:
	{
		Encryptor<Kuznechik, byteVectorKuznechik> KAVX2(key);
		fileOperation(inputPath, outputPath, mode, iV, KAVX2);
		break;
	}
	case KUZNECHIK_AVX512: 
	{
		Encryptor<KuznechikAVX512, byteVectorKuznechik> KAVX512(key);
		fileOperation(inputPath, outputPath, mode, iV, KAVX512);
		break;
	}
	case MAGMA_AVX512:
	{
		Encryptor<MagmaAVX512, byteVectorMagma> MAVX512(key);
		fileOperation(inputPath, outputPath, mode, iV, MAVX512);
		break;
	}
	case MAGMA_AVX512_REG:
	{
		Encryptor<MagmaAVX512Reg, byteVectorMagma> MAVX512(key);
		fileOperation(inputPath, outputPath, mode, iV, MAVX512);
		break;
	}
	default:
		break;
	}
}



