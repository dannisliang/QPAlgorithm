#ifndef WEIGHTS_H
#define WEIGHTS_H

#define MAX_WEIGHT 10

//随机数[a,b]
extern int RandomBetween(int a, int b);

//按权值来随机
extern int GetResultByWeight(int weight[], int len);

//权重池
class CWeight {
public:
	CWeight();
	~CWeight();
public:
	//初始化权重集合
	void init(int weight[], int len);
	//随机权重序列
	void shuffleSeq();
	//按权值来随机
	int getResultByWeight();
public:
	int count_;				 //统计个数
	int weights_[MAX_WEIGHT];//权重集合
	int indexID_[MAX_WEIGHT];//对应索引
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//换牌类型
enum ExchangeTy {
	Exchange = 0,	//换牌
	NoExchange,		//不换
	MaxExchange,
};

//根据换牌概率权重计算当前是否换牌
//ratioExchange 换牌概率权重 
extern ExchangeTy CalcExchangeOrNot(int ratioExchange);
extern ExchangeTy CalcExchangeOrNot2(CWeight& pool);

//测试按权重随机概率结果
//写入文件再导入Excel并插入图表查看概率正态分布情况
//filename char const* 要写入的文件 如/home/testweight.txt
extern void TestWeightsRatio(char const* filename);

#endif
