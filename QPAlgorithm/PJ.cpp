
#include <time.h>
#include <algorithm>
#include "math.h"
#include <iostream>
#include <memory>
#include <utility>
#include <sys/types.h>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <stdlib.h>

#include "PJ.h"

namespace QZPJ {
	
	//一副牌九(32张)
	uint8_t s_CardListData[MaxCardTotal] =
	{
		0x66,0x66,0x11,0x11,0x44,0x44,0x13,0x13,
		0x55,0x55,0x33,0x33,0x22,0x22,0x56,0x56,
		0x46,0x46,0x16,0x16,0x15,0x15,0x45,0x36,
		0x35,0x26,0x34,0x25,0x14,0x32,0x24,0x12,
	};

	//牌九信息  [TIAN] = {0x66, "TIAN"}
	struct CardInfo_t {
		uint8_t iType;
		std::string name;
	}s_cardInfo_tbl[CARDMAX] = {
		{ 0x66, "TIAN" },//天牌
		{ 0x11, "DI"   },//地牌
		{ 0x44, "REN"  },//人牌
		{ 0x13, "ER"   },//鹅牌
		{ 0x55, "MEI"  },//梅牌
		{ 0x33, "CH3"  },//长三
		{ 0x22, "BD"   },//板凳
		{ 0x56, "FT"   },//斧头
		{ 0x46, "HT10" },//红头十
		{ 0x16, "GJ7"  },//高脚七
		{ 0x15, "LL6"  },//零霖六
		{ 0x45, "Z945" },//杂九(45型)
		{ 0x36, "Z936" },//杂九(36型)
		{ 0x35, "Z835" },//杂八(35型)
		{ 0x26, "Z826" },//杂八(26型)
		{ 0x34, "Z734" },//杂七(34型)
		{ 0x25, "Z725" },//杂七(25型)
		{ 0x14, "Z514" },//杂五(14型)
		{ 0x32, "Z532" },//杂五(32型)
		{ 0x24, "C24"  },//二四
		{ 0x12, "CD3"  },//丁三
	};

	//构造函数
	CGameLogic::CGameLogic()
	{
		index_ = 0;
		memset(cardsData_, 0, sizeof(uint8_t)*MaxCardTotal);
	}

	//析构函数
	CGameLogic::~CGameLogic()
	{

	}

	//初始化扑克牌数据
	void CGameLogic::InitCards()
	{
		//printf("--- *** 初始化一副牌九...\n");
		memcpy(cardsData_, s_CardListData, sizeof(uint8_t)*MaxCardTotal);
	}

	//debug打印
	void CGameLogic::DebugListCards() {
		for (int i = 0; i < MaxCardTotal; ++i) {
			printf("%02X %s %d\n", cardsData_[i], StringCardTypeByCard(cardsData_[i]).c_str(), GetCardValue(cardsData_[i]));
		}
	}

	//剩余牌数
	int8_t CGameLogic::Remaining() {
		return int8_t(MaxCardTotal - index_);
	}

	//洗牌
	void CGameLogic::ShuffleCards()
	{
		//printf("-- *** 洗牌...\n");
		for (int k = 0; k < 5; ++k) {
			for (int i = 0; i < MaxCardTotal; ++i) {
				int j = rand() % MaxCardTotal;
				swap(cardsData_[i], cardsData_[j]);
			}
		}
		index_ = 0;
	}

	//发牌，生成n张玩家手牌
	void CGameLogic::DealCards(int8_t n, uint8_t *cards)
	{
		//printf("-- *** %d张余牌，发牌 ...\n", Remaining());
		if (cards == NULL) {
			return;
		}
		if (n > Remaining()) {
			return;
		}
		int k = 0;
		for (int i = index_; i < index_ + n; ++i) {
			cards[k++] = cardsData_[i];
		}
		index_ += n;
	}

