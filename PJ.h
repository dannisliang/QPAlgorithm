//
// Created by andy_ro@qq.com
// 			5/26/2019
//
#ifndef GAME_LOGIC_PJ_H
#define GAME_LOGIC_PJ_H

#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <memory.h>
#include <list>
#include <vector>
#include <boost/get_pointer.hpp>
#include <boost/shared_ptr.hpp>

#ifndef isZero
#define isZero(a)        ((a>-0.000001) && (a<0.000001))
#endif//isZero

#define MAX_CARD_TOTAL	QZPJ::MaxCardTotal	//牌总个数
#define GAME_PLAYER		QZPJ::MaxPlayer		//最多4人局
#define MAX_COUNT		QZPJ::MaxCount		//每人2张牌
#define MAX_ROUND       QZPJ::MaxRound		//最大局数

//抢庄牌九
namespace QZPJ {

	const int MaxCardTotal	= 32;	//牌总个数
	const int MaxPlayer		= 4;	//最多4人局
	const int MaxCount		= 2;	//每人2张牌
	const int MaxRound		= 2;	//最大局数

	//单牌类型：从大到小
	//天牌 > 地牌 > 人牌 > 鹅牌 > 梅牌 > 长三 >
	//板凳 > 斧头 > 红头十 > 高脚七 > 零霖六 > 杂九 =
	//杂九 > 杂八 = 杂八 > 杂七 = 杂七 > 杂五 =
	//杂五 > 二四 = 丁三
	enum CardType {
		CNIL = -1,		//无
		TIAN,   		//天牌
		DI,				//地牌
		REN,			//人牌
		ER,				//鹅牌
		MEI,			//梅牌
		CH3,			//长三
		BD,				//板凳
		FT,				//斧头
		HT10,			//红头十
		GJ7,			//高脚七
		LL6,			//零霖六
		Z945,			//杂九(45型)
		Z936,			//杂九(36型)
		Z835,			//杂八(35型)
		Z826,			//杂八(26型)
		Z734,			//杂七(34型)
		Z725,			//杂七(25型)
		Z514,			//杂五(14型)
		Z532,			//杂五(32型)
		C24,			//二四
		CD3,			//丁三
		CARDMAX,
	};

	//对牌类型：从大到小
	//至尊 > 双天 > 双地 > 双人 > 双鹅 >
	//双梅 > 双长三 > 双板凳 > 双斧头 > 双红头 >
	//双高脚 > 双零霖 > 杂九 > 杂八 > 杂七 >
	//杂五 > 天王 > 地王 > 天杠 > 地杠 >
	//天高九 > 地高九
	enum PairType {
		PNIL = -1,		//无
		/////////// 特殊牌型 
		PZZUN,			//至尊
		PTIAN,			//双天
		PDI,			//双地
		PREN,			//双人
		PER,			//双鹅
		PMEI,			//双梅
		PCH3,			//双长三
		PBD,			//双板凳	
		PFT,			//双斧头
		PHT10,			//双红头
		PGJ7,			//双高脚
		PLL6,			//双零霖
		PZ9,			//杂九
		PZ8,			//杂八
		PZ7,			//杂七
		PZ5,			//杂五
		PTW,			//天王
		PDW,			//地王
		PTG,			//天杠
		PDG,			//地杠
		PTG9,			//天高九
		PDG9,			//地高九
		/////////// 普通牌点 
		PPT9,			//九点
		PPT8,			//八点
		PPT7,			//七点
		PPT6,			//六点
		PPT5,			//五点
		PPT4,			//四点
		PPT3,			//三点
		PPT2,			//二点
		PPT1,			//一点
		PPT0,			//零点
	};

	//游戏逻辑类
	class CGameLogic
	{
	public:
		//构造函数
		CGameLogic();
		//析构函数
		virtual ~CGameLogic();
	public:
		//初始化扑克牌数据
		void InitCards();
		//debug打印
		void DebugListCards();
		//剩余牌数
		int8_t Remaining();
		//洗牌
		void ShuffleCards();
		//发牌，生成n张玩家手牌
		void DealCards(int8_t n, uint8_t *cards);
	public:
		//0x66->PAITIAN
		static CardType CardTypeByCard(uint8_t card);
		//卡牌类型字符串 0x66->"TIAN"
		static std::string StringCardTypeByCard(uint8_t card);
	public:
		//对牌类型字符串
		static std::string StringPairType(PairType pairType);
		//TIAN->"TIAN"
		static std::string StringCardType(CardType cardType);
		//TIAN->0x66
		static uint8_t CardByCardType(CardType cardType);
		//单牌比较 0-相等 >0-card1大 <0-cards1小
		static int CompareCard(uint8_t card1, uint8_t card2);
		//手牌由大到小排序
		static void SortCards(uint8_t *cards, int n);
		//玩家手牌类型，对牌类型
		static PairType JudgePairType(uint8_t *cards);
		//玩家手牌点数
		static int CalcCardsPoints(uint8_t *cards);
		//9->PPT9
		static PairType PairTypeByPoint(int point);
		//单牌点数(牌值)
		static int GetCardValue(uint8_t card);
	public:
		//确定牌型
		static PairType GetHandCardsType(uint8_t *cards);
		//玩家比牌(闲家与庄家比牌) 0-和局 >0-cards1赢 <0-cards2赢
		static int CompareHandCards(uint8_t *cards1, uint8_t *cards2);
	private:
		int8_t index_;
		uint8_t cardsData_[MaxCardTotal];
	};
};

#endif		 


