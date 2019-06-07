#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <algorithm>
#include "math.h"
#include <memory>
#include <utility>
#include <string.h>

#include "weights.h"

//随机种子
static struct srand_init_t {
	srand_init_t() {
		srand(time(NULL));
	}
}s_srand_init;

//RandomBetween 随机数[a,b]
extern int RandomBetween(int a, int b) {
	return a + rand() % (b - a + 1);
}

//GetResultByWeight 按权值来随机
int GetResultByWeight(int weight[], int len) {
	int sum = 0;
	for (int i = 0; i < len; ++i) {
		sum += weight[i];
	}
	for (int i = 0; i < len; ++i) {
		int r = RandomBetween(1, sum);
		if (r <= weight[i]) {
			//printf("\nv:%d < w:%d ", r, weight[i]);
			return i;
		}
		else {
			sum -= weight[i];
		}
	}
}

//权重池
CWeight::CWeight() {
	count_ = 0;
	memset(indexID_, 0, sizeof(int)*MAX_WEIGHT);
	memset(weights_, 0, sizeof(int)*MAX_WEIGHT);
}

CWeight::~CWeight() {

}

//初始化权重集合
void CWeight::init(int weight[], int len)
{
	if (len > MAX_WEIGHT) {
		printf("CWeight::init ERR: len:%d > MAX_WEIGHT:%d\n", len, MAX_WEIGHT);
		return;
	}
	for (int i = 0; i < len; ++i) {
		weights_[i] = weight[i];
		indexID_[i] = i;
	}
	count_ = len;
}

//随机权重序列
void CWeight::shuffleSeq()
{
	int c = RandomBetween(5, 10);
	for (int k = 0; k < c; ++k) {
		for (int i = 0; i < count_; ++i) {
			int j = (rand() % count_);
			if (i != j) {
				std::swap(weights_[i], weights_[j]);
				std::swap(indexID_[i], indexID_[j]);
			}
		}
	}
}

//按权值来随机
int CWeight::getResultByWeight()
{
	int i = GetResultByWeight(weights_, count_);
	//printf(" -> %d\n", indexID_[i]);
	return indexID_[i];
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//根据换牌概率权重计算当前是否换牌
//ratioExchange 换牌概率权重 
ExchangeTy CalcExchangeOrNot(int ratioExchange) {
	int weight[MaxExchange] = { ratioExchange,100 - ratioExchange };
	CWeight pool;
	pool.init(weight, MaxExchange);
	pool.shuffleSeq();
	return (ExchangeTy)pool.getResultByWeight();
}

ExchangeTy CalcExchangeOrNot2(CWeight& pool) {
	//int weight[MaxExchange] = { ratioExchange,100 - ratioExchange };
	//CWeight pool;
	//pool.init(weight, MaxExchange);
	pool.shuffleSeq();
	return (ExchangeTy)pool.getResultByWeight();
}

//测试按权重随机概率结果
//写入文件再导入Excel并插入图表查看概率正态分布情况
//filename char const* 要写入的文件 如/home/testweight.txt
void TestWeightsRatio(char const* filename) {
	while (1) {
		if ('q' == getchar()) {
			break;
		}
		remove(filename);
		FILE* fp = fopen(filename, "a");
		if (fp == NULL) {
			return;
		}
		int c = 100;			//循环总次数
		int scale = 1000;		//放大倍数
		int ratioExC = 50;		//换牌概率
		int exC = 0, noExC = 0; //换牌/不换牌分别统计次数
		CWeight pool;
		int weight[MaxExchange] = { ratioExC*scale,(100 - ratioExC)*scale };
		pool.init(weight, MaxExchange);
		for (int i = 0; i < c; ++i) {
			ExchangeTy ty = CalcExchangeOrNot2(pool);
			if (ty == Exchange) {
				++exC;
			}
			else if (ty == NoExchange) {
				++noExC;
			}
			//写入文件再导入Excel并插入图表查看概率正态分布情况
			char ch[10] = { 0 };
			sprintf(ch, "%d\t", ty == Exchange ? 1 : -1);
			fwrite(ch, strlen(ch), 1, fp);
		}
		fflush(fp);
		fclose(fp);
		printf("c:%d:%d scale:%d ratioExC:%d exC:%d:ratio:%.02f\n",
			c, exC + noExC, scale, ratioExC,
			exC, ((float)exC) / (float)(exC + noExC));
	}
}