	//0x66->PAITIAN
	CardType CGameLogic::CardTypeByCard(uint8_t card)
	{
		CardType cardtype = CNIL;
		switch (card) {
		case 0x66: cardtype = TIAN; break;
		case 0x11: cardtype = DI; break;
		case 0x44: cardtype = REN; break;
		case 0x13: cardtype = ER; break;
		case 0x55: cardtype = MEI; break;
		case 0x33: cardtype = CH3; break;
		case 0x22: cardtype = BD; break;
		case 0x56: cardtype = FT; break;
		case 0x46: cardtype = HT10; break;
		case 0x16: cardtype = GJ7; break;
		case 0x15: cardtype = LL6; break;
		case 0x45: cardtype = Z945; break;
		case 0x36: cardtype = Z936; break;
		case 0x35: cardtype = Z835; break;
		case 0x26: cardtype = Z826; break;
		case 0x34: cardtype = Z734; break;
		case 0x25: cardtype = Z725; break;
		case 0x14: cardtype = Z514; break;
		case 0x32: cardtype = Z532; break;
		case 0x24: cardtype = C24; break;
		case 0x12: cardtype = CD3; break;
		}
		return cardtype;
	}

	//卡牌类型字符串 0x66->"TIAN"
	std::string CGameLogic::StringCardTypeByCard(uint8_t card) {
		std::string cardtype = "CNIL";
		switch (card)
		{
		case 0x66: cardtype = "TIAN"; break;
		case 0x11: cardtype = "DI"; break;
		case 0x44: cardtype = "REN"; break;
		case 0x13: cardtype = "ER"; break;
		case 0x55: cardtype = "MEI"; break;
		case 0x33: cardtype = "CH3"; break;
		case 0x22: cardtype = "BD"; break;
		case 0x56: cardtype = "FT"; break;
		case 0x46: cardtype = "HT10"; break;
		case 0x16: cardtype = "GJ7"; break;
		case 0x15: cardtype = "LL6"; break;
		case 0x45: cardtype = "Z945"; break;
		case 0x36: cardtype = "Z936"; break;
		case 0x35: cardtype = "Z835"; break;
		case 0x26: cardtype = "Z826"; break;
		case 0x34: cardtype = "Z734"; break;
		case 0x25: cardtype = "Z725"; break;
		case 0x14: cardtype = "Z514"; break;
		case 0x32: cardtype = "Z532"; break;
		case 0x24: cardtype = "C24"; break;
		case 0x12: cardtype = "CD3"; break;
		}
		return cardtype;
	}

	//对牌类型字符串
	std::string CGameLogic::StringPairType(PairType pairType)
	{
		std::string spairtype = "PNIL";
		switch (pairType)
		{
			/////////// 特殊牌型
		case PNIL:  spairtype = "PNIL"; break;		//无
		case PZZUN: spairtype = "PZZUN"; break;	//至尊
		case PTIAN: spairtype = "PTIAN"; break;	//双天
		case PDI:   spairtype = "PDI"; break;		//双地
		case PREN:	 spairtype = "PREN"; break;		//双人
		case PER:	 spairtype = "PER"; break;		//双鹅
		case PMEI:	 spairtype = "PMEI"; break;		//双梅
		case PCH3:	 spairtype = "PCH3"; break;		//双长三
		case PBD:	 spairtype = "PBD"; break;		//双板凳
		case PFT:	 spairtype = "PFT"; break;		//双斧头
		case PHT10: spairtype = "PHT10"; break;	//双红头
		case PGJ7:	 spairtype = "PGJ7"; break;		//双高脚
		case PLL6:	 spairtype = "PLL6"; break;		//双零霖
		case PZ9:	 spairtype = "PZ9"; break;		//杂九
		case PZ8:	 spairtype = "PZ8"; break;		//杂八
		case PZ7:	 spairtype = "PZ7"; break;		//杂七
		case PZ5:	 spairtype = "PZ5"; break;		//杂五
		case PTW:	 spairtype = "PTW"; break;		//天王
		case PDW:	 spairtype = "PDW"; break;		//地王
		case PTG:	 spairtype = "PTG"; break;		//天杠
		case PDG:	 spairtype = "PDG"; break;		//地杠
		case PTG9:	 spairtype = "PTG9"; break;		//天高九
		case PDG9:	 spairtype = "PDG9"; break;		//地高九
		/////////// 普通牌点 
		case PPT9:	 spairtype = "PPT9"; break;		//九点
		case PPT8:	 spairtype = "PPT8"; break;		//八点
		case PPT7:	 spairtype = "PPT7"; break;		//七点
		case PPT6:	 spairtype = "PPT6"; break;		//六点
		case PPT5:	 spairtype = "PPT5"; break;		//五点
		case PPT4:	 spairtype = "PPT4"; break;		//四点
		case PPT3:	 spairtype = "PPT3"; break;		//三点
		case PPT2:	 spairtype = "PPT2"; break;		//二点
		case PPT1:	 spairtype = "PPT1"; break;		//一点
		case PPT0:	 spairtype = "PPT0"; break;		//零点
		}
		return spairtype;
	}

