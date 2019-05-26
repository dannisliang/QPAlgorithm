
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

#include "zjh.h"

namespace ZJH {
	
	//一副扑克(52张)
	uint8_t s_CardListData[MaxCardTotal] =
	{
		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D, // 方块 A - K
		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D, // 梅花 A - K
		0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D, // 红桃 A - K
		0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D, // 黑桃 A - K
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
		//printf("--- *** 初始化一副扑克...\n");
		memcpy(cardsData_, s_CardListData, sizeof(uint8_t)*MaxCardTotal);
	}

	//debug打印
	void CGameLogic::DebugListCards() {
		SortCards(cardsData_, MaxCardTotal, false, false, false);
		for (int i = 0; i < MaxCardTotal; ++i) {
			printf("%02X %s\n", cardsData_[i], StringCard(cardsData_[i]).c_str());
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
		int c = rand()%20+5;
		for (int k = 0; k < c; ++k) {
			for (int i = 0; i < MaxCardTotal; ++i) {
				int j = rand() % MaxCardTotal;
				if (i != j) {
					swap(cardsData_[i], cardsData_[j]);
				}
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

	//花色：黑>红>梅>方
	uint8_t CGameLogic::GetCardColor(uint8_t card) {
		return (card & 0xF0);
	}

	//牌值：A<2<3<4<5<6<7<8<9<10<J<Q<K
	uint8_t CGameLogic::GetCardValue(uint8_t card) {
		return (card & 0x0F);
	}
	
	//点数：2<3<4<5<6<7<8<9<10<J<Q<K<A
	uint8_t CGameLogic::GetCardPoint(uint8_t card) {
		uint8_t value = GetCardValue(card);
		return value == 0x01 ? 0x0E : value;
	}
	
	//花色和牌值构造单牌
	uint8_t CGameLogic::MakeCardWith(uint8_t color, uint8_t value) {
		return (GetCardColor(color) | GetCardValue(value));
	}

	//牌值大小：A<2<3<4<5<6<7<8<9<10<J<Q<K
	static bool byCardValueColorGG(uint8_t card1, uint8_t card2) {
		uint8_t v0 = CGameLogic::GetCardValue(card1);
		uint8_t v1 = CGameLogic::GetCardValue(card2);
		if (v0 != v1) {
			//牌值不同比大小
			return v0 > v1;
		}
		//牌值相同比花色
		uint8_t c0 = CGameLogic::GetCardColor(card1);
		uint8_t c1 = CGameLogic::GetCardColor(card2);
		return c0 > c1;
	}

	static bool byCardValueColorGL(uint8_t card1, uint8_t card2) {
		uint8_t v0 = CGameLogic::GetCardValue(card1);
		uint8_t v1 = CGameLogic::GetCardValue(card2);
		if (v0 != v1) {
			//牌值不同比大小
			return v0 > v1;
		}
		//牌值相同比花色
		uint8_t c0 = CGameLogic::GetCardColor(card1);
		uint8_t c1 = CGameLogic::GetCardColor(card2);
		return c0 < c1;
	}

	static bool byCardValueColorLG(uint8_t card1, uint8_t card2) {
		uint8_t v0 = CGameLogic::GetCardValue(card1);
		uint8_t v1 = CGameLogic::GetCardValue(card2);
		if (v0 != v1) {
			//牌值不同比大小
			return v0 < v1;
		}
		//牌值相同比花色
		uint8_t c0 = CGameLogic::GetCardColor(card1);
		uint8_t c1 = CGameLogic::GetCardColor(card2);
		return c0 > c1;
	}

	static bool byCardValueColorLL(uint8_t card1, uint8_t card2) {
		uint8_t v0 = CGameLogic::GetCardValue(card1);
		uint8_t v1 = CGameLogic::GetCardValue(card2);
		if (v0 != v1) {
			//牌值不同比大小
			return v0 < v1;
		}
		//牌值相同比花色
		uint8_t c0 = CGameLogic::GetCardColor(card1);
		uint8_t c1 = CGameLogic::GetCardColor(card2);
		return c0 < c1;
	}

	//牌点大小：2<3<4<5<6<7<8<9<10<J<Q<K<A
	static bool byCardPointColorGG(uint8_t card1, uint8_t card2) {
		uint8_t p0 = CGameLogic::GetCardPoint(card1);
		uint8_t p1 = CGameLogic::GetCardPoint(card2);
		if (p0 != p1) {
			//牌点不同比大小
			return p0 > p1;
		}
		//牌点相同比花色
		uint8_t c0 = CGameLogic::GetCardColor(card1);
		uint8_t c1 = CGameLogic::GetCardColor(card2);
		return c0 > c1;
	}

	static bool byCardPointColorGL(uint8_t card1, uint8_t card2) {
		uint8_t p0 = CGameLogic::GetCardPoint(card1);
		uint8_t p1 = CGameLogic::GetCardPoint(card2);
		if (p0 != p1) {
			//牌点不同比大小
			return p0 > p1;
		}
		//牌点相同比花色
		uint8_t c0 = CGameLogic::GetCardColor(card1);
		uint8_t c1 = CGameLogic::GetCardColor(card2);
		return c0 < c1;
	}

	static bool byCardPointColorLG(uint8_t card1, uint8_t card2) {
		uint8_t p0 = CGameLogic::GetCardPoint(card1);
		uint8_t p1 = CGameLogic::GetCardPoint(card2);
		if (p0 != p1) {
			//牌点不同比大小
			return p0 < p1;
		}
		//牌点相同比花色
		uint8_t c0 = CGameLogic::GetCardColor(card1);
		uint8_t c1 = CGameLogic::GetCardColor(card2);
		return c0 > c1;
	}

	static bool byCardPointColorLL(uint8_t card1, uint8_t card2) {
		uint8_t p0 = CGameLogic::GetCardPoint(card1);
		uint8_t p1 = CGameLogic::GetCardPoint(card2);
		if (p0 != p1) {
			//牌点不同比大小
			return p0 < p1;
		}
		//牌点相同比花色
		uint8_t c0 = CGameLogic::GetCardColor(card1);
		uint8_t c1 = CGameLogic::GetCardColor(card2);
		return c0 < c1;
	}

	//手牌排序(默认按牌点降序排列)，先比牌值/点数，再比花色
	//byValue bool false->按牌点 true->按牌值
	//ascend bool false->降序排列(即从大到小排列) true->升序排列(即从小到大排列)
	//clrAscend bool false->花色降序(即黑桃到方块) true->花色升序(即从方块到黑桃)
	void CGameLogic::SortCards(uint8_t *cards, int n, bool byValue, bool ascend, bool clrAscend)
	{
		if (byValue) {
			if (ascend) {
				if (clrAscend) {
					//LL
					std::sort(cards, cards + n, byCardValueColorLL);
				}
				else {
					//LG
					std::sort(cards, cards + n, byCardValueColorLG);
				}
			}
			else {
				if (clrAscend) {
					//GL
					std::sort(cards, cards + n, byCardValueColorGL);
				}
				else {
					//GG
					std::sort(cards, cards + n, byCardValueColorGG);
				}
			}
		}
		else {
			if (ascend) {
				if (clrAscend) {
					//LL
					std::sort(cards, cards + n, byCardPointColorLL);
				}
				else {
					//LG
					std::sort(cards, cards + n, byCardPointColorLG);
				}
			}
			else {
				if (clrAscend) {
					//GL
					std::sort(cards, cards + n, byCardPointColorGL);
				}
				else {
					//GG
					std::sort(cards, cards + n, byCardPointColorGG);
				}
			}
		}
	}

	//牌值大小：A<2<3<4<5<6<7<8<9<10<J<Q<K
	static bool byCardColorValueGG(uint8_t card1, uint8_t card2) {
		uint8_t c0 = CGameLogic::GetCardColor(card1);
		uint8_t c1 = CGameLogic::GetCardColor(card2);
		if (c0 != c1) {
			//花色不同比花色
			return c0 > c1;
		}
		//花色相同比牌值
		uint8_t v0 = CGameLogic::GetCardValue(card1);
		uint8_t v1 = CGameLogic::GetCardValue(card2);
		return v0 > v1;
	}

	static bool byCardColorValueGL(uint8_t card1, uint8_t card2) {
		uint8_t c0 = CGameLogic::GetCardColor(card1);
		uint8_t c1 = CGameLogic::GetCardColor(card2);
		if (c0 != c1) {
			//花色不同比花色
			return c0 > c1;
		}
		//花色相同比牌值
		uint8_t v0 = CGameLogic::GetCardValue(card1);
		uint8_t v1 = CGameLogic::GetCardValue(card2);
		return v0 < v1;
	}

	static bool byCardColorValueLG(uint8_t card1, uint8_t card2) {
		uint8_t c0 = CGameLogic::GetCardColor(card1);
		uint8_t c1 = CGameLogic::GetCardColor(card2);
		if (c0 != c1) {
			//花色不同比花色
			return c0 < c1;
		}
		//花色相同比牌值
		uint8_t v0 = CGameLogic::GetCardValue(card1);
		uint8_t v1 = CGameLogic::GetCardValue(card2);
		return v0 > v1;
	}

	static bool byCardColorValueLL(uint8_t card1, uint8_t card2) {
		uint8_t c0 = CGameLogic::GetCardColor(card1);
		uint8_t c1 = CGameLogic::GetCardColor(card2);
		if (c0 != c1) {
			//花色不同比花色
			return c0 < c1;
		}
		//花色相同比牌值
		uint8_t v0 = CGameLogic::GetCardValue(card1);
		uint8_t v1 = CGameLogic::GetCardValue(card2);
		return v0 < v1;
	}

	//牌点大小：2<3<4<5<6<7<8<9<10<J<Q<K<A
	static bool byCardColorPointGG(uint8_t card1, uint8_t card2) {
		uint8_t c0 = CGameLogic::GetCardColor(card1);
		uint8_t c1 = CGameLogic::GetCardColor(card2);
		if (c0 != c1) {
			//花色不同比花色
			return c0 > c1;
		}
		//花色相同比牌点
		uint8_t p0 = CGameLogic::GetCardPoint(card1);
		uint8_t p1 = CGameLogic::GetCardPoint(card2);
		return p0 > p1;
	}

	static bool byCardColorPointGL(uint8_t card1, uint8_t card2) {
		uint8_t c0 = CGameLogic::GetCardColor(card1);
		uint8_t c1 = CGameLogic::GetCardColor(card2);
		if (c0 != c1) {
			//花色不同比花色
			return c0 > c1;
		}
		//花色相同比牌点
		uint8_t p0 = CGameLogic::GetCardPoint(card1);
		uint8_t p1 = CGameLogic::GetCardPoint(card2);
		return p0 < p1;
	}

	static bool byCardColorPointLG(uint8_t card1, uint8_t card2) {
		uint8_t c0 = CGameLogic::GetCardColor(card1);
		uint8_t c1 = CGameLogic::GetCardColor(card2);
		if (c0 != c1) {
			//花色不同比花色
			return c0 < c1;
		}
		//花色相同比牌点
		uint8_t p0 = CGameLogic::GetCardPoint(card1);
		uint8_t p1 = CGameLogic::GetCardPoint(card2);
		return p0 > p1;
	}

	static bool byCardColorPointLL(uint8_t card1, uint8_t card2) {
		uint8_t c0 = CGameLogic::GetCardColor(card1);
		uint8_t c1 = CGameLogic::GetCardColor(card2);
		if (c0 != c1) {
			//花色不同比花色
			return c0 < c1;
		}
		//花色相同比牌点
		uint8_t p0 = CGameLogic::GetCardPoint(card1);
		uint8_t p1 = CGameLogic::GetCardPoint(card2);
		return p0 < p1;
	}

	//手牌排序(默认按牌点降序排列)，先比花色，再比牌值/点数
	//clrAscend bool false->花色降序(即黑桃到方块) true->花色升序(即从方块到黑桃)
	//byValue bool false->按牌点 true->按牌值
	//ascend bool false->降序排列(即从大到小排列) true->升序排列(即从小到大排列)
	void CGameLogic::SortCardsColor(uint8_t *cards, int n, bool clrAscend, bool byValue, bool ascend)
	{
		if (byValue) {
			if (ascend) {
				if (clrAscend) {
					//LL
					std::sort(cards, cards + n, byCardColorValueLL);
				}
				else {
					//GL
					std::sort(cards, cards + n, byCardColorValueGL);
				}
			}
			else {
				if (clrAscend) {
					//LG
					std::sort(cards, cards + n, byCardColorValueLG);
				}
				else {
					//GG
					std::sort(cards, cards + n, byCardColorValueGG);
				}
			}
		}
		else {
			if (ascend) {
				if (clrAscend) {
					//LL
					std::sort(cards, cards + n, byCardColorPointLL);
				}
				else {
					//GL
					std::sort(cards, cards + n, byCardColorPointGL);
				}
			}
			else {
				if (clrAscend) {
					//LG
					std::sort(cards, cards + n, byCardColorPointLG);
				}
				else {
					//GG
					std::sort(cards, cards + n, byCardColorPointGG);
				}
			}
		}
	}

	//牌值字符串 
	std::string CGameLogic::StringCardValue(uint8_t value)
	{
		switch (value)
		{
		case A: return "A";
		case J: return "J";
		case Q: return "Q";
		case K: return "K";
		}
		char ch[3] = { 0 };
		sprintf(ch, "%d", value);
		return ch;
	}
	
	//花色字符串 
	std::string CGameLogic::StringCardColor(uint8_t color)
	{
		switch (color)
		{
		case Spade:   return "♠";
		case Heart:   return "♥";
		case Club:	  return "♣";
		case Diamond: return "♦";
		}
		return "";
	}
	
	//单牌字符串
	std::string CGameLogic::StringCard(uint8_t card) {
		std::string s(StringCardColor(GetCardColor(card)));
		s += StringCardValue(GetCardValue(card));
		return s;
	}

	//比较散牌大小
	int CGameLogic::CompareSanPai(uint8_t *cards1, uint8_t *cards2)
	{
		//牌型相同按顺序比点
		for (int i = 0; i < 3; ++i) {
			uint8_t p0 = CGameLogic::GetCardPoint(cards1[i]);
			uint8_t p1 = CGameLogic::GetCardPoint(cards2[i]);
			if (p0 != p1) {
				return p0 - p1;
			}
		}
		//点数相同按顺序比花色
		for (int i = 0; i < 3; ++i) {
			uint8_t c0 = GetCardColor(cards1[i]);
			uint8_t c1 = GetCardColor(cards2[i]);
			if (c0 != c1) {
				return c0 - c1;
			}
		}
		return 0;
	}

	//比较对子大小
	int CGameLogic::CompareDuiZi(uint8_t *cards1, uint8_t *cards2)
	{
		//先比对牌点数
		uint8_t p0 = CGameLogic::GetCardPoint(cards1[1]);
		uint8_t p1 = CGameLogic::GetCardPoint(cards2[1]);
		if (p0 != p1) {
			return p0 - p1;
		}
		//对牌点数相同，比单牌点数
		uint8_t sp0 = CGameLogic::GetCardPoint(cards1[0]);
		if (sp0 == p0) {
			sp0 = CGameLogic::GetCardPoint(cards1[2]);
		}
		uint8_t sp1 = CGameLogic::GetCardPoint(cards2[0]);
		if (sp1 == p1) {
			sp1 = CGameLogic::GetCardPoint(cards2[2]);
		}
		if (sp0 != sp1) {
			return sp0 - sp1;
		}
		//单牌点数相同，比对牌里面最大的花色
		uint8_t c0, c1;
		if (sp0 == p0) {
			c0 = GetCardColor(cards1[0]);
		}
		else {
			c0 = GetCardColor(cards1[1]);
		}
		if (sp1 == p1) {
			c1 = GetCardColor(cards2[0]);
		}
		else {
			c1 = GetCardColor(cards2[1]);
		}
		return c0 - c1;
	}

	//比较顺子大小
	int CGameLogic::CompareShunZi(uint8_t *cards1, uint8_t *cards2)
	{
		//牌型相同按顺序比点
		for (int i = 0; i < 3; ++i) {
			uint8_t p0 = CGameLogic::GetCardPoint(cards1[i]);
			uint8_t p1 = CGameLogic::GetCardPoint(cards2[i]);
			if (p0 != p1) {
				return p0 - p1;
			}
		}
		//点数相同按顺序比花色
		for (int i = 0; i < 3; ++i) {
			uint8_t c0 = GetCardColor(cards1[i]);
			uint8_t c1 = GetCardColor(cards2[i]);
			if (c0 != c1) {
				return c0 - c1;
			}
		}
		return 0;
	}

	//比较金花大小
	int CGameLogic::CompareJinHua(uint8_t *cards1, uint8_t *cards2)
	{
		//牌型相同按顺序比点
		for (int i = 0; i < 3; ++i) {
			uint8_t p0 = CGameLogic::GetCardPoint(cards1[i]);
			uint8_t p1 = CGameLogic::GetCardPoint(cards2[i]);
			if (p0 != p1) {
				return p0 - p1;
			}
		}
		//点数相同按顺序比花色
		uint8_t c0 = GetCardColor(cards1[0]);
		uint8_t c1 = GetCardColor(cards2[0]);
		return c0 - c1;
	}

	//比较顺金大小
	int CGameLogic::CompareShunJin(uint8_t *cards1, uint8_t *cards2)
	{
		//牌型相同按顺序比点
		for (int i = 0; i < 3; ++i) {
			uint8_t p0 = CGameLogic::GetCardPoint(cards1[i]);
			uint8_t p1 = CGameLogic::GetCardPoint(cards2[i]);
			if (p0 != p1) {
				return p0 - p1;
			}
		}
		//点数相同按顺序比花色
		uint8_t c0 = GetCardColor(cards1[0]);
		uint8_t c1 = GetCardColor(cards2[0]);
		return c0 - c1;
	}

	//比较豹子大小
	int CGameLogic::CompareBaoZi(uint8_t *cards1, uint8_t *cards2)
	{
		//炸弹比较点数
		uint8_t p0 = CGameLogic::GetCardPoint(cards1[0]);
		uint8_t p1 = CGameLogic::GetCardPoint(cards2[0]);
		return p0 - p1;
	}
	
	//玩家手牌类型
	HandTy CGameLogic::GetHandCardsType(uint8_t *cards) {
		//手牌按牌点从大到小排序(AK...32)
		SortCards(cards, MaxCount, false, false, false);
		uint8_t p0 = GetCardPoint(cards[0]);
		uint8_t p1 = GetCardPoint(cards[1]);
		uint8_t p2 = GetCardPoint(cards[2]);
		if (p0 == p1) {
			if (p1 == p2) {
				//豹子(炸弹)：三张值相同的牌
				return BaoZi;
			}
			//对子：二张值相同的牌
			return DuiZi;
		}
		if (p1 == p2) {
			//对子：二张值相同的牌
			return DuiZi;
		}
		//三张连续牌
		if ((p1 == p2 + 1) && (p0 == p1 + 1 || p0 == p2 + 12)) {
			uint8_t c0 = GetCardColor(cards[0]);
			uint8_t c1 = GetCardColor(cards[1]);
			uint8_t c2 = GetCardColor(cards[2]);
			if (p0 == p2 + 12) {
				//手牌按牌值从大到小排序 A32->32A
				SortCards(cards, MaxCount, true, false, false);
			}
			if (c0 == c1 && c1 == c2) {
				//顺金(同花顺)：花色相同的顺子
				return ShunJin;
			}
			//顺子：花色不同的顺子
			return ShunZi;
		}
		uint8_t c0 = GetCardColor(cards[0]);
		uint8_t c1 = GetCardColor(cards[1]);
		uint8_t c2 = GetCardColor(cards[2]);
		if (c0 == c1 && c1 == c2) {
			//金花(同花)：花色相同，非顺子
			return JinHua;
		}
		if (p0 == 0x05 && p1 == 0x03 && p2 == 0x02) {
			//特殊：散牌中的235
			return TeShu235;
		}
		//散牌(高牌/单张)：三张牌不组成任何类型的牌
		return SanPai;
	}

	//牌型字符串
	std::string CGameLogic::StringHandType(HandTy ty) {
		switch (ty)
		{
		case SanPai:   return "SanPai";
		case DuiZi:    return "DuiZi";
		case ShunZi:   return "ShunZi";
		case JinHua:   return "JinHua";
		case ShunJin:  return "ShunJin";
		case BaoZi:    return "BaoZi";
		case TeShu235: return "TeShu235";
		}
		return "";
	}

	//比较手牌大小 >0-cards1大 <0-cards2大
	int CGameLogic::CompareHandCards(uint8_t *cards1, uint8_t *cards2)
	{
		HandTy t0 = GetHandCardsType(cards1);
		HandTy t1 = GetHandCardsType(cards2);
		if (t0 == t1) {
			//牌型相同情况
			if (t0 == TeShu235) {
				t0 = t1 = SanPai;
			}
		label:
			switch (t0)
			{
			case SanPai:
			{
				return CompareSanPai(cards1, cards2);
			}
			case DuiZi:
			{
				return CompareDuiZi(cards1, cards2);
			}
			case ShunZi:
			{
				return CompareShunZi(cards1, cards2);
			}
			case JinHua:
			{
				return CompareJinHua(cards1, cards2);
			}
			case ShunJin:
			{
				return CompareShunJin(cards1, cards2);
			}
			case BaoZi:
			{
				return CompareBaoZi(cards1, cards2);
			}
			}
		}
		else /*if (t0 != t1)*/ {
			//牌型不同情况
			if (t0 == TeShu235) {
				if (t1 == BaoZi) {
					return t0 - t1;
				}
				t0 = SanPai;
			} else if (t1 == TeShu235) {
				if (t0 == BaoZi) {
					return t1 - t0;
				}
				t1 = SanPai;
			}
			if (t0 == t1) {
				goto label;
			}
			//牌型不同比较
			return t0 - t1;
		}
	}
	
	//比较手牌大小 >0-cards1大 <0-cards2大
	bool CGameLogic::GreaterHandCards(boost::shared_ptr<uint8_t>& cards1, boost::shared_ptr<uint8_t>& cards2)
	{
		return CompareHandCards(boost::get_pointer(cards1), boost::get_pointer(cards2)) > 0;
	}
};