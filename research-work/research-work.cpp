#include <iostream>
#include <iostream>
#include <vector>
#include <stdint.h>
#include <string>
#include "Magma.h"
#include "MagmaAVX512.h"
#include "Kuznechik.h"
#include "Encryptor.h"
#include <chrono>
#include <string_view>
#include <algorithm>
#include <numeric>
#define GIGABYTE 1024*1024*1024

std::ostream& operator << (std::ostream& s, const byteVectorKuznechik& block) {
	for (int i = sizeof(byteVectorKuznechik) - 1; i >= 0; --i) {
		uint8_t high, low;
		high = block.bytes[i] >> 4;
		low = block.bytes[i] & 0xf;
		s << (char)(high < 10 ? ('0' + high) : ('A' + high - 10));
		s << (char)(low < 10 ? ('0' + low) : ('A' + low - 10));
	}
	return s;
}

std::ostream& operator << (std::ostream& s, const byteVectorMagma& block) {
	for (int i = sizeof(byteVectorMagma) - 1; i >= 0; --i) {
		uint8_t high, low;
		high = block.bytes[i] >> 4;
		low = block.bytes[i] & 0xf;
		s << (char)(high < 10 ? ('0' + high) : ('A' + high - 10));
		s << (char)(low < 10 ? ('0' + low) : ('A' + low - 10));
	}
	return s;
}

std::ostream& operator << (std::ostream& s, const key& key) {
	for (int i = sizeof(key) - 1; i >= 0; --i) {
		uint8_t high, low;
		high = key.bytes[i] >> 4;
		low = key.bytes[i] & 0xf;
		s << (char)(high < 10 ? ('0' + high) : ('A' + high - 10));
		s << (char)(low < 10 ? ('0' + low) : ('A' + low - 10));
	}
	return s;
}

void test(std::vector<byteVectorMagma>& dest) {
	dest.resize(GIGABYTE / 8);
	for (size_t i = 0; i < dest.size(); ++i)
	{
		dest[i].halfsData.lo = 17721245453 * i;
		dest[i].halfsData.hi = 17721245453 * i + 13243342;
	}
}

void test2(std::vector<byteVectorKuznechik>& dest) {
	dest.resize(GIGABYTE / 16);
	for (size_t i = 0; i < dest.size(); ++i)
	{
		dest[i].halfsData.lo = 17721245453 * i;
		dest[i].halfsData.hi = 17721245453 * i + 13243342;
	}
}

void magmaTestSpeed()
{
	uint8_t expectedBlockBytesMag[] = { 0x3d, 0xca, 0xd8, 0xc2, 0xe5, 0x01, 0xe9, 0x4e };
	uint8_t testDataBytesMag[8] = { 0x10, 0x32, 0x54, 0x76, 0x98, 0xBA, 0xDC, 0xFE };
	uint8_t testKeyBytesMag[32] = { 0xff, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8, 0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1, 0xf0, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };

	byteVectorMagma testBlockMag(testDataBytesMag);
	key testKeyMag(testKeyBytesMag);

	Magma m(testKeyMag);

	byteVectorMagma expectedBlockMag(expectedBlockBytesMag);

	std::vector<byteVectorMagma> testSrcMag = { testBlockMag, testBlockMag, testBlockMag, testBlockMag, testBlockMag, testBlockMag, testBlockMag, testBlockMag };
	std::vector<byteVectorMagma> testDestMag(8);

	std::vector<byteVectorMagma> testSrcMag2 = { testBlockMag, testBlockMag, testBlockMag, testBlockMag, testBlockMag, testBlockMag, testBlockMag, testBlockMag, testBlockMag, testBlockMag, testBlockMag, testBlockMag, testBlockMag, testBlockMag, testBlockMag, testBlockMag };
	std::vector<byteVectorMagma> testDestMag2(16);


	std::cout << "test open data for Magma: " << testBlockMag << std::endl;
	std::cout << "test key fo Magma: " << testKeyMag << std::endl;
	std::cout << "expected result: " << expectedBlockMag << std::endl;

	m.processData(testSrcMag, testDestMag, 1);

	std::cout << "real result (enc): " << testDestMag[0] << std::endl;

	m.processData(testDestMag, testDestMag, 0);

	std::cout << "real result (dec): " << testDestMag[0] << std::endl;

	using duration_t = std::chrono::duration<float>;
	std::vector<float> times2;
	std::vector<float> times512;
	std::vector<byteVectorMagma> vectorForMagma;
	test(vectorForMagma);

	uint8_t c[32] = "asdafasdasdasfdasdasdakfksakfsa";
	key S(c);
	Magma m2(S);

	std::span<byteVectorMagma, GIGABYTE / 8> tmpMagma(vectorForMagma);
	for (int j = 0; j < 5; ++j) {
		std::vector<byteVectorMagma> b(GIGABYTE / 8);
		std::span<byteVectorMagma, GIGABYTE / 8> dest(b);
		auto begin = std::chrono::steady_clock::now();
		m2.processData(tmpMagma, dest, 1);
		auto end = std::chrono::steady_clock::now();
		auto time = std::chrono::duration_cast<duration_t>(end - begin);
		times2.push_back(time.count());
		std::cout << j << ": " << time.count() << std::endl;
	}

	double meanM2 = std::accumulate(times2.begin(), times2.end(), 0.0) / times2.size();
	std::cout << meanM2 << std::endl;
	std::cout << "speed algorithm Magma (AVX-2): " << 1 / meanM2 << "GBs" << std::endl;

	std::cout << "----------------------------------------------" << std::endl;
}