	//TIAN->"TIAN"
	std::string CGameLogic::StringCardType(CardType cardType)
	{
		return s_cardInfo_tbl[cardType].name;
	}

	//TIAN->0x66
	uint8_t CGameLogic::CardByCardType(CardType cardType)
	{
		return s_cardInfo_tbl[cardType].iType;
	}

	//单牌比较 0-相等 >0-card1大 <0-cards1小
	int CGameLogic::CompareCard(uint8_t card1, uint8_t card2)
	{
		//单牌大小
		//天牌 > 地牌 > 人牌 > 鹅牌 > 梅牌 > 长三 >
		//板凳 > 斧头 > 红头十 > 高脚七 > 零霖六 > 杂九 =
		//杂九 > 杂八 = 杂八 > 杂七 = 杂七 > 杂五 =
		//杂五 > 二四 = 丁三
		CardType t1 = CardTypeByCard(card1);
		CardType t2 = CardTypeByCard(card2);
		if (t1 == t2) {
			return 0; //card1==card2
		}
		int d = std::abs(t1 - t2);
		if (d == 1) {//相邻
			int m = std::max(t1, t2);
			if (m == Z936 || m == Z826 || m == Z725 || m == Z532 || m == CD3)
				return 0; //card1==card2
		}
		return t2 - t1;
	}

	//单牌大小
	static bool compareByCardType(uint8_t card1, uint8_t card2) {
		int t1 = CGameLogic::CardTypeByCard(card1);
		int t2 = CGameLogic::CardTypeByCard(card2);
		return t2 > t1;
	}

	//手牌由大到小排序
	void CGameLogic::SortCards(uint8_t *cards, int n)
	{
		std::sort(cards, cards + n, compareByCardType);
	}

	//玩家手牌类型，对牌类型
	PairType CGameLogic::JudgePairType(uint8_t *cards) {
		PairType t = PNIL;
		//手牌由大到小排序
		//SortCards(cards, MAX_PLAYER_CARD);
		uint8_t t1 = CardTypeByCard(cards[0]);
		uint8_t t2 = CardTypeByCard(cards[1]);
		if (t1 == C24 && t2 == CD3)
			t = PZZUN;	//至尊
		else if (t1 == TIAN && t2 == TIAN)
			t = PTIAN;	//双天
		else if (t1 == DI && t2 == DI)
			t = PDI;	//双地
		else if (t1 == REN && t2 == REN)
			t = PREN;	//双人
		else if (t1 == ER && t2 == ER)
			t = PER;	//双鹅
		else if (t1 == MEI && t2 == MEI)
			t = PMEI;	//双梅
		else if (t1 == CH3 && t2 == CH3)
			t = PCH3;	//双长三
		else if (t1 == BD && t2 == BD)
			t = PBD;	//双板凳
		else if (t1 == FT && t2 == FT)
			t = PFT;	//双斧头
		else if (t1 == HT10 && t2 == HT10)
			t = PHT10;	//双红头
		else if (t1 == GJ7 && t2 == GJ7)
			t = PGJ7;	//双高脚
		else if (t1 == LL6 && t2 == LL6)
			t = PLL6;	//双零霖
		else if (t1 == Z945 && t2 == Z936)
			t = PZ9;	//杂九
		else if (t1 == Z835 && t2 == Z826)
			t = PZ8;	//杂八
		else if (t1 == Z734 && t2 == Z725)
			t = PZ7;	//杂七
		else if (t1 == Z514 && t2 == Z532)
			t = PZ5;	//杂五
		else if (t1 == TIAN && (t2 == Z945 || t2 == Z936))
			t = PTW;	//天王
		else if (t1 == DI && (t2 == Z945 || t2 == Z936))
			t = PDW;	//地王
		else if (t1 == TIAN && (t2 == REN || t2 == Z835 || t2 == Z826))
			t = PTG;	//天杠
		else if (t1 == DI && (t2 == REN || t2 == Z835 || t2 == Z826))
			t = PDG;	//地杠
		else if (t1 == TIAN && t2 == Z725)
			t = PTG9;	//天高九
		else if (t1 == DI && t2 == GJ7)
			t = PDG9;	//地高九
		return t;
	}

