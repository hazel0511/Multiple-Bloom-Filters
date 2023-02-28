#include <iostream>
#include <string>
#include "bloomFilter.h"


hash_function hf[11] = { RSHash,JSHash,PJWHash,ELFHash,BKDRHash,SDBMHash,DJBHash,DEKHash,BPHash,FNVHash,APHash };


int BloomFilter::getByteSize(BloomFilter bf)
{
	return sizeof(bf.filter);

}

//Create a hash of an int and get a generator of hash functions
void BloomFilter::Hash(std::string value)
{
	//std::string val = std::to_string(value) // C++11 有 std::to_string() 可以使用，要引入標頭檔 <string>
	//hash_function hf[hashNum];


	for (int i = 0; i < hashNum; i++)
	{
		//int len = sizeof(value);
		int len = value.size();

		char* val = new char[len + 1];
		strcpy_s(val, len + 1, value.c_str()); //string to char*: c_str() return的是一個臨時pointer,不能對它進行操作
		//unsigned int digest = hf[i](value.c_str(), len); //string to char : str.c_str() : const char*
		digest[i] = hf[i](val, len);
		// bitwise AND of the digest and all of the available bit positions in the filter
		digest[i] &= (bitSize - 1);

	}

}


void BloomFilter::Add(std::string value)
{

	//unsigned int digest[hashNum];

	for (int i = 0; i < hashNum; i++)
	{
		int len = value.size();

		char* val = new char[len + 1];
		strcpy_s(val, len + 1, value.c_str());
		//printf("%s\n", val);
		digest[i] = hf[i](val, len);
		//Bitwise AND of the digest and all of the available bit positions in the filter
		digest[i] &= (bitSize - 1);
		//Bitwise OR to add value(s) into the self.filter
		filter[(digest[i] >> 3)] |= (1 << (digest[i] & 7));
	}

}

// If True from a bitwise AND for each digest returned from each hash return True, else False
bool BloomFilter::Query(std::string value)
{

	for (int i = 0; i < hashNum; i++)
	{
		int len = value.size();

		char* val = new char[len + 1];
		//使用三個參數版本的strcpy_s(char(&strDestination)[size], const char *strSource)
		strcpy_s(val, value.size() + 1, value.c_str());
		digest[i] = hf[i](val, len);
		//Bitwise AND of the digest and all of the available bit positions in the filter
		digest[i] &= (bitSize - 1);
		//Bitwase AND to query values in filter
		if (!(filter[(digest[i] >> 3)] & (1 << (digest[i] & 7)))) return false;
	}

	return true;
}

void BloomFilter::Reset()
{
	//for (int i = 0; i < sizeof(bf.filter);i++)
	for (int i = 0; i < (bitSize >> 3); i++)
		filter[i] &= 0x00;

}

void BloomFilter::DisplayFilter()
{
	printf("BF byte number: %d\n", sizeof(filter) / sizeof(int));
	for (int i = 0; i < (bitSize >> 3); i++)
		printf("%d ", filter[i]);
	printf("\n");
}


/*
//Test bloomfilter
//Function: check if certain data "exist" in bloomfilter or not
int main()
{
	BloomFilter bf(2048,2);

	bf.DisplayFilter();
	bf.Add("30000");
	std::cout << "add 30000" << std::endl;
	bf.DisplayFilter();

	bf.Add("1230213");
	std::cout << "add 1230213" << std::endl;
	bf.DisplayFilter();

	std::cout << (bf.Query("213")? "True" : "False") << std::endl; //False
	std::cout << (bf.Query("30000") ? "True" : "False") << std::endl; //True
	bf.Reset();


	return 0;
}
*/