void kuznechikTestSpeed() {
	uint8_t expectedBlockBytesKuz[] = { 0xcd, 0xed, 0xd4, 0xb9, 0x42, 0x6d, 0x46, 0x5a, 0x30, 0x24, 0xbc, 0xbe, 0x90, 0x9d, 0x67, 0x7f };
	//uint8_t blockBytesKuz[] = { 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0x30, 0x24, 0xbc, 0xbe, 0x90, 0x9d, 0x67, 0x7f };

	uint8_t testDataBytesKuz[] = { 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11 };
	uint8_t testKeyBytesKuz[] = { 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0x10, 0x32, 0x54, 0x76, 0x98, 0xba, 0xdc, 0xfe, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00, 0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88 };
	
	byteVectorKuznechik testBlockKuz(testDataBytesKuz);
	//byteVectorKuznechik testBlockKuz2(blockBytesKuz);

	byteVectorKuznechik expectedBlockKuz(expectedBlockBytesKuz);
	key testKeyKuz(testKeyBytesKuz);

	std::vector<byteVectorKuznechik> testSrcKuz = { testBlockKuz, testBlockKuz, testBlockKuz, testBlockKuz };
	std::vector<byteVectorKuznechik> testDestKuz(4);

	Kuznechik k2(testKeyKuz);

	std::cout << "test open data for Kuznechick (AVX-2): " << testBlockKuz << std::endl;
	std::cout << "test key fo Kuznechik (AVX-2): " << testKeyKuz << std::endl;
	std::cout << "expected result (AVX-2): " << expectedBlockKuz << std::endl;

	k2.processData(testSrcKuz, testDestKuz, 1);

	std::cout << "real result (enc/AVX-2): " << testDestKuz[0] << std::endl;

	k2.processData(testDestKuz, testDestKuz, 0);

	std::cout << "real result (dec/AVX-2): " << testDestKuz[0] << std::endl;

	using duration_t = std::chrono::duration<float>;
	std::vector<float> timesKuz2;
	std::vector<float> timesKuz512;
	std::vector<byteVectorKuznechik> a;
	test2(a);

	std::span<byteVectorKuznechik, GIGABYTE / 16> aa2(a);
	for (int j = 0; j < 5; ++j) {
		std::vector<byteVectorKuznechik> b(GIGABYTE / 16);
		std::span<byteVectorKuznechik, GIGABYTE / 16> dest(b);
		uint8_t c[32] = "asdafasdasdasfdasdasdakfksakfsa";
		key S(c);
		Kuznechik k2(S);
		auto begin = std::chrono::steady_clock::now();
		k2.processData(aa2, dest, 1);
		auto end = std::chrono::steady_clock::now();
		auto time = std::chrono::duration_cast<duration_t>(end - begin);
		timesKuz2.push_back(time.count());
	}

	double meanKuz2 = std::accumulate(timesKuz2.begin(), timesKuz2.end(), 0.0) / timesKuz2.size();
	std::cout << meanKuz2 << std::endl;
	std::cout << "speed algorithm Kuznechik (AVX-2): " << 1 / meanKuz2 << "GBs" << std::endl;

	std::cout << "----------------------------------------------" << std::endl;
}

