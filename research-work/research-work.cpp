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

bool checkFile(const std::string& path)
{
	return std::filesystem::exists(path);
}

int main(int argc, char* argv[]) {
	int algorihm = 0;
	int mode = 0;


	// use this instead of constants
	enum Mode {
		ENCRYPT,
		DECRYPT,
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
	}
	case 2:
	{
		Encryptor<Kuznechik, byteVectorKuznechik> K2(key);
		if (mode == 1)
		{
			auto begin = std::chrono::steady_clock::now();
			K2.encrypt(inputPath, outputPath, iV);
			auto end = std::chrono::steady_clock::now();
			auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
			std::cout << "Общее время шифрования: " << time.count() << " ms" << std::endl;
		}
		else if (mode == 0)
		{
			auto begin = std::chrono::steady_clock::now();
			K2.decrypt(inputPath, outputPath, iV);
			auto end = std::chrono::steady_clock::now();
			auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
			std::cout << "Общее время дешифрования: " << time.count() << " ms" << std::endl;
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
		break;
	}
	default:
		break;
	}
}



