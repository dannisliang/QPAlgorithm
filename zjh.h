//
// Created by andy_ro@qq.com
// 			5/26/2019
//
#ifndef GAME_LOGIC_ZJH_H
#define GAME_LOGIC_ZJH_H

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

#define MAX_CARD_TOTAL	ZJH::MaxCardTotal	//牌总个数
#define GAME_PLAYER		ZJH::MaxPlayer		//最多5人局
#define MAX_COUNT		ZJH::MaxCount		//每人3张牌
#define MAX_ROUND       ZJH::MaxRound		//最大局数

//扎金花
namespace ZJH {
	
	//大小：豹子>同花顺>金花>顺子>对子>散牌；特殊235>豹子(如AAA，KKK)
	//牌型：相同牌型按顺序比点，点数相同则按顺序比花色
	//牌值：A<2<3<4<5<6<7<8<9<10<J<Q<K
	//点数：2<3<4<5<6<7<8<9<10<J<Q<K<A
	//花色：黑>红>梅>方
	//顺子：AKQ>KQJ......432>32A，注：KA2非顺子
	//对子：先比对牌点数，对牌点数相同，比单牌点数，单牌点数相同，比对牌里面最大的花色
	
	const int MaxCardTotal	= 52;	//牌总个数
	const int MaxPlayer		= 5;	//最多5人局
	const int MaxCount		= 3;	//每人3张牌
	const int MaxRound		= 1;	//最大局数
	
	//手牌类型：从小到大
	//散牌<对子<顺子<金花<同花顺<豹子；AAA<特殊235
	enum HandTy {
		SanPai,		//散牌(高牌/单张)：三张牌不组成任何类型的牌(AKJ最大，235最小)
		DuiZi,		//对子：二张值相同的牌(AAK最大，223最小)
		ShunZi,		//顺子：花色不同的顺子(QKA最大，A23最小)
		JinHua,		//金花(同花)：花色相同，非顺子(AKJ最大，235最小)
		ShunJin,	//顺金(同花顺)：花色相同的顺子(QKA最大，A23最小)
		BaoZi,		//豹子(炸弹)：三张值相同的牌(AAA最大，222最小)
		TeShu235,	//特殊：散牌中的235
	};

	//花色：黑>红>梅>方
	enum CardColor {
		Diamond = 0x00,	//方块(♦)
		Club	= 0x10,	//梅花(♣)
		Heart	= 0x20,	//红心(♥)
		Spade	= 0x30,	//黑桃(♠)
	};
	
	//牌值：A<2<3<4<5<6<7<8<9<10<J<Q<K
	enum CardValue {
		A	= 0x01,
		T   = 0x0A,
		J	= 0x0B,
		Q	= 0x0C,
		K	= 0x0D,
	};

	//游戏逻辑类
	class CGameLogic
	{
	public:
		CGameLogic();
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
		//花色：黑>红>梅>方
		static uint8_t GetCardColor(uint8_t card);
		//牌值：A<2<3<4<5<6<7<8<9<10<J<Q<K
		static uint8_t GetCardValue(uint8_t card);
		//点数：2<3<4<5<6<7<8<9<10<J<Q<K<A
		static uint8_t GetCardPoint(uint8_t card);
		//花色和牌值构造单牌
		static uint8_t MakeCardWith(uint8_t color, uint8_t value);
		//手牌排序(默认按牌点降序排列)，先比牌值/点数，再比花色
		//byValue bool false->按牌点 true->按牌值
		//ascend bool false->降序排列(即从大到小排列) true->升序排列(即从小到大排列)
		//clrAscend bool false->花色降序(即黑桃到方块) true->花色升序(即从方块到黑桃)
		static void SortCards(uint8_t *cards, int n, bool byValue, bool ascend, bool clrAscend);
		//手牌排序(默认按牌点降序排列)，先比花色，再比牌值/点数
		//clrAscend bool false->花色降序(即黑桃到方块) true->花色升序(即从方块到黑桃)
		//byValue bool false->按牌点 true->按牌值
		//ascend bool false->降序排列(即从大到小排列) true->升序排列(即从小到大排列)
		static void SortCardsColor(uint8_t *cards, int n, bool clrAscend, bool byValue, bool ascend);
		//牌值字符串 
		static std::string StringCardValue(uint8_t value);
		//花色字符串 
		static std::string StringCardColor(uint8_t color);
		//单牌字符串
		static std::string StringCard(uint8_t card);
	private:
		//比较散牌大小
		static int CompareSanPai(uint8_t *cards1, uint8_t *cards2);
		//比较对子大小
		static int CompareDuiZi(uint8_t *cards1, uint8_t *cards2);
		//比较顺子大小
		static int CompareShunZi(uint8_t *cards1, uint8_t *cards2);
		//比较金花大小
		static int CompareJinHua(uint8_t *cards1, uint8_t *cards2);
		//比较顺金大小
		static int CompareShunJin(uint8_t *cards1, uint8_t *cards2);
		//比较豹子大小
		static int CompareBaoZi(uint8_t *cards1, uint8_t *cards2);
	public:
		//玩家手牌类型
		static HandTy GetHandCardsType(uint8_t *cards);
		//牌型字符串
		static std::string StringHandType(HandTy ty);
		//比较手牌大小 >0-cards1大 <0-cards2大
		static int CompareHandCards(uint8_t *cards1, uint8_t *cards2);
		//比较手牌大小 >0-cards1大 <0-cards2大
		static bool GreaterHandCards(boost::shared_ptr<uint8_t>& cards1, boost::shared_ptr<uint8_t>& cards2);
	private:
		int8_t index_;
		uint8_t cardsData_[MaxCardTotal];
	};
};

#endif