/*
void testsSpeed()
{
	
	
	

	

	

	
	


	


	


	MagmaAVX512 m512(testKeyMag);

	std::cout << "test open data for Magma (AVX-512): " << testBlockMag << std::endl;
	std::cout << "test key fo Magma (AVX-512): " << testKeyMag << std::endl;
	std::cout << "expected result (AVX-512): " << expectedBlockMag << std::endl;

	m512.encryptTextAVX512(testSrcMag2, testDestMag2, 1);

	std::cout << "real result (AVX-512): " << testDestMag2[0] << std::endl;

	std::span<byteVectorMagma, GIGABYTE / 8> tmpMagma2(vectorForMagma);
	for (int j = 0; j < 5; ++j) {
		std::vector<byteVectorMagma> b(GIGABYTE / 8);
		std::span<byteVectorMagma, GIGABYTE / 8> dest(b);
		uint8_t c[32] = "asdafasdasdasfdasdasdakfksakfsa";
		key S(c);
		MagmaAVX512 m512(S);
		auto begin = std::chrono::steady_clock::now();
		m512.encryptTextAVX512(tmpMagma2, dest, 1);
		auto end = std::chrono::steady_clock::now();
		auto time = std::chrono::duration_cast<duration_t>(end - begin);
		times512.push_back(time.count());
	}

	double meanM512 = std::accumulate(times512.begin(), times512.end(), 0.0) / times512.size();
	std::cout << meanM512 << std::endl;
	std::cout << "speed algorithm Magma (AVX-512): " << 1 / meanM512 << "GBs" << std::endl;

	std::cout << "----------------------------------------------" << std::endl;


	

	std::cout << "test open data for Kuznechick (AVX-512): " << testBlockKuz << std::endl;
	std::cout << "test key fo Kuznechik (AVX-512): " << testKeyKuz << std::endl;
	std::cout << "expected result (AVX-512): " << expectedBlockKuz << std::endl;

	k2.encryptTextAVX512(testSrcKuz, testDestKuz, 1);

	std::cout << "real result (AVX-512): " << testDestKuz[0] << std::endl;

	std::span<byteVectorKuznechik, GIGABYTE / 16> aa512(a);
	for (int j = 0; j < 5; ++j) {
		std::vector<byteVectorKuznechik> b(GIGABYTE / 16);
		std::span<byteVectorKuznechik, GIGABYTE / 16> dest(b);
		uint8_t c[32] = "asdafasdasdasfdasdasdakfksakfsa";
		key S(c);
		Kuznechik k2(S);
		auto begin = std::chrono::steady_clock::now();
		k2.encryptTextAVX512(aa512, dest, 1);
		auto end = std::chrono::steady_clock::now();
		auto time = std::chrono::duration_cast<duration_t>(end - begin);
		timesKuz512.push_back(time.count());
	}

	double meanKuz512 = std::accumulate(timesKuz512.begin(), timesKuz512.end(), 0.0) / timesKuz512.size();
	std::cout << meanKuz512 << std::endl;
	std::cout << "speed algorithm Kuznechik (AVX-512): " << 1 / meanKuz512 << "GBs" << std::endl;
}
*/

int main(int argc, char* argv[]) {
	
	//kuznechikTestSpeed();
	//__m256i a = _mm256_set1_epi8(32);
	//__m256i b = _mm256_set1_epi8(64);
	//__m256i c = _mm256_xor_si256(a, b);

	magmaTestSpeed();
}
	/*
	int algorihm = 0;
	int mode = 0;
	setlocale(LC_ALL, "");
	if (argc != 3) 
	{
		std::cout << "Некорректное количество параметров." << std::endl;
		std::cout << "Используйте: courseProject -[алгоритм шифрования] -[шифрование/дешифрование/тест скорости]" << std::endl;
		exit(0);
	}
	if (strcmp(argv[1], "-mavx2") == 0)
	{
		algorihm = 1;
	}
	else if (strcmp(argv[1], "-kavx2") == 0)
	{
		algorihm = 2;
	}
	else
	{
		std::cout << "Некорректно задан алгоритм шифрования. Используйте: -mavx2, -kavx2";
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

	if (mode == 1 || mode == 0)
	{
		while (true) {
			std::cout << "Введите ключ (32 байта): ";
			std::string testKey;
			std::cin >> testKey;
			if (testKey.size() == 32) {
				std::copy_n(testKey.c_str(), 32, key.bytes);
				break;
			}
			std::cout << "Длина ключа должна соответствовать 32 байтам" << std::endl;
		}

		std::cout << "Введите путь до входного файла: ";



		while (true) {
			std::string testPath;
			std::cin >> testPath;
			if (std::ifstream(testPath).good()) {
				inputPath = testPath;
				break;
			}
			std::cout << "Заданный файл не найден" << std::endl;
		}

		std::cout << "Введите путь до выходного файла: ";

		std::cin >> outputPath;
	}
	
	switch (algorihm)
	{
	case 1:
	{
		Encryptor<Magma, byteVectorMagma> M2(key);

		if (mode == 1)
		{
			auto begin = std::chrono::steady_clock::now();
			M2.encrypt(inputPath, outputPath);
			auto end = std::chrono::steady_clock::now();
			auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
			std::cout << "Общее время шифрования: " << time.count() << " ms" << std::endl;
		}
		else if (mode == 0)
		{
			auto begin = std::chrono::steady_clock::now();
			M2.decrypt(inputPath, outputPath);
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
		Encryptor<Kuznechik, byteVectorKuznechik> K2(key);

		if (mode == 1)
		{
			auto begin = std::chrono::steady_clock::now();
			K2.encrypt(inputPath, outputPath);
			auto end = std::chrono::steady_clock::now();
			auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
			std::cout << "Общее время шифрования: " << time.count() << " ms" << std::endl;
		}
		else if (mode == 0)
		{
			auto begin = std::chrono::steady_clock::now();
			K2.decrypt(inputPath, outputPath);
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
	default:
		break;
	}
}
*/


