//
// Created by andy_ro@qq.com
// 			5/26/2019
//
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
#include <map>

#include "cfg.h"
#include "funcC.h"
#include "s13s.h"

namespace S13S {

	//一副扑克(52张)
	uint8_t s_CardListData[MaxCardTotal] =
	{
		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D, // 方块 A - K
		0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D, // 梅花 A - K
		0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D, // 红桃 A - K
		0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D, // 黑桃 A - K
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
		//手牌按花色升序(方块到黑桃)，同花色按牌值从小到大排序
		SortCardsColor(cardsData_, MaxCardTotal, true, true, true);
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
		static uint32_t seed = (uint32_t)time(NULL);
		//int c = rand() % 20 + 5;
		int c = rand_r(&seed) % 20 + 5;
		for (int k = 0; k < c; ++k) {
			for (int i = 0; i < MaxCardTotal; ++i) {
				//int j = rand() % MaxCardTotal;
				int j = rand_r(&seed) % MaxCardTotal;
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
			assert(i < MaxCardTotal);
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
		if (0 == value) {
			return "?";
		}
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
		return "?";
	}

	//单牌字符串
	std::string CGameLogic::StringCard(uint8_t card) {
		std::string s(StringCardColor(GetCardColor(card)));
		s += StringCardValue(GetCardValue(card));
		return s;
	}
	
	//牌型字符串
	std::string CGameLogic::StringCardType(HandTy ty) {
		switch (ty)
		{
		case Tysp:		return "Tysp";
		case Ty20:		return "Ty20";
		case Ty22:		return "Ty22";
		case Ty30:		return "Ty30";
		case Ty123:		return "Ty123";
		case Tysc:		return "Tysc";
		case Ty32:		return "Ty32";
		case Ty40:		return "Ty40";
		case Ty123sc:	return "Ty123sc";
			////// 特殊牌型
		case TyThreesc: return "TyThreesc";
		case TyThree123: return "TyThree123";
		case TySix20:	return "TySix20";
		case TyFive2030:	return "TyFive2030";
		case TyFour30:	return "TyFour30";
		case TyTwo3220:	return "TyTwo3220";
		case TyAllOneColor:	return "TyAllOneColor";
		case TyAllSmall:	return "TyAllSmall";
		case TyAllBig:	return "TyAllBig";
		case TyThree40:	return "TyThree40";
		case TyThree123sc:	return "TyThree123sc";
		case Ty12Royal:	return "Ty12Royal";
		case TyOneDragon:	return "TyOneDragon";
		case TyZZQDragon:	return "TyZZQDragon";
		}
		assert(false);
		return "";
	}

	//打印n张牌
	void CGameLogic::PrintCardList(uint8_t const* cards, int n, bool hide) {
		for (int i = 0; i < n; ++i) {
			if (cards[i] == 0 && hide) {
				continue;
			}
			printf("%s ", StringCard(cards[i]).c_str());
		}
		printf("\n");
	}

	//获取牌有效列数
	//cards uint8_t const* 相同牌值n张牌(n<=4)
	//n int 黑/红/梅/方4张牌
	uint8_t CGameLogic::get_card_c(uint8_t const* cards, int n) {
		assert(n <= 4);
		for (int i = 0; i < n; ++i) {
			if (cards[i] == 0) {
				return i;
			}
		}
		return n;
	}

	//返回指定花色牌列号
	//cards uint8_t const* 相同牌值n张牌(n<=4)
	//n int 黑/红/梅/方4张牌
	//clr CardColor 指定花色
	uint8_t CGameLogic::get_card_colorcol(uint8_t const* cards, int n, CardColor clr) {
		assert(n <= 4);
		//int c = get_card_c(cards, n);
		for (int i = 0; i < n/*c*/; ++i) {
			if (clr == GetCardColor(cards[i])) {
				return i;
			}
		}
		return 0xFF;
	}

	//拆分字符串"♦A ♦3 ♥3 ♥4 ♦5 ♣5 ♥5 ♥6 ♣7 ♥7 ♣9 ♣10 ♣J"
	void CGameLogic::CardsBy(string const& strcards, std::vector<string>& vec) {
		string str(strcards);
		while (true) {
			std::string::size_type s = str.find_first_of(' ');
			if (-1 == s) {
				break;
			}
			vec.push_back(str.substr(0, s));
			str = str.substr(s + 1);
		}
		if (!str.empty()) {
			vec.push_back(str.substr(0, -1));
		}
	}

	//字串构造牌"♦A"->0x01
	uint8_t CGameLogic::MakeCardBy(string const& name) {
		uint8_t color = 0, value = 0;
		if (0 == strncmp(name.c_str(), "♠", 3)) {
			color = Spade;
			string str(name.substr(3, -1));
			switch (str.front())
			{
			case 'J': value = J; break;
			case 'Q': value = Q; break;
			case 'K': value = K; break;
			case 'A': value = A; break;
			case 'T': value = T; break;
			default: {
				value = atoi(str.c_str());
				break;
			}
			}
		}
		else if (0 == strncmp(name.c_str(), "♥", 3)) {
			color = Heart;
			string str(name.substr(3, -1));
			switch (str.front())
			{
			case 'J': value = J; break;
			case 'Q': value = Q; break;
			case 'K': value = K; break;
			case 'A': value = A; break;
			case 'T': value = T; break;
			default: {
				value = atoi(str.c_str());
				break;
			}
			}
		}
		else if (0 == strncmp(name.c_str(), "♣", 3)) {
			color = Club;
			string str(name.substr(3, -1));
			switch (str.front())
			{
			case 'J': value = J; break;
			case 'Q': value = Q; break;
			case 'K': value = K; break;
			case 'A': value = A; break;
			case 'T': value = T; break;
			default: {
				value = atoi(str.c_str());
				break;
			}
			}
		}
		else if (0 == strncmp(name.c_str(), "♦", 3)) {
			color = Diamond;
			string str(name.substr(3, -1));
			switch (str.front())
			{
			case 'J': value = J; break;
			case 'Q': value = Q; break;
			case 'K': value = K; break;
			case 'A': value = A; break;
			case 'T': value = T; break;
			default: {
				value = atoi(str.c_str());
				break;
			}
			}
		}
		assert(value != 0);
		return value ? MakeCardWith(color, value) : 0;
	}

	//生成n张牌<-"♦A ♦3 ♥3 ♥4 ♦5 ♣5 ♥5 ♥6 ♣7 ♥7 ♣9 ♣10 ♣J"
	void CGameLogic::MakeCardList(std::vector<string> const& vec, uint8_t *cards, int size) {
		int c = 0;
		for (std::vector<string>::const_iterator it = vec.begin();
			it != vec.end(); ++it) {
			cards[c++] = MakeCardBy(it->c_str());
		}
	}
	
	//生成n张牌<-"♦A ♦3 ♥3 ♥4 ♦5 ♣5 ♥5 ♥6 ♣7 ♥7 ♣9 ♣10 ♣J"
	int CGameLogic::MakeCardList(string const& strcards, uint8_t *cards, int size) {
		std::vector<string> vec;
		CardsBy(strcards, vec);
		MakeCardList(vec, cards, size);
		return (int)vec.size();
	}
	
	//返回去重后的余牌/散牌
	//src uint8_t const* 牌源
	//pdst uint8_t(**const)[4] 衔接dst4/dst3/dst2
	//dst4 uint8_t(*)[4] 存放所有四张牌型，c4 四张牌型数
	//dst3 uint8_t(*)[4] 存放所有三张牌型，c3 三张牌型数
	//dst2 uint8_t(*)[4] 存放所有对子牌型，c2 对子牌型数
	//cpy uint8_t* 去重后的余牌/散牌(除去四张/三张/对子)///////
	int CGameLogic::RemoveRepeatCards(uint8_t const* src, int len,
		uint8_t(**const pdst)[4],
		uint8_t(*dst4)[4], int& c4, uint8_t(*dst3)[4], int& c3,
		uint8_t(*dst2)[4], int& c2, uint8_t *cpy, int& cpylen) {
		uint8_t const* psrc = src;
		int lensrc = len;
		uint8_t cpysrc[MaxSZ] = { 0 };
		//枚举所有重复四张牌型
		int const size4 = 3;
		//uint8_t dst4[size4][4] = { 0 };
		c4 = EnumRepeatCards(psrc, lensrc, 4, dst4, size4, cpy, cpylen);
		if (c4 > 0) {
			memcpy(cpysrc, cpy, cpylen);//cpysrc
			lensrc = cpylen;//lensrc
			psrc = cpysrc;//psrc
		}
		//枚举所有重复三张牌型
		int const size3 = 4;
		//uint8_t dst3[size3][4] = { 0 };
		c3 = EnumRepeatCards(psrc, lensrc, 3, dst3, size3, cpy, cpylen);
		if (c3 > 0) {
			memcpy(cpysrc, cpy, cpylen);//cpysrc
			lensrc = cpylen;//lensrc
			psrc = cpysrc;//psrc
		}
		//枚举所有重复二张牌型
		int const size2 = 6;
		//uint8_t dst2[size2][4] = { 0 };
		c2 = EnumRepeatCards(psrc, lensrc, 2, dst2, size2, cpy, cpylen);
		if (c2 == 0) {
			memcpy(cpy, psrc, lensrc);//cpy
			cpylen = lensrc;//cpylen
		}
		//用psrc衔接dst4/dst3/dst2 //////
		//uint8_t(*pdst[6])[4] = { 0 };
		//typedef uint8_t(*Ptr)[4];
		//Ptr pdst[6] = { 0 };
		int c = 0;
		//所有重复四张牌型
		for (int i = 0; i < c4; ++i) {
			pdst[c++] = &dst4[i];
		}
		//所有重复三张牌型
		for (int i = 0; i < c3; ++i) {
			pdst[c++] = &dst3[i];
		}
		//所有重复二张牌型
		for (int i = 0; i < c2; ++i) {
			pdst[c++] = &dst2[i];
		}
		return c;
	}

#if 0
	//求组合C(n,1)*C(n,1)...*C(n,1)
	//f(k)=C(n,1)
	//Muti(k)=f(1)*f(2)...*f(k)
	//n int 访问广度 由c4,c3,c2计算得到(4/3/2/1)
	//k int 访问深度
	//clr bool true->区分花色的所有组合
	//clr bool false->不区分花色的所有组合
	//深度优先遍历，由浅到深，广度遍历，由里向外
	int CGameLogic::DepthVisit(int c4, int c3, int c2,
		int k,
		uint8_t(*const*const psrc)[4],
		std::vector<std::vector<short>>& ctx,
		std::vector<std::vector<uint8_t>>& dst,
		std::vector<int> const& vec, bool clr) {
		int c = 0;
		static int const KDEPTH = MaxSZ;
		int e[KDEPTH + 1] = { 0 };
		e[0] = k;
		//dst.clear();
		//ctx.clear();
		return DepthC(c4, c3, c2, k, e, c, psrc, ctx, dst, vec, clr);
	}

	//递归求组合C(n,1)*C(n,1)...*C(n,1)
	//f(k)=C(n,1)
	//Muti(k)=f(1)*f(2)...*f(k)
	//n int 访问广度 由c4,c3,c2计算得到(4/3/2/1)
	//k int 访问深度
	//clr bool true->区分花色的所有组合
	//clr bool false->不区分花色的所有组合
	//深度优先遍历，由浅到深，广度遍历，由里向外
	int CGameLogic::DepthC(int c4, int c3, int c2,
		int k, int *e, int& c,
		uint8_t(*const*const psrc)[4],
		std::vector<std::vector<short>>& ctx,
		std::vector<std::vector<uint8_t>>& dst,
		std::vector<int> const& vec, bool clr) {
		//c4,c3,c2计算n(4/3/2/1)
		int n = clr ? 4 : 1;
		if (vec[k - 1] < c4) {
			n = clr ? 4 : 1;
		}
		else if (vec[k - 1] < (c4 + c3)) {
			n = clr ? 4/*3*/ : 1;
		}
		else {
			n = clr ? 4/*2*/ : 1;
		}
		//不区分花色取一张就够了
		for (int i = n; i > 0; --i) {
			if ((*psrc[vec[k - 1]])[i - 1] == 0) {
				continue;
			}
			assert(k > 0);
			//psrc第vec[j - 1]组第e[j] - 1张(0/1/2/3)
			//psrc第vec[j - 1]组第e[j] - 1张(0/1/2)
			//psrc第vec[j - 1]组第e[j] - 1张(0/1)
			e[k] = i;
			if (k > 1) {
				DepthC(c4, c3, c2, k - 1, e, c, psrc, ctx, dst, vec, clr);
			}
			else {
				++c;
				std::vector<uint8_t> v;
				std::vector<short> w;
				//深度1...e[0]
				for (int j = e[0]; j > 0; --j) {
					//printf("%d", e[j]);
					//printf("%s", StringCard((*psrc[vec[j - 1]])[e[j] - 1]).c_str());
					v.push_back((*psrc[vec[j - 1]])[e[j] - 1]);
					{
						//psrc第r组/牌数c ///
						uint8_t r = vec[j - 1];
						uint8_t c = 0;
						while (c < 4 && (*psrc[r])[c] > 0) {
							++c;
						}
						//当前组合位于psrc的ctx信息
						short ctx = (short)(((0xFF & r) << 8) | (0xFF & c));
						w.push_back(ctx);
					}
				}
				//printf(",");
				dst.push_back(v);
				//v.clear();
				ctx.push_back(w);
				//w.clear();
			}
		}
		return c;
	}
#endif

	static bool byValueG(const uint8_t(*const a)[4], const uint8_t(*const b)[4]) {
		uint8_t v0 = CGameLogic::GetCardValue(a[0][0]);
		uint8_t v1 = CGameLogic::GetCardValue(b[0][0]);
		return v0 > v1;
	}
	static bool byValueL(const uint8_t(*const a)[4], const uint8_t(*const b)[4]) {
		uint8_t v0 = CGameLogic::GetCardValue(a[0][0]);
		uint8_t v1 = CGameLogic::GetCardValue(b[0][0]);
		return v0 < v1;
	}
	static bool byPointG(const uint8_t(*const a)[4], const uint8_t(*const b)[4]) {
		uint8_t p0 = CGameLogic::GetCardPoint(a[0][0]);
		uint8_t p1 = CGameLogic::GetCardPoint(b[0][0]);
		return p0 > p1;
	}
	static bool byPointL(const uint8_t(*const a)[4], const uint8_t(*const b)[4]) {
		uint8_t p0 = CGameLogic::GetCardPoint(a[0][0]);
		uint8_t p1 = CGameLogic::GetCardPoint(b[0][0]);
		return p0 < p1;
	}
	static void SortCards_src(uint8_t(**const psrc)[4], int n, bool byValue, bool ascend) {
		if (byValue) {
			if (ascend) {
				std::sort(psrc, psrc + n, byValueL);
			}
			else {
				std::sort(psrc, psrc + n, byValueG);
			}
		}
		else {
			if (ascend) {
				std::sort(psrc, psrc + n, byPointL);
			}
			else {
				std::sort(psrc, psrc + n, byPointG);
			}
		}
	}

#if 0
	//枚举单张组合牌(分别从四张/三张/对子中任抽一张组合牌)
	//src4 uint8_t(*const)[4] 所有四张牌型牌源，c4 四张牌型数
	//src3 uint8_t(*const)[4] 所有三张牌型牌源，c3 三张牌型数
	//src2 uint8_t(*const)[4] 所有对子牌型牌源，c2 对子牌型数
	//dst std::vector<std::vector<uint8_t>>& 存放单张组合牌//////
	//clr bool true->区分花色的所有组合 false->不区分花色的所有组合
	int CGameLogic::EnumCombineCards(
		uint8_t(**const psrc)[4],
		uint8_t(*const src4)[4], int const c4,
		uint8_t(*const src3)[4], int const c3,
		uint8_t(*const src2)[4], int const c2,
		std::vector<std::vector<short>>& ctx,
		std::vector<std::vector<uint8_t>>& dst, bool clr) {
		//uint8_t(*src[6])[4] = { 0 };
		//typedef uint8_t(*Ptr)[4];
		//Ptr psrc[6] = { 0 };
		int c = 0;
		dst.clear();
		ctx.clear();
		//所有重复四张牌型
		for (int i = 0; i < c4; ++i) {
			psrc[c++] = &src4[i];
		}
		//所有重复三张牌型
		for (int i = 0; i < c3; ++i) {
			psrc[c++] = &src3[i];
		}
		//所有重复二张牌型
		for (int i = 0; i < c2; ++i) {
			psrc[c++] = &src2[i];
		}
		int n = c;
		CFuncC fnC;
		std::vector<std::vector<int>> vec;
		//psrc组与组之间按牌值升序排列(从小到大)
		SortCards_src(psrc, n, true, true);
		//////从n组里面选取任意1/2/3...k...n组的组合数 //////
		//C(n,1),C(n,2),C(n,3)...C(n,k)...C(n,n)
		for (int k = 1; k <= n; ++k) {
			int c = fnC.FuncC(n, k, vec);
			//printf("\n--- *** ------------------- C(%d,%d)=%d\n", n, k, c);
			for (std::vector<std::vector<int>>::iterator it = vec.begin();
				it != vec.end(); ++it) {
				assert(k == it->size());
				//printf("\n--- *** start\n");
#if 0
				for (std::vector<int>::iterator ir = it->begin();
					ir != it->end(); ++ir) {
					printf("%d", *ir);
				}
 				printf("\n---\n");
#endif
// 				for (std::vector<int>::iterator ir = it->begin();
// 					ir != it->end(); ++ir) {
// 					if (*ir < c4) {
// 						PrintCardList(*psrc[*ir], 4);
// 					}
// 					else if (*ir < (c4 + c3)) {
// 						PrintCardList(*psrc[*ir], 3);
// 					}
// 					else {
// 						PrintCardList(*psrc[*ir], 2);
// 					}
//				}
				//printf("---\n");
				//f(k)=C(n,1)
				//Muti(k)=f(1)*f(2)...*f(k)
				//////从选取的k组中分别任取一张牌的组合数 //////
				int c = DepthVisit(c4, c3, c2, k, psrc, ctx, dst, *it, clr);
				//printf("\n--- *** end c=%d\n", c);
			}
		}
		return dst.size();
	}

	static bool ExistVec(std::vector<std::vector<uint8_t>>& dst, std::vector<uint8_t>& v) {
		for (std::vector<std::vector<uint8_t>>::iterator it = dst.begin();
			it != dst.end(); ++it) {
			if (it->size() == v.size()) {
				int i = 0;
				for (; i != it->size(); ++i) {
					if (v[i] != (*it)[i]) {
						break;
					}
				}
				if (i == it->size())
					return true;
			}
		}
		return false;
	}

	//从补位合并后的单张组合牌中枚举连续牌型///////
	//src uint8_t const* 单张组合牌源，去重后的余牌/散牌与单张组合牌补位合并
	//n int 抽取n张(3/5/13)
	//dst1 std::vector<std::vector<uint8_t>>& 存放所有连续牌型(非同花)
	//dst2 std::vector<std::vector<uint8_t>>& 存放所有连续牌型(同花)
	int CGameLogic::EnumConsecCardsMarkLoc(uint8_t const* src, int len, int n,
		std::vector<short>& ctx,
		std::vector<std::vector<short>>& dstctx,
		std::vector<std::vector<uint8_t>>& dst) {
		assert(len > 0);
		int i = 0, j = 0/*, k = 0*/, s;
		uint8_t v_src_pre = 0;
	next:
		while (i < len) {
			s = i++;
			v_src_pre = GetCardValue(src[s]);
			if (v_src_pre > 0) {
				break;
			}
		}
		if (s + n <= len) {
			for (; i < len; ++i) {
				//src中当前的牌值
				uint8_t v_src_cur = GetCardValue(src[i]);
				//牌位有值则连续
				if (v_src_cur > 0) {
					//收集到n张牌后返回
					if (i - s + 1 == n) {
						//缓存符合要求的牌型
						std::vector<uint8_t> v(&src[s], &src[s] + n);
						if (!ExistVec(dst, v)) {
							dst.push_back(v);
							dstctx.push_back(ctx);
							//printf("--- *** dst s:%d i:%d\n", s, i);
							PrintCardList(&v.front(), n);
						}
						++s;
					}
				}
				//牌位无值不连续
				else {
					++i;
					goto next;
				}
			}
		}
		return dst.size();
	}
#endif
	
	//枚举所有指定重复牌型(四张/三张/二张)
	//src uint8_t const* 牌源
	//n int 抽取n张(4/3/2)
	//dst uint8_t(*)[4] 存放指定重复牌型
	//cpy uint8_t* 抽取后不符合要求的余牌
	int CGameLogic::EnumRepeatCards(uint8_t const* src, int len, int n,
		uint8_t(*dst)[4], int size, uint8_t *cpy, int& cpylen) {
		//手牌按牌值从小到大排序(A23...QK)
		//SortCards(cards, len, true, true, true);
		assert(n <= 4);
		//assert(len > 0);
		int i = 0, j = 0, k = 0, dstLen = 0;
		cpylen = 0;
	next:
		int s = i++;
		//while (i < len) {
		//	s = i++;
		//	v_src_pre = GetCardValue(src[s]);
		//	if (v_src_pre > 0) {
		//		break;
		//	}
		//}
		if (s + n <= len) {
			uint8_t v_src_pre = GetCardValue(src[s]);
			for (; i < len; ++i) {
				//src中当前的牌值
				uint8_t v_src_cur = GetCardValue(src[i]);
				//两张牌值相同的牌
				if (v_src_pre == v_src_cur) {
					//收集到n张牌后返回
					if (i - s + 1 == n) {
						//缓存符合要求的牌型
						memcpy(dst[dstLen++], &src[s], n);
						//printf("--- *** dst s:%d i:%d\n", s, i);
						//PrintCardList(dst[dstLen - 1], n);
						++i;
						goto next;
					}
				}
				//空位
				//else if (v_src_cur == 0) {
				//}
				//两张牌值不同的牌
				else {
					//缓存不合要求的副本
					memcpy(&cpy[k], &src[s], i - s);
					k += (i - s);
					//printf("--- *** cpy s:%d i:%d\n", s, i);
					//PrintCardList(cpy, k);
					//赋值新的
					s = i;
					v_src_pre = GetCardValue(src[s]);
				}
			}
		}
		if (dstLen) {
			//缓存不合要求的副本
			if (s < len) {
				memcpy(&cpy[k], &src[s], len - s);
				cpylen = k;
				cpylen += len - s;
			}
			else {
				cpylen = k;
			}
			//if (cpylen > 0) {
			//	printf("--- *** cpy s:%d i:%d\n", s, i);
			//	PrintCardList(cpy, cpylen);
			//}
		}
		return dstLen;
	}

	//填充卡牌条码标记位
	//src uint8_t* 用A2345678910JQK来占位 size = MaxSZ
	//dst uint8_t const* 由单张组成的余牌或枚举组合牌
	void CGameLogic::FillCardsMarkLoc(uint8_t *src, int size, uint8_t const* dst, int dstlen, bool reset) {
		assert(size == MaxSZ);
		if (reset) {
			memset(src, 0, MaxSZ * sizeof(uint8_t));
		}
		for (int i = 0; i < dstlen; ++i) {
			uint8_t v = GetCardValue(dst[i]);
			assert(v >= A && v <= K);
			if (src[v - 1] != 0) {
				//assert(false);
			}
			src[v - 1] = dst[i];
		}
	}
	
	//填充卡牌条码标记位
	//src uint8_t* 用2345678910JQKA来占位 size = MaxSZ
	//dst uint8_t const* 由单张组成的余牌或枚举组合牌
	void CGameLogic::FillCardsMarkLocByPoint(uint8_t *src, int size, uint8_t const* dst, int dstlen, bool reset) {
		assert(size == MaxSZ);
		if (reset) {
			memset(src, 0, MaxSZ * sizeof(uint8_t));
		}
		for (int i = 0; i < dstlen; ++i) {
			uint8_t p = GetCardPoint(dst[i]);
			assert(p >= 2 && p <= GetCardPoint(A));
			if (src[p - 1] != 0) {
				//assert(false);
			}
			src[p - 1] = dst[i];
		}
	}
	
	//从补位合并后的单张组合牌中枚举所有不区分花色n张连续牌型///////
	//src uint8_t const* 单张组合牌源，去重后的余牌/散牌与从psrc每组中各抽一张牌补位合并
	//n int 抽取n张(3/5/13)
	//start int const 检索src/mark的起始下标
	//psrcctx short const* 标记psrc的ctx信息
	//dst std::vector<std::vector<uint8_t>>& 存放所有连续牌型(不区分花色)
	//clr std::vector<bool>& 对应dst是否同花
	int CGameLogic::EnumConsecCardsMarkLoc(uint8_t const* src, int len, int n,
		int const start, short const* psrcctx,
		std::vector<std::vector<short>>& ctx,
		std::vector<std::vector<uint8_t>>& dst,
		std::vector<bool>& clr) {
		assert(len > 0);
		int i = start, j = 0, k = 0, s;
		uint8_t v_src_pre = 0, c_src_pre = 0;
		bool sameclr = true;
	next:
		while (i < len) {
			s = i++;
			v_src_pre = GetCardValue(src[s]);
			c_src_pre = GetCardColor(src[s]);
			if (v_src_pre > 0) {
				break;
			}
		}
		sameclr = true;
		if (s + n <= len) {
			for (; i < len; ++i) {
				//src中当前牌值
				uint8_t v_src_cur = GetCardValue(src[i]);
				//src中当前花色
				uint8_t c_src_cur = GetCardColor(src[i]);
				if (c_src_cur != c_src_pre) {
					sameclr = false;
				}
				//牌位有值则连续
				if (v_src_cur > 0) {
					//收集到n张牌后返回
					if (i - s + 1 == n) {
						//缓存符合要求的牌型
						std::vector<uint8_t> v(&src[s], &src[s] + n);
						dst.push_back(v);
						clr.push_back(sameclr);
						std::vector<short> x;
						//检查s到i中的牌是否存在psrc中
						for (k = s; k <= i; ++k) {
							if (psrcctx[k] > 0) {
								//当前连续牌映射psrc的ctx信息
								x.push_back(psrcctx[k]);
								//uint8_t r = (0xFF & (psrcctx[k] >> 8));
								//uint8_t	c = (0xFF & psrcctx[k]);
								//printf("r:%d c:%d ===>>> %s\n", r, c, StringCard(src[k]).c_str());
							}
						}
						ctx.push_back(x);
						//printf("--- *** dst s:%d i:%d\n", s, i);
						//PrintCardList(&v.front(), n);
						++s;
					}
				}
				//牌位无值不连续
				else {
					++i;
					goto next;
				}
			}
		}
		return dst.size();
	}
	
	//从补位合并后的单张组合牌中枚举所有n张指定花色牌型///////
	//src uint8_t const* 单张组合牌源，去重后的余牌/散牌与从psrc每组中各抽一张牌补位合并
	//n int 抽取n张(3/5/13)
	//clr CardColor 指定花色
	//psrcctx short const* 标记psrc的ctx信息
	//dst std::vector<std::vector<uint8_t>>& 存放所有同花牌型(非顺子)
	//consec bool false->不保留同花顺 true->保留同花顺 
	//dst2 std::vector<std::vector<uint8_t>>& 存放所有同花顺牌型
	int CGameLogic::EnumSameColorCardsMarkLoc(uint8_t const* src, int len, int n, CardColor clr,
		short const* psrcctx,
		std::vector<std::vector<short>>& ctx,
		std::vector<std::vector<uint8_t>>& dst,
		bool consec,
		std::vector<std::vector<uint8_t>>& dst2) {
		//assert(len > 0);
		int i = 0, j = 0, k = 0, s, c = 0;
		uint8_t cards[MaxSZ] = { 0 };
	next:
		while (k < len) {
			s = k++;
			uint8_t c_src_pre = GetCardColor(src[s]);
			if (c_src_pre > 0 && c_src_pre == clr) {
				break;
			}
		}
		c = 0;
		if (s + n <= len) {
			for (i = s; i < len; ++i) {
				//src中当前牌花色
				uint8_t c_src_cur = GetCardColor(src[i]);
				//当前牌是指定花色
				if (clr == c_src_cur) {
					//收集到n张牌后返回
					cards[c++] = src[i];
					if (c == n) {
						//缓存符合要求的牌型
						if (i - s + 1 == n) {
							if(consec) {
								//s到i为花色相同的n张连续牌型(同花顺)，不缓存
								std::vector<uint8_t> v(&src[s], &src[s] + n);
								dst2.push_back(v);
								//printf("--- *** dst s:%d i:%d n:%d\n", s, i, n);
								//PrintCardList(&v.front(), n);
							}
						}
						else {
							//s到i中间有无效牌或不同于当前花色的牌，但已收集到n张与当前花色相同的牌
							std::vector<uint8_t> v(&cards[0], &cards[0] + c);
							dst.push_back(v);
							//printf("--- *** dst s:%d i:%d n:%d\n", s, i, n);
							//PrintCardList(&v.front(), n);
						}
						c = 0;
						goto next;
					}
				}
				//牌位无值
				else if (c_src_cur == 0) {
				}
				//当前牌非指定花色
				else {
				}
			}
		}
		return dst.size();
	}

#if 0
	//枚举所有n张连续牌型(同花顺/顺子)，先去重再补位，最后遍历查找
	//去重后的余牌/散牌与单张组合牌补位合并//////
	//src std::vector<std::vector<uint8_t>> const& 所有单张组合牌源///////
	//cpy uint8_t const* 去重后的余牌/散牌(除去四张/三张/对子)牌源///////
	//n int 抽取n张(3/5/13)
	//dst1 std::vector<std::vector<uint8_t>>& 存放所有连续牌型(非同花)
	//dst2 std::vector<std::vector<uint8_t>>& 存放所有连续牌型(同花)
	void CGameLogic::EnumConsecCards(
		std::vector<std::vector<uint8_t>> const& src,
		std::vector<std::vector<short>>& ctx,
		uint8_t const* cpy, int const cpylen, int n,
		std::vector<std::vector<short>>& dstctx,
		std::vector<std::vector<uint8_t>>& dst) {
		int i = 0;
		dstctx.clear();
		//用A2345678910JQK来占位 size = MaxSZ
		uint8_t mark[MaxSZ] = { 0 };
		//各单张组合牌分别与去重后的余牌/散牌补位合并
		for (std::vector<std::vector<uint8_t>>::const_iterator it = src.begin();
			it != src.end(); ++it) {
			//用去重后的余牌/散牌进行补位
			FillCardsMarkLoc(mark, MaxSZ, cpy, cpylen, true);
			//printf("\n\n");
			//PrintCardList(&it->front(), it->size());
			//PrintCardList(mark, MaxSZ, false);
			//用当前单张组合牌进行补位合并
			FillCardsMarkLoc(mark, MaxSZ, &it->front(), it->size(), false);
			//PrintCardList(mark, MaxSZ, false);
			//从补位合并后的单张组合牌中枚举连续牌型
			EnumConsecCardsMarkLoc(mark, MaxSZ, n, ctx[i++], dstctx, dst);
			//printf("\n\n\n");
		}
		printf("n = %d\n\n", dst.size());
	}
#endif

	//枚举所有不区分花色n张连续牌型(同花顺/顺子)，先去重再补位，最后遍历查找
	//去重后的余牌/散牌与单张组合牌补位合并//////
	//psrc uint8_t(**const)[4] 衔接dst4/dst3/dst2
	//psrc uint8_t(**const)[4] 从所有四张/三张/对子中各取一张合成单张组合牌
	//cpy uint8_t const* 去重后的余牌/散牌(除去四张/三张/对子)牌源///////
	//n int 抽取n张(3/5/13)
	//ctx std::vector<std::vector<short>>& 标记psrc的ctx信息
	//dst std::vector<std::vector<uint8_t>>& 存放所有连续牌型
	//clr std::vector<bool>& 对应dst是否同花
	void CGameLogic::EnumConsecCards(
		uint8_t(**const psrc)[4], int const psrclen,
		uint8_t const* cpy, int const cpylen, int n,
		std::vector<std::vector<short>>& ctx,
		std::vector<std::vector<uint8_t>>& dst,
		std::vector<bool>& clr) {
		//用A2345678910JQK来占位 size = MaxSZ
		uint8_t mark[MaxSZ] = { 0 };
		//标记psrc的ctx信息mark[j]=>psrcctx[j]=>psrc[i],c
		short psrcctx[MaxSZ] = { 0 };
		//用去重后的余牌/散牌进行补位
		FillCardsMarkLoc(mark, MaxSZ, cpy, cpylen, true);
		//printf("\n\n");
		//PrintCardList(mark, MaxSZ, false);
		//psrc[ai],ac,av
		uint8_t ai = 0, ac = 0, av = 0;
		for (int i = 0; i < psrclen; ++i) {
			//psrc第i组/牌数c ///
			uint8_t c = get_card_c(psrc[i][0], 4);
			//mark[j]=>psrcctx[j]=>psrc[i],c
			uint8_t v = GetCardValue((*psrc[i])[0]);
			//当前组合位于psrc的ctx信息
			psrcctx[v - 1] = (short)(((0xFF & i) << 8) | (0xFF & c));
			//从psrc每组中各抽一张牌补位 //////
			FillCardsMarkLoc(mark, MaxSZ, psrc[i][0], 1, false);
			if (v == A) {
				ai = i, ac = c, av = v;
			}
		}
		//补位完成后判断能否构成10JDKA牌型A...K
		if (GetCardValue(mark[0]) == A && GetCardValue(mark[K - 1]) == K) {
			int i;
			for (i = K - 2; i > K - n; --i) {
				if (mark[i] == 0) {
					//有断层不连续
					break;
				}
			}
			//满足要求的连续n张
			if (i == K - n) {
				//psrc中有单牌A
				if (av > 0) {
					//当前组合位于psrc的ctx信息
					psrcctx[K] = (short)(((0xFF & ai) << 8) | (0xFF & ac));
				}
				//mark下标为K的位置用单牌A占位
				mark[K] = av > 0 ? (*psrc[ai])[0] : mark[0];
				//printf("r:%d c:%d ===>>> %s\n", ai, ac, StringCard(mark[K]).c_str());
			}
		}
		//PrintCardList(mark, MaxSZ, false);
		//printf("\n\n\n");
		//补位合并后枚举所有n张连续牌型
		EnumConsecCardsMarkLoc(mark, MaxSZ, n, 0, psrcctx, ctx, dst, clr);
	}

	//枚举所有n张相同花色不连续牌型(同花)，先去重再补位，最后遍历查找
	//去重后的余牌/散牌与单张组合牌补位合并//////
	//psrc uint8_t(**const)[4] 衔接dst4/dst3/dst2
	//psrc uint8_t(**const)[4] 从所有四张/三张/对子中各取一张合成单张组合牌
	//cpy uint8_t const* 去重后的余牌/散牌(除去四张/三张/对子)牌源///////
	//n int 抽取n张(3/5/13)
	//ctx std::vector<std::vector<short>>& 标记psrc的ctx信息
	//dst std::vector<std::vector<uint8_t>>& 存放所有同花牌型
	void CGameLogic::EnumSameColorCards(
		uint8_t(**const psrc)[4], int const psrclen,
		uint8_t const* cpy, int const cpylen, int n,
		std::vector<std::vector<short>>& ctx,
		std::vector<std::vector<uint8_t>>& dst) {
		//printf("--- *** 枚举所有同花\n");
		CardColor color[4] = { Spade,Heart,Club,Diamond, };
		for (int k = 0; k < 4; ++k) {
			//用A2345678910JQK来占位 size = MaxSZ
			uint8_t mark[MaxSZ] = { 0 };
			//标记psrc的ctx信息mark[j]=>psrcctx[j]=>psrc[i],c
			//short psrcctx[MaxSZ] = { 0 };
			//用去重后的余牌/散牌进行补位
			FillCardsMarkLoc(mark, MaxSZ, cpy, cpylen, false);
			//printf("\n\n");
			//PrintCardList(mark, MaxSZ, false);
			for (int i = 0; i < psrclen; ++i) {
				//psrc第i组/牌数c ///
				uint8_t c = get_card_c(psrc[i][0], 4);
				uint8_t j = get_card_colorcol(psrc[i][0], (int)c, color[k]);
				if (j != 0xFF) {
					//PrintCardList(psrc[i][0], c, false);
					//printf("color[%s] ==>> j == %d\n", StringCardColor(color[k]).c_str(), j);
					assert(GetCardValue((*psrc[i])[0]) == GetCardValue((*psrc[i])[j]));
					//mark[j]=>psrcctx[j]=>psrc[i],c
					//uint8_t v = GetCardValue((*psrc[i])[0]);
					//当前组合位于psrc的ctx信息
					//psrcctx[v - 1] = (short)(((0xFF & i) << 8) | (0xFF & c));
					//从psrc每组中调取指定花色牌补位 //////
					FillCardsMarkLoc(mark, MaxSZ, &(*psrc[i])[j], 1, false);
				}
			}
			//PrintCardList(mark, MaxSZ, false);
			//printf("\n\n\n");
			//std::vector<std::vector<uint8_t>> _;
			//补位合并后枚举所有n张当前花色牌型(同花非顺子)，不保留同花顺(在别的地方枚举出来了)//////
			//EnumSameColorCardsMarkLoc(mark, MaxSZ, n, color[k], NULL/*psrcctx*/, ctx, dst, false, _);
			int c = 0;
			short psrcctx[MaxSZ] = { 0 };
			for (int i = 0; i < MaxSZ; ++i) {				
				if (color[k] == GetCardColor(mark[i])) {
					assert(GetCardValue(mark[i]) > 0);
					psrcctx[c++] = i;
				}
			}
			//满足要求的n张同花
 			if (c >= n) {
 				CFuncC fnC;
 				std::vector<std::vector<int>> vec;
 				//////从c个当前花色牌里面选取任意n个的组合数C(c,n) //////
 				int m = fnC.FuncC(c, n, vec);
				//printf("--- *** C(%d,%d) = %d\n", c, n, m);
				for (std::vector<std::vector<int>>::iterator it = vec.begin();
					it != vec.end(); ++it) {
					if (psrcctx[*it->begin()] - psrcctx[*it->rbegin()] + 1 == n) {
						//同花顺
						std::vector<uint8_t> v;
						for (std::vector<int>::const_reverse_iterator ir = it->rbegin();
							ir != it->rend(); ++ir) {
							assert(psrcctx[*ir] < MaxSZ &&
								mark[psrcctx[*ir]] > 0);
							v.push_back(mark[psrcctx[*ir]]);
						}
						//_.push_back(v);
					}
					else {
						//同花
						std::vector<uint8_t> v;
						for (std::vector<int>::const_reverse_iterator ir = it->rbegin();
							ir != it->rend(); ++ir) {
							assert(psrcctx[*ir] < MaxSZ &&
								mark[psrcctx[*ir]] > 0);
							v.push_back(mark[psrcctx[*ir]]);
						}
						dst.push_back(v);
						//PrintCardList(&v.front(), v.size());
					}
				}
				//printf("\n---\n");
 			}
		}
		//for (std::vector<std::vector<uint8_t>>::const_iterator it = dst.begin();
		//	it != dst.end(); ++it) {
		//	PrintCardList(&it->front(), it->size());
		//}
	}

	//枚举所有同花顺/顺子(区分花色五张连续牌)
	//psrc uint8_t(**const)[4] 衔接dst4/dst3/dst2
	//psrc uint8_t(**const)[4] 从所有四张/三张/对子中各取一张合成单张组合牌
	//ctx std::vector<std::vector<short>>& 标记psrc的ctx信息
	//src std::vector<std::vector<uint8_t>> const& 所有单张组合牌源///////
	//clr std::vector<bool> const& 对应src是否同花
	//dst1 std::vector<std::vector<uint8_t>>& 存放所有同花顺
	//dst2 std::vector<std::vector<uint8_t>>& 存放所有顺子
	void CGameLogic::EnumConsecCardsByColor(
		uint8_t(**const pdst)[4],
		std::vector<std::vector<short>> const& ctx,
		std::vector<std::vector<uint8_t>> const& src,
		std::vector<bool> const& clr,
		std::vector<std::vector<uint8_t>>& dst1,
		std::vector<std::vector<uint8_t>>& dst2) {
		int i = 0;
		for (std::vector<std::vector<uint8_t>>::const_iterator it = src.begin();
			it != src.end(); ++it) {
			//printf("--->>> start ctx.size=%d\n", ctx[i].size());
			typedef uint8_t(*Ptr)[4];
			Ptr psrc[6] = { 0 };
			int colc[6] = { 0 };
			int k = 0;
			std::vector<short> const& pctx = ctx[i++];
			for (std::vector<short>::const_iterator ir = pctx.begin();
				ir != pctx.end(); ++ir) {
				//pdst第j组/牌数c
				uint8_t j = (0xFF & ((*ir) >> 8));
				uint8_t	c = (0xFF & (*ir));
				//牌值A,2,3,4,5,6,7,8,9,10,J,Q,K
				uint8_t vdst_c = GetCardValue((*pdst[j])[0]);
				uint8_t vsrc_s = GetCardValue((&it->front())[0]);
				uint8_t vsrc_p = GetCardValue((&it->front())[it->size() - 1]);
				//牌点2,3,4,5,6,7,8,9,10,J,Q,K,A
				uint8_t pdst_c = GetCardPoint((*pdst[j])[0]);
				uint8_t psrc_s = GetCardPoint((&it->front())[0]);
				uint8_t psrc_p = GetCardPoint((&it->front())[it->size() - 1]);
				assert(
					(vdst_c >= vsrc_s && vdst_c <= vsrc_p) ||
					(pdst_c >= psrc_s && pdst_c <= psrc_p));
				//PrintCardList(&(*pdst[j])[0], c);
				colc[k] = c;
				psrc[k++] = pdst[j];
			}
			if (k > 0) {
				std::vector<std::vector<int>> vec;
				//psrc组与组之间按牌值升序排列(从小到大)
				//SortCards_src(psrc, k, true, true);
				//printf("---\n");
				//PrintCardList(&it->front(), it->size());
				//f(k)=C(n,1)
				//Muti(k)=f(1)*f(2)...*f(k)
				//////从选取的k组中分别任取一张牌的组合数 //////
				int c = DepthVisit(4, k, psrc, colc, *it, dst1, dst2);
				//printf("--- c=%d dst1(sameclr)=%d dst2=%d\n", c, dst1.size(), dst2.size());
			}
			else {
				clr[i - 1] ? dst1.push_back(*it) : dst2.push_back(*it);
			}
			//printf("--->>> end\n\n");
		}
	}
	
	//求组合C(n,1)*C(n,1)...*C(n,1)
	//f(k)=C(n,1)
	//Muti(k)=f(1)*f(2)...*f(k)
	//n int 访问广度
	//k int 访问深度
	//深度优先遍历，由浅到深，广度遍历，由里向外
	int CGameLogic::DepthVisit(int n,
		int k,
		uint8_t(*const*const psrc)[4],
		int const* colc,
		std::vector<uint8_t> const& vec,
		std::vector<std::vector<uint8_t>>& dst0,
		std::vector<std::vector<uint8_t>>& dst1) {
		int c = 0;
		static int const KDEPTH = MaxSZ;
		int e[KDEPTH + 1] = { 0 };
		e[0] = k;
		//dst0.clear();
		//dst1.clear();
		return DepthC(n, k, e, c, psrc, colc, vec, dst0, dst1);
	}
	
	//递归求组合C(n,1)*C(n,1)...*C(n,1)
	//f(k)=C(n,1)
	//Muti(k)=f(1)*f(2)...*f(k)
	//n int 访问广度
	//k int 访问深度
	//深度优先遍历，由浅到深，广度遍历，由里向外
	int CGameLogic::DepthC(int n,
		int k, int *e, int& c,
		uint8_t(*const*const psrc)[4],
		int const* colc,
		std::vector<uint8_t> const& vec,
		std::vector<std::vector<uint8_t>>& dst0,
		std::vector<std::vector<uint8_t>>& dst1) {
		for (int i = colc[k - 1]/*n*/; i > 0; --i) {
			//if ((*psrc[k - 1])[i - 1] == 0) {
			//	continue;
			//}
			assert(k > 0);
			//psrc第[j - 1]组第e[j] - 1张(0/1/2/3)
			//psrc第[j - 1]组第e[j] - 1张(0/1/2)
			//psrc第[j - 1]组第e[j] - 1张(0/1)
			e[k] = i;
			if (k > 1) {
				DepthC(colc[k - 1]/*n*/, k - 1, e, c, psrc, colc, vec, dst0, dst1);
			}
			else {
				++c;
				std::vector<uint8_t> v;
				//深度1...e[0]
				for (int j = e[0]; j > 0; --j) {
					//printf("%d", e[j]);
					//printf("%s", StringCard((*psrc[j - 1])[e[j] - 1]).c_str());
					v.push_back((*psrc[j - 1])[e[j] - 1]);
				}
				//printf(",");
				//PrintCardList(&vec[0], vec.size());
				//用2345678910JQKA来占位 size = MaxSZ
				uint8_t mark[MaxSZ] = { 0 };
				if (GetCardValue(*vec.rbegin()) == A) {
					//按牌点来补位
					FillCardsMarkLocByPoint(mark, MaxSZ, &vec[0], vec.size(), false);
					FillCardsMarkLocByPoint(mark, MaxSZ, &v[0], v.size(), false);
				}
				//用A2345678910JQK来占位 size = MaxSZ
				else {
					//按牌值来补位
					FillCardsMarkLoc(mark, MaxSZ, &vec[0], vec.size(), false);
					FillCardsMarkLoc(mark, MaxSZ, &v[0], v.size(), false);
				}
				v.clear();
				bool sameclr = true;
				uint8_t c_pre = 0;
				for (int j = 0; j < MaxSZ; ++j) {
					if (mark[j] > 0) {
						if (c_pre == 0) {
							c_pre = GetCardColor(mark[j]);
						}
						else if(c_pre != GetCardColor(mark[j])) {
							sameclr = false;
						}
						v.push_back(mark[j]);
					}
				}
				//PrintCardList(&v[0], v.size());
				sameclr ?
					dst0.push_back(v) :
					dst1.push_back(v);
				//v.clear();
			}
		}
		return c;
	}

	//简单牌型分类/重复(四张/三张/二张)/同花/顺子/同花顺/散牌
	//src uint8_t const* 牌源
	//n int 抽取n张(5/5/3)
	//pdst uint8_t(**const)[4] 衔接dst4/dst3/dst2
	//dst4 uint8_t(*)[4] 存放所有四张牌型，c4 四张牌型数
	//dst3 uint8_t(*)[4] 存放所有三张牌型，c3 三张牌型数
	//dst2 uint8_t(*)[4] 存放所有对子牌型，c2 对子牌型数
	//cpy uint8_t* 存放去重后的余牌/散牌(除去四张/三张/对子)///////
	//dst0 std::vector<std::vector<uint8_t>>& 存放所有同花色n张连续牌
	//dst1 std::vector<std::vector<uint8_t>>& 存放所有非同花n张连续牌
	//dstc std::vector<std::vector<uint8_t>>& 存放所有同花n张非连续牌
	void CGameLogic::ClassifyCards(uint8_t const* src, int len, int n,
		uint8_t(**const pdst)[4],
		uint8_t(*dst4)[4], int& c4,
		uint8_t(*dst3)[4], int& c3,
		uint8_t(*dst2)[4], int& c2,
		uint8_t *cpy, int& cpylen,
		std::vector<std::vector<uint8_t>>& dst0,
		std::vector<std::vector<uint8_t>>& dst1,
		std::vector<std::vector<uint8_t>>& dstc) {
		int c = 0/*c4 = 0, c3 = 0, c2 = 0, cpylen = 0*/;
		//uint8_t cpy[MaxSZ] = { 0 };
		//所有重复四张牌型
		int const size4 = 3;
		//uint8_t dst4[size4][4] = { 0 };
		//所有重复三张牌型
		int const size3 = 4;
		//uint8_t dst3[size3][4] = { 0 };
		//所有重复二张牌型
		int const size2 = 6;
		//uint8_t dst2[size2][4] = { 0 };
		//返回去重后的余牌/散牌cpy
		//printf("---\n");
		c = RemoveRepeatCards(src, len, pdst, dst4, c4, dst3, c3, dst2, c2, cpy, cpylen);
		//printf("---\n");
		//PrintCardList(cpy, cpylen);
		//printf("---\n\n");
#if 0
		//枚举单张组合牌(分别从四张/三张/对子中任抽一张组合牌)
		std::vector<std::vector<uint8_t>> all;
		std::vector<std::vector<short>> allctx;
		EnumCombineCards(psrc, dst4, c4, dst3, c3, dst2, c2, allctx, all, clr);
		printf("\n\nall = %d\n", all.size());
		//枚举所有五张连续牌型(同花顺/顺子)
		EnumConsecCards(all, allctx, cpy, cpylen, 5, ctx, dst);
		assert(allctx.size() == all.size());
#else
		//pdst组与组之间按牌值升序排列(从小到大)
		//SortCards_src(pdst, c, true, true);
		//枚举所有不区分花色n张连续牌型(同花顺/顺子)
		std::vector<std::vector<short>> ctx;
		std::vector<std::vector<uint8_t>> dst;
		std::vector<bool> clr;
		EnumConsecCards(pdst, c, cpy, cpylen, n, ctx, dst, clr);
		//枚举所有区分花色n张连续牌(同花顺/顺子)
		EnumConsecCardsByColor(pdst, ctx, dst, clr, dst0, dst1);
		//枚举所有同花色n张不连续牌型(同花)
		EnumSameColorCards(pdst, c, cpy, cpylen, n, ctx, dstc);
#endif
	}

	//求组合C(n,k)
	//n int 访问广度
	//k int 访问深度
	//深度优先遍历，由浅到深，广度遍历，由里向外
	int CGameLogic::FuncC(int n, int k,
		uint8_t(*const*const psrc)[4], int const r,
		std::vector<std::vector<uint8_t>>& dst) {
		int c = 0;
		static int const KDEPTH = MaxSZ;
		int e[KDEPTH + 1] = { 0 };
		e[0] = k;
		return FuncC(n, k, e, c, psrc, r, dst);
	}

	//递归求组合C(n,k)
	//n int 访问广度
	//k int 访问深度
	//深度优先遍历，由浅到深，广度遍历，由里向外
	int CGameLogic::FuncC(int n, int k, int *e, int& c,
		uint8_t(*const*const psrc)[4], int const r,
		std::vector<std::vector<uint8_t>>& dst) {
		for (int i = n; i > 0; --i) {
			if ((*psrc[r])[i - 1] == 0) {
				continue;
			}
			assert(k > 0);
			//psrc第[r]组第e[j] - 1张(0/1/2/3)
			//psrc第[r]组第e[j] - 1张(0/1/2)
			//psrc第[r]组第e[j] - 1张(0/1)
			e[k] = i;
			if (k > 1) {
				//递归C(i-1,k-1)
				FuncC(i - 1, k - 1, e, c, psrc, r, dst);
			}
			else {
				++c;
				std::vector<uint8_t> v;
				//深度1...e[0]
				for (int j = e[0]; j > 0; --j) {
					//printf("%d", e[j]);
					//printf("%s", StringCard((*psrc[r])[e[j] - 1]).c_str());
					v.push_back((*psrc[r])[e[j] - 1]);
				}
				//printf(",");
				dst.push_back(v);
				//v.clear();
			}
		}
		return c;
	}
	
	//葫芦牌型组牌规则，三张尽量取最大(不同的手牌，三张决定了葫芦之间的大小)，对子取值越小，对其余组牌干扰越小
	static bool As32byPointG(const uint8_t(*const a)[5], const uint8_t(*const b)[5]) {
		//比较三张的大小(中间的牌)
		uint8_t p0 = CGameLogic::GetCardPoint(a[0][2]);
		uint8_t p1 = CGameLogic::GetCardPoint(b[0][2]);
		if (p0 != p1) {
			return p0 > p1;
		}
		//三张大小相同，对子取最小
		uint8_t sp0 = CGameLogic::GetCardPoint(a[0][0]);
		if (sp0 == p0) {
			sp0 = CGameLogic::GetCardPoint(a[0][4]);
		}
		uint8_t sp1 = CGameLogic::GetCardPoint(b[0][0]);
		if (sp1 == p1) {
			sp1 = CGameLogic::GetCardPoint(b[0][4]);
		}
		return sp0 < sp1;
	}
	//葫芦牌型组牌规则，三张尽量取最大(不同的手牌，三张决定了葫芦之间的大小)，对子取值越小，对其余组牌干扰越小
	static bool As32byPointL(const uint8_t(*const a)[5], const uint8_t(*const b)[5]) {
		//比较三张的大小(中间的牌)
		uint8_t p0 = CGameLogic::GetCardPoint(a[0][2]);
		uint8_t p1 = CGameLogic::GetCardPoint(b[0][2]);
		if (p0 != p1) {
			return p0 < p1;
		}
		//三张大小相同，对子取最小
		uint8_t sp0 = CGameLogic::GetCardPoint(a[0][0]);
		if (sp0 == p0) {
			sp0 = CGameLogic::GetCardPoint(a[0][4]);
		}
		uint8_t sp1 = CGameLogic::GetCardPoint(b[0][0]);
		if (sp1 == p1) {
			sp1 = CGameLogic::GetCardPoint(b[0][4]);
		}
		return sp0 > sp1;
	}
	//葫芦牌型组牌规则，三张尽量取最大(不同的手牌，三张决定了葫芦之间的大小)，对子取值越小，对其余组牌干扰越小
	static void SortCardsByPoint_src32(uint8_t(**const psrc)[5], int n, bool ascend) {
		if (ascend) {
			std::sort(psrc, psrc + n, As32byPointL);
		}
		else {
			std::sort(psrc, psrc + n, As32byPointG);
		}
	}

	//枚举所有葫芦(一组三条加上一组对子)
	//psrc uint8_t(**const)[4] 衔接dst4/dst3/dst2
	//src4 uint8_t(*const)[4] 所有四张牌型牌源，c4 四张牌型数
	//src3 uint8_t(*const)[4] 所有三张牌型牌源，c3 三张牌型数
	//src2 uint8_t(*const)[4] 所有对子牌型牌源，c2 对子牌型数
	//dst std::vector<std::vector<uint8_t>>& 存放所有葫芦牌型
	int CGameLogic::EnumCards32(
		uint8_t(**const psrc)[4],
		uint8_t(*const src4)[4], int const c4,
		uint8_t(*const src3)[4], int const c3,
		uint8_t(*const src2)[4], int const c2,
		std::vector<std::vector<uint8_t>>& dst) {
		//printf("--- *** 枚举所有葫芦\n");
		int c = 0;
		uint8_t src[MaxEnumSZ][5] = { 0 };
		int n = c4 + c3 + c2;
		CFuncC fnC;
		std::vector<std::vector<int>> vec;
		//psrc组与组之间按牌值升序排列(从小到大)
		//SortCards_src(psrc, n, true, true);
		//////从n组里面选取任意2组的组合数C(n,2) //////
		/*c = */fnC.FuncC(n, 2, vec);
		//for (int i = 0; i < n; ++i) {
		//	PrintCardList(psrc[i][0], get_card_c(psrc[i][0], 4));
		//}
		//printf("\n--- *** ------------------- C(%d,%d)=%d\n", n, 2, c);
		for (std::vector<std::vector<int>>::iterator it = vec.begin();
			it != vec.end(); ++it) {
			assert(2 == it->size());
			//printf("\n--- *** start\n");
#if 0
			for (std::vector<int>::iterator ir = it->begin();
				ir != it->end(); ++ir) {
				printf("%d", *ir);
			}
			printf("\n---\n");
#endif
			//PrintCardList(psrc[(*it)[0]][0], get_card_c(psrc[(*it)[0]][0], 4));
			//PrintCardList(psrc[(*it)[1]][0], get_card_c(psrc[(*it)[1]][0], 4));
			//printf("---\n");
			//////从选取的2组中一组取三张，另一组取二张 //////
			int c0 = get_card_c(psrc[(*it)[0]][0], 4);
			int c1 = get_card_c(psrc[(*it)[1]][0], 4);
			if (c0 >= 3) {
				//第0组取三张C(c0,3)，第1组取二张C(c1,2)
				std::vector<std::vector<uint8_t>> v0, v1;
				//psrc[(*it)[0]]组的牌值小于psrc[(*it)[1]]组
				int c_0 = FuncC(c0, 3, psrc, (*it)[0], v0);
				int c_1 = FuncC(c1, 2, psrc, (*it)[1], v1);
				//printf("\n--- C(%d,3)=%d，C(%d,2)=%d\n", c0, c_0, c1, c_1);
				//////从v1,v0中分别任取一项组合葫芦牌型C(v0.size(),1)*C(v1.size(),1) //////
				for (std::vector<std::vector<uint8_t>>::iterator it2 = v1.begin();
					it2 != v1.end(); ++it2) {//对子
					for (std::vector<std::vector<uint8_t>>::iterator it3 = v0.begin();
						it3 != v0.end(); ++it3) {//三张
#if 0
						std::vector<uint8_t> v(&it2->front(), &it2->front() + it2->size());
						v.resize(v.size() + it3->size());
						memcpy(&v.front() + v.size(), &it3->front(), it3->size());
#else
						//std::vector<uint8_t> v;
						//for (std::vector<uint8_t>::iterator ir = it2->begin(); ir != it2->end(); ++ir) {
						//	v.push_back(*ir);
						//}
						//for (std::vector<uint8_t>::iterator ir = it3->begin(); ir != it3->end(); ++ir) {
						//	v.push_back(*ir);
						//}
						assert(c < MaxEnumSZ);
						assert(it2->size() + it3->size() == 5);
						memcpy(&(src[c])[0], &it2->front(), it2->size());
						memcpy(&(src[c++])[it2->size()], &it3->front(), it3->size());
#endif
						//PrintCardList(&v.front(), v.size());
						//dst.push_back(v);
					}
				}
			}
			if (c1 >= 3) {
				//第0组取二张C(c0,2)，第1组取三张C(c1,3)
				std::vector<std::vector<uint8_t>> v0, v1;
				//psrc[(*it)[0]]组的牌值小于psrc[(*it)[1]]组
				int c_0 = FuncC(c0, 2, psrc, (*it)[0], v0);
				int c_1 = FuncC(c1, 3, psrc, (*it)[1], v1);
				//printf("\n--- C(%d,2)=%d，C(%d,3)=%d\n", c0, c_0, c1, c_1);
				//////从v1,v0中分别任取一项组合葫芦牌型C(v0.size(),1)*C(v1.size(),1) //////
				for (std::vector<std::vector<uint8_t>>::iterator it2 = v1.begin();
					it2 != v1.end(); ++it2) {//三张
					for (std::vector<std::vector<uint8_t>>::iterator it3 = v0.begin();
						it3 != v0.end(); ++it3) {//对子
#if 0
						std::vector<uint8_t> v(&it2->front(), &it2->front() + it2->size());
						v.resize(v.size() + it3->size());
						memcpy(&v.front() + v.size(), &it3->front(), it3->size());
#else
						//std::vector<uint8_t> v;
						//for (std::vector<uint8_t>::iterator ir = it2->begin(); ir != it2->end(); ++ir) {
						//	v.push_back(*ir);
						//}
						//for (std::vector<uint8_t>::iterator ir = it3->begin(); ir != it3->end(); ++ir) {
						//	v.push_back(*ir);
						//}
						assert(c < MaxEnumSZ);
						assert(it2->size() + it3->size() == 5);
						memcpy(&(src[c])[0], &it2->front(), it2->size());
						memcpy(&(src[c++])[it2->size()], &it3->front(), it3->size());
#endif
						//PrintCardList(&v.front(), v.size());
						//dst.push_back(v);
					}
				}
			}
			//printf("\n--- *** end c=%d\n", dst.size());
		}
		
		{
			n = c;
			//uint8_t(*src[6])[4] = { 0 };
			typedef uint8_t(*Ptr)[5];
			Ptr psrc[MaxEnumSZ] = { 0 };
			c = 0;
			for (int i = 0; i < n; ++i) {
				psrc[c++] = &src[i];
			}
			//葫芦牌型组牌规则，三张尽量取最大(不同的手牌，三张决定了葫芦之间的大小)，对子取值越小，对其余组牌干扰越小
			//psrc组与组之间按牌值升序排列(从小到大)
			SortCardsByPoint_src32(psrc, n, true);
			for (int i = 0; i < n; ++i) {
				std::vector<uint8_t> v(&(*psrc[i])[0], &(*psrc[i])[0] + 5);
				dst.push_back(v);
			}
		}
		return dst.size();
	}
	
	//枚举所有三条(三张值相同的牌)
	//psrc uint8_t(**const)[4] 衔接dst4/dst3/dst2
	//src4 uint8_t(*const)[4] 所有四张牌型牌源，c4 四张牌型数
	//src3 uint8_t(*const)[4] 所有三张牌型牌源，c3 三张牌型数
	//src2 uint8_t(*const)[4] 所有对子牌型牌源，c2 对子牌型数
	//dst std::vector<std::vector<uint8_t>>& 存放所有三条牌型
	int CGameLogic::EnumCards30(
		uint8_t(**const psrc)[4],
		uint8_t(*const src4)[4], int const c4,
		uint8_t(*const src3)[4], int const c3,
		uint8_t(*const src2)[4], int const c2,
		std::vector<std::vector<uint8_t>>& dst) {
		//printf("--- *** 枚举所有三条\n");
		int n = c4 + c3 + c2;
		CFuncC fnC;
		std::vector<std::vector<int>> vec;
		//psrc组与组之间按牌值升序排列(从小到大)
		//SortCards_src(psrc, n, true, true);
		//////从n组里面选取任意1组的组合数C(n,1) //////
		int c = fnC.FuncC(n, 1, vec);
		//for (int i = 0; i < n; ++i) {
		//	PrintCardList(psrc[i][0], get_card_c(psrc[i][0], 4));
		//}
		//printf("\n--- *** ------------------- C(%d,%d)=%d\n", n, 1, c);
		for (std::vector<std::vector<int>>::iterator it = vec.begin();
			it != vec.end(); ++it) {
			assert(1 == it->size());
			//printf("\n--- *** start\n");
#if 0
			for (std::vector<int>::iterator ir = it->begin();
				ir != it->end(); ++ir) {
				printf("%d", *ir);
			}
			printf("\n---\n");
#endif
			//PrintCardList(psrc[(*it)[0]][0], get_card_c(psrc[(*it)[0]][0], 4));
			//PrintCardList(psrc[(*it)[1]][0], get_card_c(psrc[(*it)[1]][0], 4));
			//printf("---\n");
			//////从选取的组中取三张 //////
			int c0 = get_card_c(psrc[(*it)[0]][0], 4);
			if (c0 >= 3) {
				//取三张C(c0,3)
				std::vector<std::vector<uint8_t>> v0;
				int c_0 = FuncC(c0, 3, psrc, (*it)[0], v0);
				//printf("\n--- C(%d,3)=%d\n", c0, c_0);
				//////从v0中任取一项组合三条牌型C(v0.size(),1) //////
				for (std::vector<std::vector<uint8_t>>::iterator it3 = v0.begin();
					it3 != v0.end(); ++it3) {//三张
#if 0
					std::vector<uint8_t> v(&it3->front(), &it3->front() + it3->size());
#else
					std::vector<uint8_t> v;
					for (std::vector<uint8_t>::iterator ir = it3->begin(); ir != it3->end(); ++ir) {
						v.push_back(*ir);
					}
#endif
					//PrintCardList(&v.front(), v.size());
					dst.push_back(v);
				}
			}
			//printf("\n--- *** end c=%d\n", dst.size());
		}
		//for (std::vector<std::vector<uint8_t>>::const_iterator it = dst.begin();
		//	it != dst.end(); ++it) {
		//	PrintCardList(&it->front(), it->size());
		//}
		return dst.size();
	}
	
	//枚举所有两对(两个对子加上一张单牌)
	//psrc uint8_t(**const)[4] 衔接dst4/dst3/dst2
	//src4 uint8_t(*const)[4] 所有四张牌型牌源，c4 四张牌型数
	//src3 uint8_t(*const)[4] 所有三张牌型牌源，c3 三张牌型数
	//src2 uint8_t(*const)[4] 所有对子牌型牌源，c2 对子牌型数
	//dst std::vector<std::vector<uint8_t>>& 存放所有两对牌型
	int CGameLogic::EnumCards22(
		uint8_t(**const psrc)[4],
		uint8_t(*const src4)[4], int const c4,
		uint8_t(*const src3)[4], int const c3,
		uint8_t(*const src2)[4], int const c2,
		std::vector<std::vector<uint8_t>>& dst) {
		//printf("--- *** 枚举所有两对\n");
		int n = c4 + c3 + c2;
		CFuncC fnC;
		std::vector<std::vector<int>> vec;
		//psrc组与组之间按牌值升序排列(从小到大)
		//SortCards_src(psrc, n, true, true);
		//////从n组里面选取任意2组的组合数C(n,2) //////
		int c = fnC.FuncC(n, 2, vec);
		//for (int i = 0; i < n; ++i) {
		//	PrintCardList(psrc[i][0], get_card_c(psrc[i][0], 4));
		//}
		//printf("\n--- *** ------------------- C(%d,%d)=%d\n", n, 2, c);
		for (std::vector<std::vector<int>>::iterator it = vec.begin();
			it != vec.end(); ++it) {
			assert(2 == it->size());
			//printf("\n--- *** start\n");
#if 0
			for (std::vector<int>::iterator ir = it->begin();
				ir != it->end(); ++ir) {
				printf("%d", *ir);
			}
			printf("\n---\n");
#endif
			//PrintCardList(psrc[(*it)[0]][0], get_card_c(psrc[(*it)[0]][0], 4));
			//PrintCardList(psrc[(*it)[1]][0], get_card_c(psrc[(*it)[1]][0], 4));
			//printf("---\n");
			//////从选取的2组中一组取二张，另一组取二张 //////
			int c0 = get_card_c(psrc[(*it)[0]][0], 4);
			int c1 = get_card_c(psrc[(*it)[1]][0], 4);
			assert(c0 >= 2 && c1 >= 2);
			//第0组取二张C(c0,2)，第1组取二张C(c1,2)
			std::vector<std::vector<uint8_t>> v0, v1;
			int c_0 = FuncC(c0, 2, psrc, (*it)[0], v0);
			int c_1 = FuncC(c1, 2, psrc, (*it)[1], v1);
			//printf("\n--- C(%d,2)=%d，C(%d,2)=%d\n", c0, c_0, c1, c_1);
			//////从v1,v0中分别任取一项组合两对子牌型C(v0.size(),1)*C(v1.size(),1) //////
			for (std::vector<std::vector<uint8_t>>::iterator it2 = v1.begin();
				it2 != v1.end(); ++it2) {//对子
				for (std::vector<std::vector<uint8_t>>::iterator it3 = v0.begin();
					it3 != v0.end(); ++it3) {//对子
#if 0
					std::vector<uint8_t> v(&it2->front(), &it2->front() + it2->size());
					v.resize(v.size() + it3->size());
					memcpy(&v.front() + v.size(), &it3->front(), it3->size());
#else
					std::vector<uint8_t> v;
					for (std::vector<uint8_t>::iterator ir = it2->begin(); ir != it2->end(); ++ir) {
						v.push_back(*ir);
					}
					for (std::vector<uint8_t>::iterator ir = it3->begin(); ir != it3->end(); ++ir) {
						v.push_back(*ir);
					}
#endif
					//PrintCardList(&v.front(), v.size());
					dst.push_back(v);
				}
			}
			//printf("\n--- *** end c=%d\n", dst.size());
		}
		//for (std::vector<std::vector<uint8_t>>::const_iterator it = dst.begin();
		//	it != dst.end(); ++it) {
		//	PrintCardList(&it->front(), it->size());
		//}
		return dst.size();
	}
	
	//枚举所有对子(一对)
	//psrc uint8_t(**const)[4] 衔接dst4/dst3/dst2
	//src4 uint8_t(*const)[4] 所有四张牌型牌源，c4 四张牌型数
	//src3 uint8_t(*const)[4] 所有三张牌型牌源，c3 三张牌型数
	//src2 uint8_t(*const)[4] 所有对子牌型牌源，c2 对子牌型数
	//dst std::vector<std::vector<uint8_t>>& 存放所有对子牌型
	int CGameLogic::EnumCards20(
		uint8_t(**const psrc)[4],
		uint8_t(*const src4)[4], int const c4,
		uint8_t(*const src3)[4], int const c3,
		uint8_t(*const src2)[4], int const c2,
		std::vector<std::vector<uint8_t>>& dst) {
		//printf("--- *** 枚举所有对子\n");
		int n = c4 + c3 + c2;
		CFuncC fnC;
		std::vector<std::vector<int>> vec;
		//psrc组与组之间按牌值升序排列(从小到大)
		//SortCards_src(psrc, n, true, true);
		//////从n组里面选取任意1组的组合数C(n,1) //////
		int c = fnC.FuncC(n, 1, vec);
		//for (int i = 0; i < n; ++i) {
		//	PrintCardList(psrc[i][0], get_card_c(psrc[i][0], 4));
		//}
		//printf("\n--- *** ------------------- C(%d,%d)=%d\n", n, 1, c);
		for (std::vector<std::vector<int>>::iterator it = vec.begin();
			it != vec.end(); ++it) {
			assert(1 == it->size());
			//printf("\n--- *** start\n");
#if 0
			for (std::vector<int>::iterator ir = it->begin();
				ir != it->end(); ++ir) {
				printf("%d", *ir);
			}
			printf("\n---\n");
#endif
			//PrintCardList(psrc[(*it)[0]][0], get_card_c(psrc[(*it)[0]][0], 4));
			//PrintCardList(psrc[(*it)[1]][0], get_card_c(psrc[(*it)[1]][0], 4));
			//printf("---\n");
			//////从选取的组中取二张 //////
			int c0 = get_card_c(psrc[(*it)[0]][0], 4);
			assert(c0 >= 2);
			//取二张C(c0,2)
			std::vector<std::vector<uint8_t>> v0;
			int c_0 = FuncC(c0, 2, psrc, (*it)[0], v0);
			//printf("\n--- C(%d,2)=%d\n", c0, c_0);
			//////从v0中任取一项组合对子牌型C(v0.size(),1) //////
			for (std::vector<std::vector<uint8_t>>::iterator it3 = v0.begin();
				it3 != v0.end(); ++it3) {//二张
#if 0
				std::vector<uint8_t> v(&it3->front(), &it3->front() + it3->size());
#else
				std::vector<uint8_t> v;
				for (std::vector<uint8_t>::iterator ir = it3->begin(); ir != it3->end(); ++ir) {
					v.push_back(*ir);
				}
#endif
				//PrintCardList(&v.front(), v.size());
				dst.push_back(v);
			}
			//printf("\n--- *** end c=%d\n", dst.size());
		}
		//for (std::vector<std::vector<uint8_t>>::const_iterator it = dst.begin();
		//	it != dst.end(); ++it) {
		//	PrintCardList(&it->front(), it->size());
		//}
		return dst.size();
	}
	
	//按照尾墩5张/中墩5张/头墩3张依次抽取枚举普通牌型
	//src uint8_t const* 手牌余牌(13/8/3)，初始13张，按5/5/3依次抽，余牌依次为13/8/3
	//n int 抽取n张(5/5/3) 第一次抽5张余8张，第二次抽5张余3张，第三次取余下3张抽完
	//dst4 uint8_t(*)[4] 存放所有四张牌型，c4 四张牌型数
	//dst3 uint8_t(*)[4] 存放所有三张牌型，c3 三张牌型数
	//dst2 uint8_t(*)[4] 存放所有对子牌型，c2 对子牌型数
	//cpy uint8_t* 存放去重后的余牌/散牌(除去四张/三张/对子)
	//v123sc std::vector<std::vector<uint8_t>>& 存放所有同花色n张连续牌
	//v123 std::vector<std::vector<uint8_t>>& 存放所有非同花n张连续牌
	//vsc std::vector<std::vector<uint8_t>>& 存放所有同花n张非连续牌
	//v40 std::vector<std::vector<uint8_t>>& 存放所有铁支
	//v32 std::vector<std::vector<uint8_t>>& 存放所有葫芦
	//v30 std::vector<std::vector<uint8_t>>& 存放所有三条
	//v22 std::vector<std::vector<uint8_t>>& 存放所有两对
	//v20 std::vector<std::vector<uint8_t>>& 存放所有对子
	void CGameLogic::EnumCards(uint8_t const* src, int len, int n,
		uint8_t(*dst4)[4], int& c4,
		uint8_t(*dst3)[4], int& c3,
		uint8_t(*dst2)[4], int& c2,
		uint8_t *cpy, int& cpylen,
		std::vector<std::vector<uint8_t>>& v123sc,
		std::vector<std::vector<uint8_t>>& v123,
		std::vector<std::vector<uint8_t>>& vsc,
		std::vector<std::vector<uint8_t>>& v40,
		std::vector<std::vector<uint8_t>>& v32,
		std::vector<std::vector<uint8_t>>& v30,
		std::vector<std::vector<uint8_t>>& v22,
		std::vector<std::vector<uint8_t>>& v20) {
		//合并dst4/dst3/dst2到psrc然后排序
		//uint8_t(*ptr[6])[4] = { 0 };
		typedef uint8_t(*Ptr)[4];
		Ptr psrc[6] = { 0 };
		//所有同花色n张连续牌
		v123sc.clear();
		//所有非同花n张连续牌
		v123.clear();
		//所有同花n张非连续牌
		vsc.clear();
		//简单牌型分类/重复(四张/三张/二张)/同花/顺子/同花顺/散牌
		ClassifyCards(src, len, n, psrc, dst4, c4, dst3, c3, dst2, c2, cpy, cpylen, v123sc, v123, vsc);
		//psrc组与组之间按牌值升序排列(从小到大)
		SortCards_src(psrc, c4 + c3 + c2, true, true);
		if (n >= 5) {
			//所有葫芦(一组三条加上一组对子)
			v32.clear();
			EnumCards32(psrc, dst4, c4, dst3, c3, dst2, c2, v32);
		}
		if (n >= 3) {
			//所有三条(三张值相同的牌)
			v30.clear();
			EnumCards30(psrc, dst4, c4, dst3, c3, dst2, c2, v30);
		}
		if (n >= 4) {
			//所有两对(两个对子加上一张单牌)
			v22.clear();
			EnumCards22(psrc, dst4, c4, dst3, c3, dst2, c2, v22);
		}
		if (n >= 2) {
			//所有对子(一对)
			v20.clear();
			EnumCards20(psrc, dst4, c4, dst3, c3, dst2, c2, v20);
		}
		if (n >= 4) {
			//所有铁支(四张值相同的牌)
			v40.clear();
			for (int i = 0; i < c4; ++i) {
				//std::vector<uint8_t> v(&(*&dst4[i])[0], &(*&dst4[i])[0] + 4);
				std::vector<uint8_t> v(&(dst4[i])[0], &(dst4[i])[0] + 4);
				v40.push_back(v);
			}
		}
	}

	//是否连号n张牌
	bool CGameLogic::CheckConsecCards(uint8_t const* src, int len)
	{
		//手牌按牌值从小到大排序(A23...QK)
		//SortCards(cards, len, true, true, true);
		//判断第二张牌到最后一张牌的连续性([A]23....QK)
		for (int i = 1; i < len - 1; ++i) {
			uint8_t v0 = GetCardValue(src[i]);
			uint8_t v1 = GetCardValue(src[i + 1]);
			if (v0 + 1 != v1) {
				//有一个不连续
				return false;
			}
		}
		//第二张牌到最后一张牌连续
		uint8_t v0 = GetCardValue(src[0]);
		uint8_t v1 = GetCardValue(src[1]);
		if (v0 + 1 == v1) {
			//前两张牌也连续
			return true;
		}
		//前两张牌不连续，判断是否A...K
		if (v0 + 12 == GetCardValue(src[len - 1])) {
			//手牌按牌点从小到大排序([A]...QKA)
			//SortCards(src, len, false, true, true);
			return true;
		}
		return false;
	}

	//是否同花n张牌
	bool CGameLogic::CheckSameColorCards(uint8_t const* src, int len)
	{
		for (int i = 0; i < len - 1; ++i) {
			uint8_t c0 = GetCardColor(src[i]);
			uint8_t c1 = GetCardColor(src[i + 1]);
			if (c0 != c1) {
				//有不同花色的牌
				return false;
			}
		}
		return true;
	}

	//至尊青龙/一条龙(十三水)/十二皇族
	HandTy CGameLogic::CheckDragonRoyal(uint8_t const* src, int len) {
		HandTy specialTy = TyNil;
		{
			//十三张连续牌
			if (CheckConsecCards(src, len)) {
				if (CheckSameColorCards(src, len)) {
					//至尊青龙：同花A到K的牌型，A2345678910JQK
					return specialTy = TyZZQDragon;
				}
				//一条龙(十三水)：A到K的牌型，非同花，A2345678910JQK
				return specialTy = TyOneDragon;
			}
		}
		{
			//十二皇族：十三张全是J，Q，K，A的牌型
			int i = 0;
			while (i < len) {
				uint8_t v = GetCardValue(src[i++]);
				if (v != A) {
					if (v >= J) {
						//牌值从小到大
						specialTy = Ty12Royal;
					}
					break;
				}
			}
		}
		return specialTy;
	}
	
	//凑一色：全是红牌(方块/红心)或黑牌(黑桃/梅花)
	HandTy CGameLogic::CheckAllOneColor(uint8_t const* src, int len) {
		HandTy specialTy = TyAllOneColor;
		uint8_t c0 = GetCardColor(src[0]);
		switch (c0)
		{
		case Spade:
		case Club:
			for (int i = 1; i < len; ++i) {
				uint8_t ci = GetCardColor(src[i]);
				if (ci == Heart || ci == Diamond) {
					return specialTy = TyNil;
				}
			}
			break;
		case Heart:
		case Diamond:
			for (int i = 1; i < len; ++i) {
				uint8_t ci = GetCardColor(src[i]);
				if (ci == Spade || ci == Club) {
					return specialTy = TyNil;
				}
			}
			break;
		}
		return specialTy;
	}
	
	//全大：全是8至A的牌型
	HandTy CGameLogic::CheckAllBig(uint8_t const* src, int len) {
		HandTy specialTy = TyNil;
		int i = 0;
		while (i < len) {
			uint8_t v = GetCardValue(src[i++]);
			if (v != A) {
				if (v >= 8) {
					//全大/牌值从小到大
					specialTy = TyAllBig;
				}
				break;
			}
		}
		return specialTy;
	}
	
	//全小：全是2至8的牌型
	HandTy CGameLogic::CheckAllSmall(uint8_t const* src, int len) {
		HandTy specialTy = TyNil;
		if (GetCardValue(src[0]) != A) {
			if (GetCardValue(src[len - 1]) <= 8) {
				//全小/牌值从小到大
				specialTy = TyAllSmall;
			}
		}
		return specialTy;
	}
	
	void CGameLogic::classify_t::PrintCardList() {
		for (int i = 0; i < c4; ++i) {
			CGameLogic::PrintCardList(&(dst4[i])[0], 4);
		}
		for (int i = 0; i < c3; ++i) {
			CGameLogic::PrintCardList(&(dst3[i])[0], 3);
		}
		for (int i = 0; i < c2; ++i) {
			CGameLogic::PrintCardList(&(dst2[i])[0], 2);
		}
		printf("---\n");
		CGameLogic::PrintCardList(cpy, cpylen);
	}

	//手牌牌型分析(特殊牌型判断/枚举三墩组合)，算法入口 /////////
	//src uint8_t const* 一副手牌(13张)
	//n int 最大枚举多少组墩(头墩&中墩&尾墩加起来为一组)
	//chairID int 玩家座椅ID
	//hand handinfo_t& 保存手牌信息
	int CGameLogic::AnalyseHandCards(uint8_t const* src, int len, int n, handinfo_t& hand) {
		int c = 0;
		uint8_t cpy[MaxSZ] = { 0 }, cpy2[MaxSZ] = { 0 }, cpy3[MaxSZ] = { 0 };
		int cpylen = 0, cpylen2 = 0, cpylen3 = 0;

		uint8_t const* psrc = NULL, *psrc2 = NULL;
		int lensrc = 0, lensrc2 = 0;
		
		//叶子节点(头墩)/子节点(中墩)/根节点(尾墩)
		int cursorLeaf = 0, cursorChild = 0, cursorRoot = 0;
		HandTy tyLeaf = TyNil, tyChild = TyNil, tyRoot = TyNil;
		EnumList::EnumCards const *leaf = NULL, *child = NULL, *root = NULL;
		
		//防止重复添加
		std::map<int64_t, bool> masks;
		int64_t maskRoot = 0, maskChild = 0;

		hand.Init();
		//叶子节点列表
		//枚举几组最优墩(头墩&中墩&尾墩加起来为一组)，由叶子节点向上往根节点遍历
		std::vector<EnumList::TraverseTreeNode>& leafList = hand.leafList;
		//根节点：初始枚举所有牌型列表
		EnumList *& rootEnumList = hand.rootEnumList;
		assert(rootEnumList != NULL);
		hand.Reset();
		
		//枚举尾墩/5张 //////
		EnumCards(src, len, 5, hand.classify, *rootEnumList, DunLast);
	end:
		while (c < n) {
			//返回一个枚举牌型及对应的余牌
			//按同花顺/铁支/葫芦/同花/顺子/三条/两对/对子/散牌的顺序
			memset(cpy, 0, MaxSZ * sizeof(uint8_t));
			if (!rootEnumList->GetNextEnumItem(src, len, root, tyRoot, cursorRoot, cpy, cpylen)) {
				break;
			}
			
			//printf("取尾墩 = [%s] ", StringCardType(tyRoot).c_str());
			//PrintCardList(&root->front(), root->size());
			//rootEnumList->PrintCursorEnumCards();

			//除去尾墩后的余牌
			psrc = cpy;
			lensrc = cpylen;
			
			//子节点：根节点当前枚举项牌型对应余牌枚举子项列表
			EnumList*& childEnumList = rootEnumList->GetCursorChildItem(cursorRoot);
			assert(childEnumList == NULL);
			childEnumList = new EnumList();
			//指向父节点/对应父节点游标位置
			childEnumList->parent_ = rootEnumList;
			childEnumList->parentcursor_ = cursorRoot;
			
			//计算根节点游标掩码
			maskRoot = cursorRoot & 0xFFFFFFFF;

			classify_t classify = { 0 };
			//从余牌中枚举中墩/5张 //////
			EnumCards(psrc, lensrc, 5, classify, *childEnumList, DunSecond);
			while (c < n) {
				//返回一个枚举牌型及对应的余牌
				//按同花顺/铁支/葫芦/同花/顺子/三条/两对/对子/散牌的顺序
				memset(cpy2, 0, MaxSZ * sizeof(uint8_t));
				if (!childEnumList->GetNextEnumItem(psrc, lensrc, child, tyChild, cursorChild, cpy2, cpylen2)) {
					
					//printf("--- *** --------------------------------------------------\n");
					//rootEnumList->PrintCursorEnumCards();
					//printf("--- *** --------------------------------------------------\n");
					
					std::map<int64_t, bool>::iterator it = masks.find(maskRoot);
					if (it == masks.end()) {
						//根节点为叶子节点，记录尾墩
						leafList.push_back(EnumList::TraverseTreeNode(rootEnumList, cursorRoot));
						if (++c >= n) {
							goto end;
						}
					}
					break;
				}
				//跳过三墩牌组合出现倒水情况
				{
					//牌型不同比牌型
					if (tyChild != tyRoot) {
						if (tyChild > tyRoot) {
							continue;
						}
					}
					else {
						//牌型相同从大到小比点数，葫芦牌型比较三张的大小(中间的牌)
						if (CompareCards(&root->front(), &child->front(), child->size(), tyChild) < 0) {
							continue;
						}
					}
				}
				
				masks[maskRoot] = true;
				
				//printf("\n取中墩 = [%s] ", StringCardType(tyChild).c_str());
				//PrintCardList(&child->front(), child->size());
				//childEnumList->PrintCursorEnumCards();

				//除去中墩后的余牌
				psrc2 = cpy2;
				lensrc2 = cpylen2;

				//叶子节点：子节点当前枚举项牌型对应余牌枚举子项列表
				EnumList*& leafEnumList = childEnumList->GetCursorChildItem(cursorChild);
				assert(leafEnumList == NULL);
				leafEnumList = new EnumList();
				//指向父节点/对应父节点游标位置
				leafEnumList->parent_ = childEnumList;
				leafEnumList->parentcursor_ = cursorChild;
				
				//计算子节点游标掩码
				maskChild = ((cursorChild & 0xFFFFFFFF) << 32) | (cursorRoot & 0xFFFFFFFF);
				
				classify_t classify = { 0 };
				//从余牌中枚举头墩/3张 //////
				EnumCards(psrc2, lensrc2, 3, classify, *leafEnumList, DunFirst);
				while (c < n) {
					//返回一个枚举牌型及对应的余牌
					//按同花顺/铁支/葫芦/同花/顺子/三条/两对/对子/散牌的顺序
					memset(cpy3, 0, MaxSZ * sizeof(uint8_t));
					if (!leafEnumList->GetNextEnumItem(psrc2, lensrc2, leaf, tyLeaf, cursorLeaf, cpy3, cpylen3)) {
						
						//printf("--- *** --------------------------------------------------\n");
						//childEnumList->PrintCursorEnumCards();
						//rootEnumList->PrintCursorEnumCards();
						//printf("--- *** --------------------------------------------------\n");
						
						std::map<int64_t, bool>::iterator it = masks.find(maskChild);
						if (it == masks.end()) {
							//子节点为叶子节点，记录中墩和尾墩，由叶子节点向上查找根节点
							leafList.push_back(EnumList::TraverseTreeNode(childEnumList, cursorChild));
							if (++c >= n) {
								goto end;
							}
						}
						break;
					}
					//跳过三墩牌组合出现倒水情况
					{
						//牌型不同比牌型
						if (tyLeaf != tyChild) {
							if(tyLeaf == Ty30 || tyLeaf == Ty20) {
								if (tyLeaf > tyChild) {
									continue;
								}
							}
						}
						else {
							if (tyLeaf == Ty30 || tyLeaf == Ty20) {
								//牌型相同从大到小比点数，葫芦牌型比较三张的大小(中间的牌)
								if (CompareCards(&child->front(), &leaf->front(), leaf->size(), tyLeaf) < 0) {
									continue;
								}
							}
						}
					}
					masks[maskChild] = true;

					//printf("\n取头墩 = [%s] ", StringCardType(tyLeaf).c_str());
					//PrintCardList(&leaf->front(), leaf->size());
					//leafEnumList->PrintCursorEnumCards();

					//printf("--- *** --------------------------------------------------\n");
					//leafEnumList->PrintCursorEnumCards();
					//childEnumList->PrintCursorEnumCards();
					//rootEnumList->PrintCursorEnumCards();
					//printf("--- *** --------------------------------------------------\n");
					
					//叶子节点作为叶子节点，记录头墩，中墩和尾墩，由叶子节点向上查找父节点和根节点
					leafList.push_back(EnumList::TraverseTreeNode(leafEnumList, cursorLeaf));
					if (++c >= n) {
						goto end;
					}
				}
			}
		}
		hand.CalcHandCardsType(src, len, info);
		return c;
	}
	
	//确定手牌牌型
	void CGameLogic::handinfo_t::CalcHandCardsType(uint8_t const* src, int len, classify_t& info) {
		
		//叶子节点(头墩)/子节点(中墩)/根节点(尾墩)
		int cursorLeaf = 0, cursorChild = 0, cursorRoot = 0;
		HandTy tyLeaf = TyNil, tyChild = TyNil, tyRoot = TyNil;
		EnumList::EnumCards const *leaf = NULL, *child = NULL, *root = NULL;

		//至尊青龙/一条龙(十三水)/十二皇族
		specialTy_ = CheckDragonRoyal(src, len);

		//遍历枚举出来的每组牌型(头墩&中墩&尾墩加起来为一组) ////////////////////////////
		for (std::vector<EnumList::TraverseTreeNode>::iterator it = leafList.begin();
			it != leafList.end(); ++it) {
			//从叶子节点往根节点遍历 ////////////////////////////
			EnumList::TraverseTreeNode& leafTraverseNode = *it;
			EnumList*& nodeLeaf = leafTraverseNode.first;//
			cursorLeaf = leafTraverseNode.second;//
			switch (nodeLeaf->dt_)
			{
			case DunLast: {
				groupdun_t group;
				//尾墩
				{
					EnumList::TreeEnumItem& treeItem = nodeLeaf->tree[cursorLeaf];
					EnumList::EnumItem& leafItem = treeItem.first;
					tyRoot = leafItem.first;
					root = leafItem.second;
					group.assign(DunLast, tyRoot, &root->front(), root->size());
				}
				groups.push_back(group);
				break;
			}
			case DunSecond: {
				groupdun_t group;
				//中墩
				{
					EnumList::TreeEnumItem& treeItem = nodeLeaf->tree[cursorLeaf];
					EnumList::EnumItem& leafItem = treeItem.first;
					tyChild = leafItem.first;
					child = leafItem.second;
					group.assign(DunSecond, tyChild, &child->front(), child->size());
				}
				//尾墩
				EnumList *nodeRoot = nodeLeaf->parent_;//
				cursorRoot = nodeLeaf->parentcursor_;//
				{
					assert(nodeRoot != NULL);
					EnumList::TreeEnumItem& treeItem = nodeRoot->tree[cursorRoot];
					EnumList::EnumItem& rootItem = treeItem.first;
					tyRoot = rootItem.first;
					root = rootItem.second;
					group.assign(DunLast, tyRoot, &root->front(), root->size());
				}
				groups.push_back(group);
				break;
			}
			case DunFirst: {
				groupdun_t group;
				//头敦
				{
					EnumList::TreeEnumItem& treeItem = nodeLeaf->tree[cursorLeaf];
					EnumList::EnumItem& leafItem = treeItem.first;
					tyLeaf = leafItem.first;
					leaf = leafItem.second;
					assert(leaf->size() <= 3);
					//group.assign(DunFirst, tyLeaf, &leaf->front(), leaf->size());
				}
				//中墩
				EnumList *nodeChild = nodeLeaf->parent_;//
				cursorChild = nodeLeaf->parentcursor_;//
				{
					assert(nodeChild != NULL);
					EnumList::TreeEnumItem& treeItem = nodeChild->tree[cursorChild];
					EnumList::EnumItem& childItem = treeItem.first;
					tyChild = childItem.first;
					child = childItem.second;
					group.assign(DunSecond, tyChild, &child->front(), child->size());
				}
				//尾墩
				EnumList *nodeRoot = nodeChild->parent_;//
				cursorRoot = nodeChild->parentcursor_;//
				{
					assert(nodeRoot != NULL);
					EnumList::TreeEnumItem& treeItem = nodeRoot->tree[cursorRoot];
					EnumList::EnumItem& rootItem = treeItem.first;
					tyRoot = rootItem.first;
					root = rootItem.second;
					group.assign(DunLast, tyRoot, &root->front(), root->size());
				}
				//如果不是至尊青龙/一条龙(十三水)/十二皇族
				if (specialTy_ != TyZZQDragon && specialTy_ != TyOneDragon && specialTy_ != Ty12Royal) {
					if (tyRoot == Ty123sc && tyChild == Ty123sc && tyLeaf == Ty123sc) {
						//三同花顺
						specialTy_ = TyThree123sc;
						group.specialTy = specialTy_;
					}
					else if (tyRoot == Ty123 && tyChild == Ty123 && tyLeaf == Ty123) {
						//如果不是同花顺
						if (specialTy_ != TyThree123sc) {
							//三顺子
							specialTy_ = TyThree123;
							group.specialTy = specialTy_;
						}
					}
					else if (tyRoot == Tysc && tyChild == Tysc && tyLeaf == Tysc) {
						//如果不是同花顺且不是三顺子
						if (specialTy_ != TyThree123sc && specialTy_ != TyThree123) {
							//三同花
							specialTy_ = TyThreesc;
							group.specialTy = specialTy_;
						}
					}
				}
				//头敦非对子/非三张，整墩非三同花顺/非三顺子/非三同花，则修改头敦为乌龙
				if (tyLeaf != Ty20 && tyLeaf != Ty30 && group.specialTy != TyThree123sc && group.specialTy != TyThree123 && group.specialTy != TyThreesc) {
					tyLeaf = Tysp;
				}
				assert(leaf->size() <= 3);
				group.assign(DunFirst, tyLeaf, &leaf->front(), leaf->size());
				groups.push_back(group);
				break;
			}
			}
		}
		//如果不是至尊青龙/一条龙(十三水)/十二皇族/三同花顺
		if (specialTy_ != TyZZQDragon && specialTy_ != TyOneDragon &&
			specialTy_ != Ty12Royal && specialTy_ != TyThree123sc) {
			HandTy specialTy = TyNil;
			if (classify.c4 == 3) {
				//三分天下(三套炸弹)
				specialTy_ = TyThree40;
			}
			else if ((specialTy = CheckAllBig(src, len)) == TyAllBig) {
				//全大/牌值从小到大
				specialTy_ = specialTy;
			}
			else if ((specialTy = CheckAllSmall(src, len)) == TyAllSmall) {
				//全小/牌值从小到大
				specialTy_ = specialTy;
			}
			else if ((specialTy = CheckAllOneColor(src, len)) == TyAllOneColor) {
				//凑一色：全是红牌(方块/红心)或黑牌(黑桃/梅花)
				specialTy_ = specialTy;
			}
			else if (classify.c3 == 2 && classify.c2 == 3) {
				//双怪冲三
				specialTy_ = TyTwo3220;
			}
			else if (classify.c3 == 4) {
				//四套三条(四套冲三)
				specialTy_ = TyFour30;
			}
			else if (classify.c2 == 5 && classify.c3 == 1) {
				//五对三条(五对冲三)
				specialTy_ = TyFive2030;
			}
			else if (classify.c2 == 6) {
				//六对半
				specialTy_ = TySix20;
			}
		}
	}

	//初始化牌墩
	void CGameLogic::EnumList::Init(DunTy dt) {
		//按同花顺/铁支/葫芦/同花/顺子/三条/两对/对子顺序依次进行
		{
			c = 0;
			//同花顺
			for (std::vector<EnumDunCards>::const_reverse_iterator it = v123sc.rbegin();
				it != v123sc.rend(); ++it) {
				assert(c < MaxEnumSZ);
				tree[c++] = std::make_pair<EnumItem, EnumList*>(std::make_pair(Ty123sc, &*it), NULL);
			}
			//铁支
			for (std::vector<EnumDunCards>::const_reverse_iterator it = v40.rbegin();
				it != v40.rend(); ++it) {
				assert(c < MaxEnumSZ);
				tree[c++] = std::make_pair<EnumItem, EnumList*>(std::make_pair(Ty40, &*it), NULL);
			}
			//葫芦
			for (std::vector<EnumDunCards>::const_reverse_iterator it = v32.rbegin();
				it != v32.rend(); ++it) {
				assert(c < MaxEnumSZ);
				tree[c++] = std::make_pair<EnumItem, EnumList*>(std::make_pair(Ty32, &*it), NULL);
			}
			//同花
			for (std::vector<EnumDunCards>::const_reverse_iterator it = vsc.rbegin();
				it != vsc.rend(); ++it) {
				assert(c < MaxEnumSZ);
				tree[c++] = std::make_pair<EnumItem, EnumList*>(std::make_pair(Tysc, &*it), NULL);
			}
			//顺子
			for (std::vector<EnumDunCards>::const_reverse_iterator it = v123.rbegin();
				it != v123.rend(); ++it) {
				assert(c < MaxEnumSZ);
				tree[c++] = std::make_pair<EnumItem, EnumList*>(std::make_pair(Ty123, &*it), NULL);
			}
			//三条
			for (std::vector<EnumDunCards>::const_reverse_iterator it = v30.rbegin();
				it != v30.rend(); ++it) {
				assert(c < MaxEnumSZ);
				tree[c++] = std::make_pair<EnumItem, EnumList*>(std::make_pair(Ty30, &*it), NULL);
			}
			//两对
			for (std::vector<EnumDunCards>::const_reverse_iterator it = v22.rbegin();
				it != v22.rend(); ++it) {
				assert(c < MaxEnumSZ);
				tree[c++] = std::make_pair<EnumItem, EnumList*>(std::make_pair(Ty22, &*it), NULL);
			}
			//对子
			for (std::vector<EnumDunCards>::const_reverse_iterator it = v20.rbegin();
				it != v20.rend(); ++it) {
				assert(c < MaxEnumSZ);
				tree[c++] = std::make_pair<EnumItem, EnumList*>(std::make_pair(Ty20, &*it), NULL);
			}
			//printf("-- *** tree.size = %d\n", c);
		}
		{
			dt_ = dt;
			//初始化游标
			cursor_ = -1;
		}
	}
	
	void CGameLogic::EnumList::Reset() {
		//按同花顺/铁支/葫芦/同花/顺子/三条/两对/对子顺序依次进行
		v123sc.clear();
		v40.clear();
		v32.clear();
		vsc.clear();
		v123.clear();
		v30.clear();
		v22.clear();
		v20.clear();
		c = 0;
		cursor_ = -1;
	}

	//重置游标
	void CGameLogic::EnumList::ResetCursor() {
		cursor_ = -1;
	}

	//返回下一个游标
	bool CGameLogic::EnumList::GetNextCursor(int& cursor) {
		if (++cursor_ < c) {
			cursor = cursor_;
			return true;
		}
		return false;
	}

	//释放new内存
	void CGameLogic::EnumList::Release() {
		
		//子节点/根节点游标
		int cursorChild = 0, cursorRoot = 0;
		EnumList* const& rootEnumList = this;
		
		rootEnumList->ResetCursor();
		while (true) {
			//返回下一个游标
			if (!rootEnumList->GetNextCursor(cursorRoot)) {
				break;
			}
			//子节点
			EnumList*& childEnumList = rootEnumList->GetCursorChildItem(cursorRoot);
			if (!childEnumList) {
				break;
			}
			childEnumList->ResetCursor();
			while (true) {
				//返回下一个游标
				if (!childEnumList->GetNextCursor(cursorChild)) {
					break;
				}

				//叶子节点
				EnumList*& leafEnumList = childEnumList->GetCursorChildItem(cursorChild);
				if (!leafEnumList) {
					break;
				}
				delete leafEnumList;//删除叶子节点
				leafEnumList = NULL;
				//printf("--- *** 删除叶子节点\n");
			}
			//删除子节点
			delete childEnumList;
			childEnumList = NULL;
			//printf("--- *** 删除子节点\n");
		}
	}

	//打印枚举牌型
	void CGameLogic::EnumList::PrintEnumCards(HandTy ty) {
		switch (ty)
		{
		case Tysp:		PrintEnumCards("乌龙", ty, *wl);		break;//乌龙
		case Ty20:		PrintEnumCards("对子", ty, v20);		break;//对子
		case Ty22:		PrintEnumCards("两对", ty, v22);		break;//两对
		case Ty30:		PrintEnumCards("三条", ty, v30);		break;//三条
		case Ty123:		PrintEnumCards("顺子", ty, v123);		break;//顺子
		case Tysc:		PrintEnumCards("同花", ty, vsc);		break;//同花
		case Ty32:		PrintEnumCards("葫芦", ty, v32);		break;//葫芦
		case Ty40:		PrintEnumCards("铁支", ty, v40);		break;//铁支
		case Ty123sc:	PrintEnumCards("同花顺", ty, v123sc);	break;//同花顺
		case TyAllBase:
		default: {
			for (int i = Ty123sc; i >= Ty20; --i) {
				PrintEnumCards((HandTy)(i));
			}
			break;
		}
		}
	}

	//打印枚举牌型
	void CGameLogic::EnumList::PrintEnumCards(std::string const& name, HandTy ty, std::vector<std::vector<uint8_t>> const& src) {
		if (src.size() > 0) {
			printf("--- *** 第[%d]墩 - %s[%s]\n", dt_ + 1, name.c_str(), StringCardType(ty).c_str());
			for (std::vector<std::vector<uint8_t>>::const_reverse_iterator it = src.rbegin();
				it != src.rend(); ++it) {
				PrintCardList(&it->front(), it->size());
			}
		}
	}

	//打印游标处枚举牌型
	void CGameLogic::EnumList::PrintCursorEnumCards() {
		EnumList::EnumItem const* item = GetCursorItem(cursor_);
		if (item) {
			switch (item->first)
			{
			case Tysp:break;
			case Ty20:		PrintCursorEnumCards("对子", item->first, *item->second);	break;//对子
			case Ty22:		PrintCursorEnumCards("两对", item->first, *item->second);	break;//两对
			case Ty30:		PrintCursorEnumCards("三条", item->first, *item->second);	break;//三条
			case Ty123:		PrintCursorEnumCards("顺子", item->first, *item->second);	break;//顺子
			case Tysc:		PrintCursorEnumCards("同花", item->first, *item->second);	break;//同花
			case Ty32:		PrintCursorEnumCards("葫芦", item->first, *item->second);	break;//葫芦
			case Ty40:		PrintCursorEnumCards("铁支", item->first, *item->second);	break;//铁支
			case Ty123sc:	PrintCursorEnumCards("同花顺", item->first, *item->second);	break;//同花顺
			}
		}
	}
	
	//打印游标处枚举牌型
	void CGameLogic::EnumList::PrintCursorEnumCards(std::string const& name, HandTy ty, std::vector<uint8_t> const& src) {
		printf("--- *** 第[%d]墩 - %s[%s]：", dt_ + 1, name.c_str(), StringCardType(ty).c_str());
		PrintCardList(&src[0], src.size());
	}
	
	//返回游标处枚举牌型
	CGameLogic::EnumList::EnumItem const* CGameLogic::EnumList::GetCursorItem(int cursor) {
		return cursor < c ? &tree[cursor].first : NULL;
	}
	
	//返回游标处枚举牌型对应余牌枚举子项列表指针
	CGameLogic::EnumList*& CGameLogic::EnumList::GetCursorChildItem(int cursor)/* __attribute__((noreturn))*/ {
		assert(cursor < c);
		return tree[cursor].second;
	}

	//返回下一个枚举牌型(从大到小返回)
	bool CGameLogic::EnumList::GetNextEnumItem(uint8_t const* src, int len,
		CGameLogic::EnumList::EnumCards const*& dst, HandTy& ty,
		int& cursor, uint8_t *cpy, int& cpylen) {
		ty = TyNil;
		cpylen = 0;
		if (++cursor_ < c) {
			ty = tree[cursor_].first.first;
			dst = tree[cursor_].first.second;
			cursor = cursor_;
			for (int i = 0; i < len; ++i) {
				EnumCards::const_iterator it;
				for (it = dst->begin(); it != dst->end(); ++it) {
					if (src[i] == *it) {
						//src[i]在dst中存在了
						break;
					}
				}
				//不存在的话添加到余牌中
				if (it == dst->end()) {
					cpy[cpylen++] = src[i];
				}
			}
			return true;
		}
		return false;
	}

	//打印指定墩牌型
	void CGameLogic::groupdun_t::PrintCardList(DunTy dt) {
		assert(dt < DunMax);
		switch (ty_[dt])
		{
		case Tysp:		PrintCardList("乌龙", dt, ty_[dt]);	break;//乌龙
		case Ty20:		PrintCardList("对子", dt, ty_[dt]);	break;//对子
		case Ty22:		PrintCardList("两对", dt, ty_[dt]);	break;//两对
		case Ty30:		PrintCardList("三条", dt, ty_[dt]);	break;//三条
		case Ty123:		PrintCardList("顺子", dt, ty_[dt]);	break;//顺子
		case Tysc:		PrintCardList("同花", dt, ty_[dt]);	break;//同花
		case Ty32:		PrintCardList("葫芦", dt, ty_[dt]);	break;//葫芦
		case Ty40:		PrintCardList("铁支", dt, ty_[dt]);	break;//铁支
		case Ty123sc:	PrintCardList("同花顺", dt, ty_[dt]); break;//同花顺
		}
	}

	//打印指定墩牌型
	void CGameLogic::groupdun_t::PrintCardList(std::string const& name, DunTy dt, HandTy ty) {
		printf("--- *** 第[%d]墩 - %s[%s]：", dt + 1, name.c_str(), StringCardType(ty).c_str());
		CGameLogic::PrintCardList(&(dun[dt])[0], 5, true);
	}

	//初始化
	void CGameLogic::handinfo_t::Init() {
		if (rootEnumList == NULL) {
			//必须用成员结构体指针形式来new结构体成员对象，否则类成员变量数据会错乱，
			//只要类成员结构体嵌入vector/string这些STL对象会出问题，编译器bug???
			rootEnumList = new EnumList();
		}
	}
	
	void CGameLogic::handinfo_t::Reset() {
		chairID = -1;
		specialTy_ = TyNil;
		current = 0;
		groups.clear();
		leafList.clear();
		if (rootEnumList) {
			rootEnumList->Release();
			rootEnumList->Reset();
		}
		memset(&classify, 0, sizeof(classify_t));
	}
	
	//打印全部枚举墩牌型
	void CGameLogic::handinfo_t::PrintEnumCards(bool ascend) {
		if(ascend) {
			int i = groups.size();
			//倒序从小到大输出
			for (std::vector<groupdun_t>::reverse_iterator it = groups.rbegin();
				it != groups.rend(); ++it) {
				printf("\n--- *** --------------------------------------------------\n");
				if (it->specialTy != TyNil) {
					printf("--- *** 第[%d]组 %s\n", i--, StringSpecialTy(it->specialTy).c_str());
				}
				else {
					printf("--- *** 第[%d]组\n", i--);
				}
				it->PrintCardList(DunFirst);
				it->PrintCardList(DunSecond);
				it->PrintCardList(DunLast);
				printf("--- *** --------------------------------------------------\n\n");
			}
		}
		else {
			int i = 0;
			for (std::vector<groupdun_t>::iterator it = groups.begin();
				it != groups.end(); ++it) {
				printf("\n--- *** --------------------------------------------------\n");
				if (it->specialTy != TyNil) {
					printf("--- *** 第[%d]组 %s\n", i++ + 1, StringSpecialTy(it->specialTy).c_str());
				}
				else {
					printf("--- *** 第[%d]组\n", i++ + 1);
				}
				it->PrintCardList(DunFirst);
				it->PrintCardList(DunSecond);
				it->PrintCardList(DunLast);
				printf("--- *** --------------------------------------------------\n\n");
			}
		}
	}
	
	static std::string CNStringTy(char const* name, char const* ty) {
		char ch[50] = { 0 };
		sprintf(ch, "%s[%s]", name, ty);
		return ch;
	}
	
	//返回特殊牌型字符串
	std::string CGameLogic::handinfo_t::StringSpecialTy() {
		return StringSpecialTy(specialTy_);
	}

	//返回特殊牌型字符串
	std::string CGameLogic::handinfo_t::StringSpecialTy(HandTy specialTy) {
		switch (specialTy)
		{
		case TyThreesc:		return CNStringTy("三同花", "TyThreesc");
		case TyThree123:	return CNStringTy("三顺子", "TyThree123");
		case TySix20:		return CNStringTy("六对半", "TySix20");
		case TyFive2030:	return CNStringTy("五对三条", "TyFive2030");
		case TyFour30:		return CNStringTy("四套三条", "TyFour30");
		case TyTwo3220:		return CNStringTy("双怪冲三", "TyTwo3220");
		case TyAllOneColor:	return CNStringTy("凑一色", "TyAllOneColor");
		case TyAllSmall:	return CNStringTy("全小", "TyAllSmall");
		case TyAllBig:		return CNStringTy("全大", "TyAllBig");
		case TyThree40:		return CNStringTy("三分天下", "TyThree40");
		case TyThree123sc:	return CNStringTy("三同花顺", "TyThree123sc");
		case Ty12Royal:		return CNStringTy("十二皇族", "Ty12Royal");
		case TyOneDragon:	return CNStringTy("一条龙", "TyOneDragon");
		case TyZZQDragon:	return CNStringTy("至尊青龙", "TyZZQDragon");
		}
		return "";
	}

	//按照尾墩5张/中墩5张/头墩3张依次抽取枚举普通牌型
	//src uint8_t const* 手牌余牌(13/8/3)，初始13张，按5/5/3依次抽，余牌依次为13/8/3
	//n int 抽取n张(5/5/3) 第一次抽5张余8张，第二次抽5张余3张，第三次取余下3张抽完
	//classify classify_t& 存放分类信息(所有重复四张/三张/二张/散牌/余牌)
	//enumList EnumList& 存放枚举墩牌型列表数据 dt DunTy 指定为第几墩
	void CGameLogic::EnumCards(uint8_t const* src, int len,
		int n, classify_t& classify, EnumList& enumList, DunTy dt) {
		//printf("\n\n--- *** EnumCards(%d, %d) from ", len, n);
		//PrintCardList(src, len);
		//int c4 = 0, c3 = 0, c2 = 0;
		//所有重复四张牌型
		//static int const size4 = 3;
		//uint8_t dst4[size4][4] = { 0 };
		//所有重复三张牌型
		//static int const size3 = 4;
		//uint8_t dst3[size3][4] = { 0 };
		//所有重复二张牌型
		//static int const size2 = 6;
		//uint8_t dst2[size2][4] = { 0 };
		//去重后的余牌/散牌
		//uint8_t cpy[MaxSZ] = { 0 };
		//int cpylen = 0;
		//memset(&classify, 0, sizeof(classify_t));
		enumList.Reset();
		//枚举墩牌型
		EnumCards(src, len, n, 
			classify.dst4, classify.c4, classify.dst3, classify.c3, classify.dst2, classify.c2, classify.cpy, classify.cpylen,
			enumList.v123sc, enumList.v123, enumList.vsc, enumList.v40, enumList.v32, enumList.v30, enumList.v22, enumList.v20);
		//初始化牌墩
		enumList.Init(dt);
	}
	
	//枚举牌型测试
	void CGameLogic::TestEnumCards() {
		CGameLogic g;
		handinfo_t hand;
		//初始化
		g.InitCards();
		//洗牌
		g.ShuffleCards();
		bool pause = false;
		while (1) {
			if (pause) {
				getchar();
			}
			//余牌不够则重新洗牌
			if (g.Remaining() < 13) {
				g.ShuffleCards();
			}
			uint8_t cards[MaxCount] = { 0 };
			//发牌
			g.DealCards(MaxCount, cards);
			//手牌排序
			CGameLogic::SortCards(cards, MaxCount, true, true, true);
			printf("=================================================\n\n");
			//一副手牌
			CGameLogic::PrintCardList(cards, MaxCount);
 			//手牌牌型分析
			int c = g.AnalyseHandCards(cards, MaxCount, 20, hand);
			//查看枚举葫芦牌型
			//hand.rootEnumList->PrintEnumCards(Ty32);
			//查看手牌枚举三墩牌型
			hand.PrintEnumCards();
			//查看重复牌型和散牌
			hand.classify.PrintCardList();
			printf("--- *** c = %d %s\n\n\n\n", c, hand.StringSpecialTy().c_str());
			//有特殊牌型时暂停
			pause = (hand.specialTy_ != SSS::TyNil);
			//没有重复四张，有2个重复三张和3个重复二张
// 			pause = (hand.classify.c4 == 0 &&
// 				hand.classify.c3 >= 2 &&
// 				hand.classify.c2 >= 3);
		}
	}

	//枚举牌型测试
	//filename char const* 文件读取手牌 cardsList.ini
	void CGameLogic::TestEnumCards(char const* filename) {
		std::vector<string> lines;
		readFile(filename, lines, ";;");
		assert(lines.size() == 2);
		int flag = atoi(lines[0].c_str());
		//1->文件读取手牌 0->随机生成13张牌
		if (flag > 0) {
			CGameLogic g;
			handinfo_t hand;
			uint8_t cards[MaxCount + 10] = { 0 };
			int n = CGameLogic::MakeCardList(lines[1], cards, MaxCount);
			//assert(n == 13);
			//手牌排序
			CGameLogic::SortCards(cards, n, true, true, true);
			//一副手牌
			CGameLogic::PrintCardList(cards, n);
			//手牌牌型分析
			int c = g.AnalyseHandCards(cards, n, 2000, hand);
			//查看手牌枚举墩牌型
			hand.PrintEnumCards();
			//查看重复牌型和散牌
			hand.classify.PrintCardList();
			printf("--- *** c = %d %s\n\n\n\n", c, hand.StringSpecialTy().c_str());
		}
		else {
			TestEnumCards();
		}
	}
	
	//src与dst牌型相同的情况下比大小，且牌数必须相同
	int CGameLogic::CompareCards(uint8_t const* src, uint8_t const* dst, int n, HandTy ty) {
		switch (ty) {
		case Ty32: {
			//葫芦牌型比较三张的大小(中间的牌)
			return GetCardPoint(src[2]) > GetCardPoint(dst[2]);
		}
		default: {
			uint8_t psrc[MaxSZ] = { 0 }, pdst[MaxSZ] = { 0 };
			memcpy(psrc, src, n);
			memcpy(pdst, dst, n);
			CGameLogic::SortCards(psrc, n, false, true, true);
			CGameLogic::SortCards(pdst, n, false, true, true);
			//牌型相同按顺序比点，先将src/dst按照牌点升序排
			for (int i = n - 1; i >= 0; --i) {
				uint8_t p0 = GetCardPoint(psrc[i]);
				uint8_t p1 = GetCardPoint(pdst[i]);
				if (p0 != p1) {
					return p0 - p1;
				}
			}
			//点数相同按顺序比花色
			//for (int i = n - 1; i >= 0; --i) {
			//	uint8_t c0 = GetCardColor(src[i]);
			//	uint8_t c1 = GetCardColor(dst[i]);
			//	if (c0 != c1) {
			//		return c0 - c1;
			//	}
			//}
			break;
		}
		}		
		return 0;
	}

	//玩家手牌类型
	HandTy CGameLogic::GetHandCardsType(handinfo_t& hand, DunTy dt)
	{
		if (hand.specialTy_ != TyNil) {
			return hand.specialTy_;
		}
		//EnumList& dun = hand.dun[dt];

	}

#if 0
	//从src中抽取连续n张牌到dst中
	//src uint8_t* 牌源
	//ty FetchTy 抽取类型
	//n int 抽取n张，dst长度
	//dst uint8_t* 符合要求的单个目标牌型
	//cpy uint8_t* 抽取后不符合要求的余牌
	bool CGameLogic::FetchCardsFrom(uint8_t *src, int len, FetchTy Ty, uint8_t *dst, int n, uint8_t *cpy, int& cpylen)
	{
		int i = 0, j = 0, k = 0;
		bool succ = false;
		cpylen = 0;
		switch (Ty)
		{
			//同花色非顺子
		case SameColor:
		{
			if (len <= 0)
				break;
			int s = i++, c = 0;
			uint8_t c_src_pre = GetCardColor(src[s]);
			for (; i < len; ++i) {
				//src中当前的花色
				uint8_t c_src_cur = GetCardColor(src[i]);
				//两张花色相同的牌
				if (c_src_pre == c_src_cur) {
					//收集到n张牌后返回
					if (++c + 1 == n) {
						succ = true;
						break;
					}
				}
				else {
					//缓存不合要求的副本
					memcpy(&cpy[k], &src[s], i - s);
					k += (i - s);
					//赋值新的
					c = 0;
					s = i;
					c_src_pre = GetCardColor(src[s]);
				}
			}
			if (succ) {
				//缓存符合要求的牌型
				assert(n == i - s + 1);
				memcpy(dst, &src[s], n);
				//缓存不合要求的副本
				if (len - (i + 1) > 0) {
					memcpy(&cpy[k], &src[i + 1], len - (i + 1));
					cpylen = k;
					cpylen += len - (i + 1);
				}
				else {
					cpylen = k;
				}
			}
			break;
		}
		//顺子非同花色
		case Consecutive:
		{
			if (len <= 0)
				break;
			dst[j++] = src[i++];
			for (; i < len;) {
				//dst中之前的牌值
				uint8_t v_dst_pre = GetCardValue(dst[j - 1]);
				//src中当前的牌值
				uint8_t v_src_cur = GetCardValue(src[i]);
				//dst中之前的花色
				//uint8_t c_dst_pre = GetCardColor(dst[j - 1]);
				//src中当前的花色
				//uint8_t c_src_cur = GetCardColor(src[i]);
				//dst[j - 1]与src[i]为连续牌♠A [♣2 ♠2]
				if (v_dst_pre + 1 == v_src_cur) {
					//同花色
					//if (c_dst_pre == c_src_cur) {
					dst[j++] = src[i++];
					//收集到n张牌后返回
					if (j == n) {
						succ = true;
						break;
					}
					continue;
					//}
					//花色不同♠A [♣2 ♠2]
					//cpy[k++] = src[i++];
					//continue;
				}
				else if (v_dst_pre == v_src_cur) {
					//连续两张相同牌[♠A ♣2] ♠2
					cpy[k++] = src[i++];
					continue;
				}
				else /*if (v_dst_pre + 1 < v_src_cur)*/ {
					//不连续了
					cpy[k++] = src[i++];
					break;
				}
			}
			//收集到n张牌后拷贝剩余的牌
			if (j == n && i < len) {
				memcpy(&cpy[k], &src[i], len - i);
				k += (len - i);
				cpylen = k;
			}
			break;
		}
		//同花色顺子
		case SameColorConsecutive:
		{
			if (len <= 0)
				break;
			dst[j++] = src[i++];
			for (; i < len;) {
				//dst中之前的牌值
				uint8_t v_dst_pre = GetCardValue(dst[j - 1]);
				//src中当前的牌值
				uint8_t v_src_cur = GetCardValue(src[i]);
				//dst中之前的花色
				uint8_t c_dst_pre = GetCardColor(dst[j - 1]);
				//src中当前的花色
				uint8_t c_src_cur = GetCardColor(src[i]);
				//dst[j - 1]与src[i]为连续牌♠A [♣2 ♠2]
				if (v_dst_pre + 1 == v_src_cur) {
					//同花色
					if (c_dst_pre == c_src_cur) {
						dst[j++] = src[i++];
						//收集到n张牌后返回
						if (j == n) {
							succ = true;
							break;
						}
						continue;
					}
					//花色不同♠A [♣2 ♠2]
					cpy[k++] = src[i++];
					continue;
				}
				else if (v_dst_pre == v_src_cur) {
					//连续两张相同牌[♠A ♣2] ♠2
					cpy[k++] = src[i++];
					continue;
				}
				else /*if (v_dst_pre + 1 < v_src_cur)*/ {
					//不连续了
					cpy[k++] = src[i++];
					break;
				}
			}
			//收集到n张牌后拷贝剩余的牌
			if (j == n && i < len) {
				memcpy(&cpy[k], &src[i], len - i);
				k += (len - i);
				cpylen = k;
			}
			break;
		}
		}//{{switch}}
		return succ;
	}
#endif

};

//////////////////////////////////////////////////////////////////////////////////