	//玩家手牌点数
	int CGameLogic::CalcCardsPoints(uint8_t *cards)
	{
		//手牌由大到小排序
		//SortCards(cards, MAX_COUNT);
		return (GetCardValue(cards[0]) + GetCardValue(cards[1])) % 10;
	}

	//9->PPT9
	PairType CGameLogic::PairTypeByPoint(int point)
	{
		PairType pairtype = PNIL;
		switch (point)
		{
		case 9: pairtype = PPT9; break;
		case 8: pairtype = PPT8; break;
		case 7: pairtype = PPT7; break;
		case 6: pairtype = PPT6; break;
		case 5: pairtype = PPT5; break;
		case 4: pairtype = PPT4; break;
		case 3: pairtype = PPT3; break;
		case 2: pairtype = PPT2; break;
		case 1: pairtype = PPT1; break;
		case 0: pairtype = PPT0; break;
		}
		return pairtype;
	}

	//单牌点数(牌值)
	int CGameLogic::GetCardValue(uint8_t card) {
		return (card & 0x0F) + ((card >> 4) & 0x0F);
	}

	//玩家比牌(闲家与庄家比牌) 0-和局 >0-cards1赢 <0-cards2赢
	int CGameLogic::CompareHandCards(uint8_t *cards1, uint8_t *cards2)
	{
		//手牌由大到小排序
		//SortCards(cards1, MAX_COUNT);
		//SortCards(cards2, MAX_COUNT);
		//判断玩家手牌类型
		PairType pairType1 = JudgePairType(cards1);
		PairType pairType2 = JudgePairType(cards2);
		//有对牌且牌型不同
		if (pairType1 != pairType2) {
			if (pairType1 == PNIL) {
				return -1;
			}
			if (pairType2 == PNIL) {
				return 1;
			}
			return pairType2 - pairType1;
		}
		//pairType1 == pairType2 != nil 有对牌且牌型相同
		if (pairType1 != PNIL) {
			return 0;//和局
		}
		//pairType1 == pairType2 == nil 无对牌，两牌点数和取个位数来进行比牌 
		int pt1 = CalcCardsPoints(cards1);
		int pt2 = CalcCardsPoints(cards2);
		//点数不同
		if (pt1 != pt2) {
			return pt1 - pt2;
		}
		//点数相同，比较单牌最大牌大小
		int result = CompareCard(cards1[0], cards2[0]);
		if (result != 0) {
			return result;
		}
		//result == 0 单牌最大牌大小相同，比较单牌最小牌大小
		result = CompareCard(cards1[1], cards2[1]);
		if (result != 0) {
			return result;
		}
		//result == 0 单牌最小牌大小相同
		return 0;//和局
	}
	
	//确定牌型
	PairType CGameLogic::GetHandCardsType(uint8_t *cards)
	{
		PairType pairType = JudgePairType(cards);
		if (pairType != PNIL) {
			return pairType;//特殊牌型
		}
		pairType = PairTypeByPoint(CalcCardsPoints(cards));
		return pairType;//普通牌点
	}
};