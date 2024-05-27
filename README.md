# Multiple Bloom Filter
## Basic Concept
This is a simple C++ implementation of multiple bloom filters, which can be used for hot/cold data identification. 
We use multiple Bloom filters (BFs) with different recency weights. 
Each write request is hashed and recorded in one of the BFs in a round-robin fashion. Periodically, the oldest BF is reset to reflect data decay.

### BloomFilter
Traditional Bloom filter is a probabilistic data structure used to test whether an element is a member of a set. 
![](https://i.imgur.com/npPE5BE.png)

value recorded by hash function


Bloom filter 是一種機率資料結構（probabilistic data structure），類似於集合，常用於需**快速驗證成員是否「可能存在」或是「絕對不存在」在容器中**，亦即有機會出現false positive，但絕不會有假陰性false negative。

### Multiple Bloomfilter
![](https://i.imgur.com/18NocsG.png)

多個Bloomfilter以盡量降低false positive

MBF以Round Robin輪流方式在下一個BF的相同位置傳遞bit(1)
#### 決定hot/cold data
* Frequency：Sequential Examination每個BF是否記錄了相應的值
Sequential Examination：按順序（輪流）檢查其他BF，直到找到一個新的沒有記錄該值的BF。
* Recency：在一段period內，兩個value在MBF的數量是相同的，我們需要比較哪個值在更近的時間內被access。
在T（定義為固定數量的write request）之後，在最長的時間間隔內沒有被選擇的BF，其BF（圖中為BFV）中的所有bit被重置為0
重置的BF(BFV)只能記住在最近的一個時間間隔T內訪問的value資訊，重置的BF記錄了最近(most recent)的access資料，所以必須給它分配最高的recency weight。
* 結合Frequency和Recency的值作為hot data index，如果hot data index的值大於或等於一個閾值，我們將該資料標記為Hot。

## Code
### Single bloomfilter
```CPP=
#include "GeneralHashFunctions.h"

//#define bitSize 2048
//#define hashNum 2   //at most 11

//引用GeneralHashFunctions.h裡的hash function,最多11個
//hash_function hf[11] = { RSHash,JSHash,PJWHash,ELFHash,BKDRHash,SDBMHash,DJBHash,DEKHash,BPHash,FNVHash,APHash };

class BloomFilter
{
public:
	BloomFilter(int m_byteSIze, int m_hashNum)
		:bitSize(m_byteSIze), hashNum(m_hashNum)
	{}

	int bitSize;
	int hashNum;
	int byteSize = bitSize >> 3;
	int* filter = new int[bitSize >> 3]();  
	unsigned int* digest = new unsigned int[hashNum]; 

	int getByteSize(BloomFilter bf);

	void Hash(std::string value); //
	void Add(std::string value);
	bool Query(std::string value);
	void Reset();
	void DisplayFilter();


};


```
Bloomfilter.h/Bloomfilter.cpp檔在MBF中需要的功能:
* Hash: hash value
```CPP=
//Create a hash of an int and get a generator of hash functions
void BloomFilter::Hash(std::string value)
{

	for (int i = 0; i < hashNum; i++)
	{
		int len = value.size();

		char* val = new char[len + 1];
		strcpy_s(val, len + 1, value.c_str()); 
        
		digest[i] = hf[i](val, len);
		// bitwise AND of the digest and all of the available bit positions in the filter
		digest[i] &= (bitSize - 1);

	}

}

```

* Add:放入bloom filter紀錄
```CPP=
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
```
* Query:查詢是否在Bloomfilter中
```CPP=
// If all hashes return True from a bitwise AND for each digest returned from hash return True, else False
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

```

### Multiple Bloomfilter
#### 初始化成員
```CPP=
#include "bloomFilter.h"
class MBF{
    public:
	MBF(int _bfNum, int _maxWeight, int _hotThres, int _decay)
	{
		std::cout << "new MBF" << std::endl;
		bfNum = _bfNum;
		maxWeight = _maxWeight;
		hotThres = _hotThres;
		decayWindow = _decay;

		curBF = 0;
		decBF = 0;

		//std::vector<double> weights;
		for (int i = 0; i < bfNum; i++)
		{
			weights.push_back(i + 1.0);
		}
		reqNum = 0;

	}

	void initBloomFilter(int _bitSize, int _hashNum)
	{
		for (int i = 0; i < bfNum; i++)
		{
			BloomFilter new_bf(_bitSize, _hashNum);
			bloomfilters.push_back(new_bf);
		}
	}
    private:
	int bfNum;
	int maxWeight;
	int hotThres;
	int decayWindow;
	std::vector<BloomFilter> bloomfilters;
	int curBF = 0;
	int decBF = 0;
	std::vector<double> weights;
	int reqNum;

	//kv pair對應:<key,"VALID">
	std::map<std::string, std::string> keySet;
	//kv pair對應:<key,"HOT"> or <key,"COLD">
	std::map<std::string, std::string> hotKeySet;
}
```
#### Decay with Recency
```CPP=
	void decay()
	{
		//reset weights
		for (int i = 0; i < bfNum; i++)
		{
			weights[i] -= 1;
            //當權重扣到0代表跑完一round Bloomfilter
			if (weights[i] == 0) weights[i] = bfNum;

		}

		//reset decay BF
		bloomfilters[decBF].Reset();

		//set next decay BF
		decBF = (bfNum + decBF + 1) % bfNum; //round robin

	}
```
decay:每過一個interval便把權重減1然後傳遞到下一個bloom filter
#### Count 計算權重
```CPP=
	double countWeight(std::string key)
	{
		double w = 0.0;
        //累計出現資料的權重
		for (int i = 0; i < bfNum; i++)
		{
			if (bloomfilters[i].Query(key))
				w += weights[i];
		}
		return (w * maxWeight / bfNum);
	}
```
Count weight 是把recency 加權值 Scale up 乘上maxWeight再除以BF數量作為hot data的判斷依據

#### 寫入MBF
```CPP=
	void handleWriteRequest(std::string key)
	{
		reqNum += 1;
		keySet[key] = "VALID";

		//already done certain # of write, decay
		if (reqNum % decayWindow == 0)
			decay();

		if (bloomfilters[curBF].Query(key))
		{
			int nextBF = (curBF + 1) % bfNum;
			while (nextBF != curBF)
			{
			//Sequential Examination
                        if (bloomfilters[nextBF].Query(key))
				{
					
                        //Keep find next BF without any info about this value
					nextBF = (nextBF + 1) % bfNum;
				}
				else
				{
					//write hashed value to BF
					bloomfilters[nextBF].Add(key);
					break;
				}
			}
		}
		else
			bloomfilters[curBF].Add(key);

		curBF = (curBF + 1) % bfNum;
	}
```

reqNum: 記錄request數量,因為decay是**以request的次數作為time interval而不是固定時間**為依據
sequential examination: 不斷query下一個bloom filter直到當前位置上bit是0(沒有紀錄資訊)就呼叫bloomfilter.Add把它加上去

More details can be found in paper "Hot Data Identification for Flash-based Storage Systems Using Multiple Bloom Filters."[^1]
## General Purpose Hash Function Algorithms Library   
All hash functions are based on GeneralHashFunctions.py from https://github.com/JamzyWang/HashCollector/blob/master/GeneralHashFunctions_Python/GeneralHashFunctions.py.

we have:

RSHash (Robert Sedgwicks Hash)
+ Utilizes a prime multiplier and combines it with the current hash value using bitwise operations.
+ Provides good distribution and was designed for use in educational contexts.


JSHash (JavaScript Hash)
+ Uses bitwise operations and shifts to mix the input string's characters.
+ Designed to be efficient and provide a good distribution for typical JavaScript applications.

PJWHash (Peter J. Weinberger Hash)
+ Splits the hash value into parts and uses bitwise operations to mix these parts.
+ Designed to minimize collisions in small hash tables.



ELFHash (Extended Library Function Hash)
+ Similar to PJWHash but optimized for use in the ELF file format.
+ Efficient and suitable for use in file hashing.



BKDRHash (Brian Kernighan and Dennis Ritchie Hash)
+ Uses a seed (often 31, which is a prime number) and iterates over each character of the string, combining it with the hash value calculated so far.
+ Simple and produces good distribution for small inputs.


SDBMHash (Stack DataBase Manager Hash)
+ Iterates over each character, combining it with the current hash value using bitwise operations and multiplication.
+ Produces good distribution and is simple to implement.

DJBHash (Daniel J. Bernstein Hash)
+ Starts with a large prime (5381) and for each character, multiplies the current hash value by 33 and adds the character's ASCII value.
+ Simple and fast with good distribution properties for small to medium-sized strings.



DEKHash (Donald E. Knuth Hash)
+ Uses bitwise shifts and XOR operations to mix the input characters.
+ Simple and effective, designed to provide a good distribution.

BPHash (Brian Kernighan and Dennis Ritchie Hash)
+ Uses a prime multiplier and combines it with the current hash value and character's ASCII value using bitwise operations.
+ Designed for simplicity and good distribution.


FNVHash (Fowler-Noll-Vo Hash)
+ Combines the current hash value with a prime multiplier and the character's ASCII value using bitwise operations.
+ Extremely fast and provides good distribution, especially for small inputs.



APHash (Arash Partow Hash)
+ Alternates between two operations for even and odd characters, which include bitwise shifts and additions.
+ Aims for better distribution by mixing input characters differently based on their positions.

## Use
For Multiple Bloom Filter, we have to assign number of bloom filter, max weight, hot threshold, decay period.

For each bloom filter initialization, we also need to give bitSize(here we set 2048) and nubmer of hash function(at most 11 kinds of hash function).

All these parameters are tunable so that we can decide how recent it appears again qualifys "Hot Data".

After that we can perform write request and check it is hot or not.

[^1]: Park D, Du D H C. Hot data identification for flash-based storage systems using multiple bloom filters. In Proc. the 27th IEEE Symp. Mass Storage Systems and Technologies, May 2011.
