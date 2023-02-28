// MBF_02.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include <iostream>
#include <vector>
#include <map>
#include "bloomFilter.h"

#define bitSize 2048
#define hashNum 2   //at most 11


class MBF
{
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

	void decay()
	{
		//reset weights
		for (int i = 0; i < bfNum; i++)
		{
			weights[i] -= 1;
			if (weights[i] == 0) weights[i] = bfNum;

		}

		//reset decay BG
		bloomfilters[decBF].Reset();

		//set next decay BF
		decBF = (bfNum + decBF + 1) % bfNum; //round robin

	}

	double countWeight(std::string key)
	{
		double w = 0.0;
		for (int i = 0; i < bfNum; i++)
		{
			if (bloomfilters[i].Query(key))
				w += weights[i];
		}
		return (w * maxWeight / bfNum);

	}


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

	bool checkHot(std::string key)
	{
		if (countWeight(key) >= hotThres)
		{
			hotKeySet[key] = "HOT";
			//std::cout <<"hot:"<< key << std::endl;
			return true;

		}
		else return false;
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


};






int main()
{
	std::cout << "start MBF" << std::endl;
	MBF mbf(4, 2, 4, 512); //(bfNum, maxWeight, hot Threshold, decay)
	//mbf.initBloomFilter(2048, 2);
	mbf.initBloomFilter(bitSize,hashNum);

	//Cold data write
	mbf.handleWriteRequest("0511");
	//Hot data write test
	//max Weight=2, hot thershold=4 or 5時出現3次(含)以上判斷為hot
	//max Weight=4, hot thershold=10時出現4次(含)以上判斷為hot
	for (int i = 0; i < 3; i++)
		mbf.handleWriteRequest("0818");

	std::cout << ((mbf.checkHot("0818")) ? "Hot" : "Cold") << std::endl; //Hot
	std::cout << ((mbf.checkHot("0511")) ? "Hot" : "Cold") << std::endl; //Cold

	return 0;
}
