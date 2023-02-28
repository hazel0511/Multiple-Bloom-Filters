#pragma once
#include <stdio.h>
#include "GeneralHashFunctions.h"

//#define bitSize 2048
//#define hashNum 2   //at most 11

//重複定義
//hash_function hf[11] = { RSHash,JSHash,PJWHash,ELFHash,BKDRHash,SDBMHash,DJBHash,DEKHash,BPHash,FNVHash,APHash };
//hash_function* hf;

class BloomFilter
{
public:
	BloomFilter(int m_byteSIze, int m_hashNum)
		:bitSize(m_byteSIze), hashNum(m_hashNum)
	{}

	// bitSize:  filter size in bits
	// hashNum(int) : # of hashes
	int bitSize;
	int hashNum;
	int byteSize = bitSize >> 3;
	//int filter[bitSize >> 3]; //不能在方括號中用變數來表示元素的個數， 但是可以是符號常數或常量運算式
	int* filter = new int[bitSize >> 3]();  // 動態記憶體配置 要delete掉,初始化為0
	unsigned int* digest = new unsigned int[hashNum]; // 動態記憶體配置 要delete掉
	/*
	//會導致_free_dbg(block, _UNKNOWN_BLOCK); exception
	~BloomFilter()
	{
		delete[] filter;
		delete[] digest;
	}
	*/
	int getByteSize(BloomFilter bf);

	void Hash(std::string value);
	void Add(std::string value);
	bool Query(std::string value);
	void Reset();
	void DisplayFilter();


};




