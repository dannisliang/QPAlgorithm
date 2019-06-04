//
// Created by andy_ro@qq.com
// 			5/26/2019
//
#ifndef GAME_LOGIC_S13S_H
#define GAME_LOGIC_S13S_H

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

#define MAX_CARD_TOTAL	S13S::MaxCardTotal	//牌总个数
#define GAME_PLAYER		S13S::MaxPlayer		//最多4人局
#define MAX_COUNT		S13S::MaxCount		//每人13张牌
#define MAX_ROUND       S13S::MaxRound		//最大局数

//十三水
namespace S13S {
	const int MaxCardTotal = 52;	//牌总个数
	const int MaxPlayer = 4;		//最多4人局
	const int MaxCount = 13;		//每人13张牌
	const int MaxRound = 1;			//最大局数

	//牌值：A<2<3<4<5<6<7<8<9<10<J<Q<K
	//点数：2<3<4<5<6<7<8<9<10<J<Q<K<A
	//花色：黑>红>梅>方
	//
	//普通牌型
	//同花顺>铁支>葫芦>同花>顺子>三条>两对>对子>乌龙
	//
	//特殊牌型
	//至尊青龙>一条龙>十二皇族>三同花顺>三分天下>全大>全小>凑一色>双怪冲三>四套三条>五对三条>六对半>三顺子>三同花
	//至尊青龙>一条龙>十二皇族>三同花顺>三套炸弹>全大>全小>凑一色>       >四套冲三>五对冲三>六对半>三同花>三顺子
	//
	//手牌类型：从小到大
	enum HandTy {
		////// 普通牌型
		TyNil,
		TyAllBase,		//所有普通牌型
		Tysp,			//散牌(乌龙)：一墩牌不组成任何牌型
		Ty20,			//对子(一对)：除了两张值相同的牌外没有其它牌型
		Ty22,			//两对：两个对子加上一张单牌
		Ty30,			//三条：除了三张值相同的牌外没有其它牌型
		Ty123,			//顺子：花色不同的连续五张牌(A2345仅小于10JQKA)
		Tysc,			//同花：花色相同的五张牌，非顺子
		Ty32,			//葫芦：一组三条加上一组对子
		Ty40,			//铁支：除了四张值相同的牌外没有其它牌型
		Ty123sc,		//同花顺：花色相同的连续五张牌(A2345仅小于10JQKA)
		////// 特殊牌型
		TyThreesc,		//三同花：三墩均是同一花色的牌型
		TyThree123,		//三顺子：三墩均是顺子的牌型
		TySix20,		//六对半：六个对子加上一张单张的牌型
		TyFive2030,		//五对三条(五对冲三)：五个对子加上一个三条
		TyFour30,		//四套三条(四套冲三)：四个三条加上一张单张的牌型
		TyTwo3220,		//双怪冲三：二对葫芦加上一个对子加上一张单张的牌型
		TyAllOneColor,	//凑一色：全是红牌(方块/红心)或黑牌(黑桃/梅花)的牌型
		TyAllSmall,		//全小：全是2至8的牌型
		TyAllBig,		//全大：全是8至A的牌型
		TyThree40,		//三分天下(三套炸弹)：三副炸弹(四张值相同)加上一张单张的牌型
		TyThree123sc,	//三同花顺：三墩均是同花顺的牌型
		Ty12Royal,		//十二皇族：十三张全是J，Q，K，A的牌型
		TyOneDragon,	//一条龙(十三水)：A到K的牌型，非同花，A2345678910JQK
		TyZZQDragon,	//至尊青龙：同花A到K的牌型，A2345678910JQK
	};

	//花色：黑>红>梅>方
	enum CardColor {
		Diamond = 0x10,	//方块(♦)
		Club	= 0x20,	//梅花(♣)
		Heart	= 0x30,	//红心(♥)
		Spade	= 0x40,	//黑桃(♠)
	};

	//牌值：A<2<3<4<5<6<7<8<9<10<J<Q<K
	enum CardValue {
		A = 0x01,
		T = 0x0A,
		J = 0x0B,
		Q = 0x0C,
		K = 0x0D,
	};
	
	//抽取类型
	enum FetchTy {
		SameColor   = 0x01,								//同花非顺子
		Consecutive = 0x02,								//顺子非同花
		SameValue	= 0x04,								//同牌值
		SameColorConsecutive = SameColor & Consecutive,	//同花顺
	};

	//前/中/尾墩
	enum DunTy {
		DunNil = -1,
		DunFirst = 0,
		DunSecond,
		DunLast,
		DunMax,
	};

	//手牌占位条码mark大小 14
	//牌值对应占位->A,2,,,K,0 牌点对应占位->0,2,,,K,A
	int const MaxSZ = K + 1;
	int const MaxEnumSZ = 1500;

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
		//牌型字符串
		static std::string StringHandTy(HandTy ty);
		//打印n张牌
		static void PrintCardList(uint8_t const* cards, int n, bool hide = true);
		//获取牌有效列数
		//cards uint8_t const* 相同牌值n张牌(n<=4)
		//n uint8_t 黑/红/梅/方4张牌
		static uint8_t get_card_c(uint8_t const* cards, int n);
		//返回指定花色牌列号
		//cards uint8_t const* 相同牌值n张牌(n<=4)
		//n uint8_t 黑/红/梅/方4张牌
		//clr CardColor 指定花色
		static uint8_t get_card_colorcol(uint8_t const* cards, int n, CardColor clr);
	private:
		//拆分字符串"♦A ♦3 ♥3 ♥4 ♦5 ♣5 ♥5 ♥6 ♣7 ♥7 ♣9 ♣10 ♣J"
		static void CardsBy(std::string const& strcards, std::vector<std::string>& vec);
		//字串构造牌"♦A"->0x01
		static uint8_t MakeCardBy(std::string const& name);
		//生成n张牌<-"♦A ♦3 ♥3 ♥4 ♦5 ♣5 ♥5 ♥6 ♣7 ♥7 ♣9 ♣10 ♣J"
		static void MakeCardList(std::vector<std::string> const& vec, uint8_t *cards, int size);
	public:
		//生成n张牌<-"♦A ♦3 ♥3 ♥4 ♦5 ♣5 ♥5 ♥6 ♣7 ♥7 ♣9 ♣10 ♣J"
		static int MakeCardList(std::string const& strcards, uint8_t *cards, int size);
	private:
		int8_t index_;
		uint8_t cardsData_[MaxCardTotal];
	public:
		//////////////////////////////////////////////////////////////
		//EnumTree 枚举一墩牌型的所有可能，多叉树结构
		class EnumTree {
		public:
			//枚举牌，枚举一墩牌，5/3张
			typedef std::vector<uint8_t>                CardData;
			//枚举项，pair<牌型，一墩牌>
			typedef std::pair<HandTy, CardData const*>  EnumItem;
			//树节点，pair<枚举项，子枚举项列表>
			typedef std::pair<EnumItem, EnumTree*>      TreeNode;
			//树节点，pair<树节点指针，对应树枚举项>
			typedef std::pair<EnumTree*, int>   TraverseTreeNode;
		public:
			EnumTree() {
				Reset();
				parent_ = NULL;
				parentcursor_ = -1;
			}
			~EnumTree() {
				Reset();
			}
			//初始化牌墩
			void Init(DunTy dt);
			void Reset();
			//释放new内存
			void Release();
			//重置游标
			void ResetCursor();
			//返回下一个游标
			bool GetNextCursor(int& cursor);
			//打印指定枚举牌型
			void PrintEnumCards(bool reverse/* = false*/, HandTy ty/* = TyAllBase*/);
			//打印指定枚举牌型
			void PrintEnumCards(std::string const& name, HandTy ty, std::vector<std::vector<uint8_t>> const& src, bool reverse);
			//打印游标处枚举牌型
			void PrintCursorEnumCards();
			//打印游标处枚举牌型
			void PrintCursorEnumCards(std::string const& name, HandTy ty, std::vector<uint8_t> const& src);
			//返回游标处枚举牌型
			EnumItem const* GetCursorItem(int cursor);
			//返回游标处枚举牌型对应余牌枚举子项列表指针
			EnumTree*& GetCursorChild(int cursor);
			//返回下一个枚举牌型(从大到小返回)
			bool GetNextEnumItem(uint8_t const* src, int len,
				CardData const*& dst, HandTy& ty,
				int& cursor, uint8_t *cpy, int& cpylen);
		public:
			//所有同花色五张/三张连续牌(五张/三张同花顺)
			std::vector<CardData> v123sc;
			//所有铁支(四张)
			std::vector<CardData> v40;
			//所有葫芦(一组三条加上一组对子)
			std::vector<CardData> v32;
			//所有同花五张/三张非连续牌(五张/三张同花)
			std::vector<CardData> vsc;
			//所有非同花五张/三张连续牌(五张/三张顺子)
			std::vector<CardData> v123;
			//所有三条(三张)
			std::vector<CardData> v30;
			//所有两对(两个对子)
			std::vector<CardData> v22;
			//所有对子(一对)
			std::vector<CardData> v20;
			//散牌/乌龙(头敦中指向Ty123sc/Ty123/Tysc中的一个)
			std::vector<CardData> const* vsp;
		public:
			//标识头/中/尾墩
			DunTy dt_;
			//遍历游标
			int c, cursor_;
			//父亲节点
			EnumTree* parent_;
			//对应父节点游标位置
			int parentcursor_;
			//pair<枚举项牌型，对应余牌枚举子项列表>，多叉树结构
			TreeNode tree[MaxEnumSZ];
		};
	public:
		//////////////////////////////////////////////////////////////
		//classify_t 分类牌型
		struct classify_t {
			int c4, c3, c2;
			//所有重复四张牌型
			static int const size4 = 3;
			uint8_t dst4[size4][4];
			//所有重复三张牌型
			static int const size3 = 4;
			uint8_t dst3[size3][4];
			//所有重复二张牌型
			static int const size2 = 6;
			uint8_t dst2[size2][4];
			//去重后的余牌/散牌
			uint8_t cpy[MaxSZ];
			int cpylen;
			void PrintCardList();
		};
		//////////////////////////////////////////////////////////////
		//dundata_t 单墩数据
		struct dundata_t {
			dundata_t() :dt_(DunNil), ty_({ TyNil }), c(0), cards({ 0 }) {
			}
			void assign(DunTy dt, HandTy ty, uint8_t const* src, int len) {
				assert(len > 0);
				assert(dt > DunNil && dt < DunMax);
				assert(len <= ((dt == DunFirst) ? 3 : 5));
				assert(dt_ == DunNil && ty_ == TyNil && c == 0);
				dt_ = dt; ty_ = ty; c = len;
				memcpy(cards, src, len);
			}
			void Reset() {
				dt_ = DunNil;
				ty_ = TyNil;
				c = 0;
				memset(cards, 0, sizeof(uint8_t) * 5);
			}
			int needC() {
				switch (dt_)
				{
				case DunFirst: return 3 - c;
				case DunSecond:
				case DunLast:  return 5 - c;
				}
				return 0;
			}
			void append(uint8_t const* src, int len) {
				assert(len > 0);
				assert(dt_ > DunNil && dt_ < DunMax);
				assert(c + len == ((dt_ == DunFirst) ? 3 : 5));
				memcpy(&cards[c], src, len);
				c += len;
			}
			inline int GetC() { return c; }
			//标记0-头/1-中/2-尾
			DunTy dt_;
			//墩对应普通牌型
			HandTy ty_;
			//墩对应牌数c(3/5/5)
			uint8_t c;
			//墩牌数据(头敦3/中墩5/尾墩5)
			uint8_t cards[5];
		};
		//////////////////////////////////////////////////////////////
		//groupdun_t 一组墩(头墩&中墩&尾墩)
		class groupdun_t {
		public:
			groupdun_t() :specialTy(TyNil), start(DunNil) {
				//memset(duns, 0, sizeof(dundata_t)*DunMax);
				for (int i = DunFirst; i <= DunLast; ++i) {
					duns[i].Reset();
				}
			}
			groupdun_t(groupdun_t const& ref) {
				copy(ref);
			}
			groupdun_t& operator=(groupdun_t const& ref) {
				return copy(ref);
			}
			groupdun_t& copy(groupdun_t const& ref) {
				start = ref.start;
				specialTy = ref.specialTy;
				memcpy(duns, ref.duns, sizeof(dundata_t)*DunMax);
				return *this;
			}
			void assign(DunTy dt, HandTy ty, uint8_t const* src, int len) {
				assert(dt != DunNil);
				if (start == DunNil || dt < start) {
					start = dt;
				}
				duns[(int)(dt)].assign(dt, ty, src, len);
			}
			int needC(DunTy dt) {
				return duns[(int)(dt)].needC();
			}
			void append(DunTy dt, uint8_t const* src, int len) {
				duns[(int)(dt)].append(src, len);
			}
			//打印指定墩牌型
			void PrintCardList(DunTy dt);
			//打印指定墩牌型
			void PrintCardList(std::string const& name, DunTy dt, HandTy ty);
		public:
			//从哪墩开始的
			DunTy start;
			//总体对应特殊牌型
			HandTy specialTy;
			//[0]头敦(3)/[1]中墩(5)/[2]尾墩(5)
			dundata_t duns[DunMax];
		};
		//////////////////////////////////////////////////////////////
		//handinfo_t 一副手牌信息
		class handinfo_t {
			friend class CGameLogic;
		public:
			handinfo_t() :rootEnumList(NULL), specialTy_(TyNil), chairID(-1), current(0), classify({ 0 }) {
				Reset();
				memset(duns_select, 0, sizeof(dundata_t)*DunMax);
			}
			~handinfo_t() {
				if (rootEnumList) {
					rootEnumList->Release();
					delete rootEnumList;
				}
			}
			//初始化
			void Init();
			void Reset();
			//打印全部枚举墩牌型
			void PrintEnumCards(bool reverse = true);
			//返回特殊牌型字符串
			std::string StringSpecialTy();
			//返回特殊牌型字符串
			static std::string StringSpecialTy(HandTy specialTy);
		protected:
			//确定手牌牌型
			void CalcHandCardsType(uint8_t const* src, int len);
		public:
			//给指定墩(头/中/尾墩)选择一组牌(头敦3/中墩5/尾墩5)
			//dt DunTy 指定为哪墩
			//src uint8_t const* 选择的一组牌(5张或3张)
			//len int 3/5张，头敦3张/中墩5张/尾墩5张
			//ty HandTy 指定墩牌型
			bool SelectAs(DunTy dt, uint8_t const* src, int len, HandTy ty);
			//返回组墩后剩余牌/散牌
			//src uint8_t const* 一副手牌13张
			//cpy uint8_t *cpy 组墩后剩余牌 cpylen int& 余牌数量
			void GetLeftCards(uint8_t const* src, int len, uint8_t *cpy, int& cpylen);
			//返回组墩总牌数
			inline int GetCardCount() { return duns_select[DunFirst].c + duns_select[DunSecond].c + duns_select[DunLast].c; }
		public:
			//玩家座椅ID
			int chairID;
			//优先特殊牌型
			HandTy specialTy_;
			//手牌重复牌型
			classify_t classify;
			//根节点：初始枚举所有牌型列表
			EnumTree *rootEnumList;
			//当前选择groups中的第几组优先
			int current;
			//枚举几组最优墩，指向EnumTree::TraverseTreeNode成员
			std::vector<groupdun_t> groups;
			//[0]头敦(3)/[1]中墩(5)/[2]尾墩(5)
			dundata_t duns_select[DunMax];
			//叶子节点列表
			//枚举几组最优墩(头墩&中墩&尾墩加起来为一组)，由叶子节点向上往根节点遍历
			//叶子节点 dt_ 成员判断当前是从哪墩节点开始，
			//叶子节点 dt_ == DunFirst  时，叶子节点(头墩)/父节点(中墩)/根节点(尾墩)
			//叶子节点 dt_ == DunSecond 时，叶子节点(中墩)/父节点 = 根节点(尾墩)
			//叶子节点 dt_ == DunLast   时，叶子节点 = 根节点(尾墩)
			std::vector<EnumTree::TraverseTreeNode> leafList;
		};
	public:
		//枚举牌型测试
		static void TestEnumCards(int size);
		//枚举牌型测试
		//filename char const* 文件读取手牌 cardsList.ini
		static void TestEnumCards(char const* filename);
		//玩家发牌测试
		static void TestPlayerCards();
		//protobuf测试
		static void TestProtoCards();
		//手动摆牌测试
		static void TestManualCards();
	public:
		//手牌牌型分析(特殊牌型判断/枚举三墩组合)，算法入口 /////////
		//src uint8_t const* 一副手牌(13张)
		//n int 最大枚举多少组墩(头墩&中墩&尾墩加起来为一组)
		//chairID int 玩家座椅ID
		//hand handinfo_t& 保存手牌信息
		static int AnalyseHandCards(uint8_t const* src, int len, int n, handinfo_t& hand);
		//单墩牌型判断(3/5张牌)
		//dt DunTy 指定为第几墩
		//src uint8_t const* 一墩5张或3张的牌
		static HandTy GetDunCardHandTy(DunTy dt, uint8_t const* src, int len);
		//牌型相同的src与dst比大小，牌数相同
		//src uint8_t const* 单墩牌(3/5张)
		//dst uint8_t const* 单墩牌(3/5张)
		//clr bool 是否比花色
		//ty HandTy 比较的两单墩牌的普通牌型
		static int CompareCards(uint8_t const* src, uint8_t const* dst, int n, bool clr, HandTy ty);
		//按照尾墩5张/中墩5张/头敦3张依次抽取枚举普通牌型
		//src uint8_t const* 手牌余牌(13/8/3)，初始13张，按5/5/3依次抽，余牌依次为13/8/3
		//n int 抽取n张(5/5/3) 第一次抽5张余8张，第二次抽5张余3张，第三次取余下3张抽完
		//classify classify_t& 存放分类信息(所有重复四张/三张/二张/散牌/余牌)
		//enumList EnumTree& 存放枚举墩牌型列表数据 dt DunTy 指定为第几墩
		static void EnumCards(uint8_t const* src, int len,
			int n, classify_t& classify, EnumTree& enumList, DunTy dt);
		//返回组墩后剩余牌/散牌
		//src uint8_t const* 一副手牌13张
		//duns dundata_t const* 一组墩(头/中/尾墩)
		//cpy uint8_t *cpy 组墩后剩余牌 cpylen int& 余牌数量
		static void GetLeftCards(uint8_t const* src, int len,
			dundata_t const* duns, uint8_t *cpy, int& cpylen);
	private:
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
		static void EnumCards(uint8_t const* src, int len, int n,
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
			std::vector<std::vector<uint8_t>>& v20);
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
		static void ClassifyCards(uint8_t const* src, int len, int n,
			uint8_t(**const pdst)[4],
			uint8_t(*dst4)[4], int& c4,
			uint8_t(*dst3)[4], int& c3,
			uint8_t(*dst2)[4], int& c2,
			uint8_t *cpy, int& cpylen,
			std::vector<std::vector<uint8_t>>& dst0,
			std::vector<std::vector<uint8_t>>& dst1,
			std::vector<std::vector<uint8_t>>& dstc);
		//返回去重后的余牌/散牌
		//src uint8_t const* 牌源
		//pdst uint8_t(**const)[4] 衔接dst4/dst3/dst2
		//dst4 uint8_t(*)[4] 存放所有四张牌型，c4 四张牌型数
		//dst3 uint8_t(*)[4] 存放所有三张牌型，c3 三张牌型数
		//dst2 uint8_t(*)[4] 存放所有对子牌型，c2 对子牌型数
		//cpy uint8_t* 去重后的余牌/散牌(除去四张/三张/对子)///////
		static int RemoveRepeatCards(uint8_t const* src, int len,
			uint8_t(**const pdst)[4],
			uint8_t(*dst4)[4], int& c4, uint8_t(*dst3)[4], int& c3,
			uint8_t(*dst2)[4], int& c2, uint8_t *cpy, int& cpylen);
		//枚举所有不区分花色n张连续牌型(同花顺/顺子)，先去重再补位，最后遍历查找
		//去重后的余牌/散牌与单张组合牌补位合并//////
		//psrc uint8_t(**const)[4] 衔接dst4/dst3/dst2
		//psrc uint8_t(**const)[4] 从所有四张/三张/对子中各取一张合成单张组合牌
		//cpy uint8_t const* 去重后的余牌/散牌(除去四张/三张/对子)牌源///////
		//n int 抽取n张(3/5/13)
		//ctx std::vector<std::vector<short>>& 标记psrc的ctx信息
		//dst std::vector<std::vector<uint8_t>>& 存放所有连续牌型
		//clr std::vector<bool>& 对应dst是否同花
		static void EnumConsecCards(
			uint8_t(**const psrc)[4], int const psrclen,
			uint8_t const* cpy, int const cpylen, int n,
			std::vector<std::vector<short>>& ctx,
			std::vector<std::vector<uint8_t>>& dst,
			std::vector<bool>& clr);
		//枚举所有n张相同花色不连续牌型(同花)，先去重再补位，最后遍历查找
		//去重后的余牌/散牌与单张组合牌补位合并//////
		//psrc uint8_t(**const)[4] 衔接dst4/dst3/dst2
		//psrc uint8_t(**const)[4] 从所有四张/三张/对子中各取一张合成单张组合牌
		//cpy uint8_t const* 去重后的余牌/散牌(除去四张/三张/对子)牌源///////
		//n int 抽取n张(3/5/13)
		//ctx std::vector<std::vector<short>>& 标记psrc的ctx信息
		//dst std::vector<std::vector<uint8_t>>& 存放所有同花牌型
		static void EnumSameColorCards(
			uint8_t(**const psrc)[4], int const psrclen,
			uint8_t const* cpy, int const cpylen, int n,
			std::vector<std::vector<short>>& ctx,
			std::vector<std::vector<uint8_t>>& dst);
		//枚举所有同花顺/顺子(区分花色五张连续牌)
		//psrc uint8_t(**const)[4] 衔接dst4/dst3/dst2
		//psrc uint8_t(**const)[4] 从所有四张/三张/对子中各取一张合成单张组合牌
		//ctx std::vector<std::vector<short>>& 标记psrc的ctx信息
		//src std::vector<std::vector<uint8_t>> const& 所有单张组合牌源///////
		//clr std::vector<bool> const& 对应src是否同花
		//dst1 std::vector<std::vector<uint8_t>>& 存放所有同花顺
		//dst2 std::vector<std::vector<uint8_t>>& 存放所有顺子
		static void EnumConsecCardsByColor(
			uint8_t(**const psrc)[4],
			std::vector<std::vector<short>> const& ctx,
			std::vector<std::vector<uint8_t>> const& src,
			std::vector<bool> const& clr,
			std::vector<std::vector<uint8_t>>& dst1,
			std::vector<std::vector<uint8_t>>& dst2);
		//枚举所有葫芦(一组三条加上一组对子)
		//psrc uint8_t(**const)[4] 衔接dst4/dst3/dst2
		//src4 uint8_t(*const)[4] 所有四张牌型牌源，c4 四张牌型数
		//src3 uint8_t(*const)[4] 所有三张牌型牌源，c3 三张牌型数
		//src2 uint8_t(*const)[4] 所有对子牌型牌源，c2 对子牌型数
		//dst std::vector<std::vector<uint8_t>>& 存放所有葫芦牌型
		static int EnumCards32(
			uint8_t(**const psrc)[4],
			uint8_t(*const src4)[4], int const c4,
			uint8_t(*const src3)[4], int const c3,
			uint8_t(*const src2)[4], int const c2,
			std::vector<std::vector<uint8_t>>& dst);
		//枚举所有三条(三张值相同的牌)
		//psrc uint8_t(**const)[4] 衔接dst4/dst3/dst2
		//src4 uint8_t(*const)[4] 所有四张牌型牌源，c4 四张牌型数
		//src3 uint8_t(*const)[4] 所有三张牌型牌源，c3 三张牌型数
		//src2 uint8_t(*const)[4] 所有对子牌型牌源，c2 对子牌型数
		//dst std::vector<std::vector<uint8_t>>& 存放所有三条牌型
		static int EnumCards30(
			uint8_t(**const psrc)[4],
			uint8_t(*const src4)[4], int const c4,
			uint8_t(*const src3)[4], int const c3,
			uint8_t(*const src2)[4], int const c2,
			std::vector<std::vector<uint8_t>>& dst);
		//枚举所有两对(两个对子加上一张单牌)
		//psrc uint8_t(**const)[4] 衔接dst4/dst3/dst2
		//src4 uint8_t(*const)[4] 所有四张牌型牌源，c4 四张牌型数
		//src3 uint8_t(*const)[4] 所有三张牌型牌源，c3 三张牌型数
		//src2 uint8_t(*const)[4] 所有对子牌型牌源，c2 对子牌型数
		//dst std::vector<std::vector<uint8_t>>& 存放所有两对牌型
		static int EnumCards22(
			uint8_t(**const psrc)[4],
			uint8_t(*const src4)[4], int const c4,
			uint8_t(*const src3)[4], int const c3,
			uint8_t(*const src2)[4], int const c2,
			std::vector<std::vector<uint8_t>>& dst);
		//枚举所有对子(一对)
		//psrc uint8_t(**const)[4] 衔接dst4/dst3/dst2
		//src4 uint8_t(*const)[4] 所有四张牌型牌源，c4 四张牌型数
		//src3 uint8_t(*const)[4] 所有三张牌型牌源，c3 三张牌型数
		//src2 uint8_t(*const)[4] 所有对子牌型牌源，c2 对子牌型数
		//dst std::vector<std::vector<uint8_t>>& 存放所有对子牌型
		static int EnumCards20(
			uint8_t(**const psrc)[4],
			uint8_t(*const src4)[4], int const c4,
			uint8_t(*const src3)[4], int const c3,
			uint8_t(*const src2)[4], int const c2,
			std::vector<std::vector<uint8_t>>& dst);
	private:
		//枚举所有指定重复牌型(四张/三张/二张)
		//src uint8_t const* 牌源
		//n int 抽取n张(4/3/2)
		//dst uint8_t(*)[4] 存放指定重复牌型
		//cpy uint8_t* 抽取后不符合要求的余牌
		static int EnumRepeatCards(uint8_t const* src, int len, int n,
			uint8_t(*dst)[4], int size, uint8_t *cpy, int& cpylen);
		//填充卡牌条码标记位
		//src uint8_t* 用A2345678910JQK来占位 size = MaxSZ
		//dst uint8_t const* 由单张组成的余牌或枚举组合牌
		static void FillCardsMarkLoc(uint8_t *src, int size, uint8_t const* dst, int dstlen, bool reset);
		//填充卡牌条码标记位
		//src uint8_t* 用2345678910JQKA来占位 size = MaxSZ
		//dst uint8_t const* 由单张组成的余牌或枚举组合牌
		static void FillCardsMarkLocByPoint(uint8_t *src, int size, uint8_t const* dst, int dstlen, bool reset);
		//从补位合并后的单张组合牌中枚举所有不区分花色n张连续牌型///////
		//src uint8_t const* 单张组合牌源，去重后的余牌/散牌与从psrc每组中各抽一张牌补位合并
		//n int 抽取n张(3/5/13)
		//start int const 检索src/mark的起始下标
		//psrcctx short const* 标记psrc的ctx信息
		//dst std::vector<std::vector<uint8_t>>& 存放所有连续牌型(不区分花色)
		//clr std::vector<bool>& 对应dst是否同花
		static int EnumConsecCardsMarkLoc(uint8_t const* src, int len, int n,
			int const start, short const* psrcctx,
			std::vector<std::vector<short>>& ctx,
			std::vector<std::vector<uint8_t>>& dst,
			std::vector<bool>& clr);
		//从补位合并后的单张组合牌中枚举所有n张指定花色牌型///////
		//src uint8_t const* 单张组合牌源，去重后的余牌/散牌与从psrc每组中各抽一张牌补位合并
		//n int 抽取n张(3/5/13)
		//clr CardColor 指定花色
		//psrcctx short const* 标记psrc的ctx信息
		//dst std::vector<std::vector<uint8_t>>& 存放所有同花牌型(非顺子)
		//consec bool false->不保留同花顺 true->保留同花顺 
		//dst2 std::vector<std::vector<uint8_t>>& 存放所有同花顺牌型
		static int EnumSameColorCardsMarkLoc(uint8_t const* src, int len, int n, CardColor clr,
			short const* psrcctx,
			std::vector<std::vector<short>>& ctx,
			std::vector<std::vector<uint8_t>>& dst,
			bool consec,
			std::vector<std::vector<uint8_t>>& dst2);
	private:
		//求组合C(n,1)*C(n,1)...*C(n,1)
		//f(k)=C(n,1)
		//Muti(k)=f(1)*f(2)...*f(k)
		//n int 访问广度
		//k int 访问深度
		//深度优先遍历，由浅到深，广度遍历，由里向外
		static int DepthVisit(int n,
			int k,
			uint8_t(*const*const psrc)[4],
			int const* colc,
			std::vector<uint8_t> const& vec,
			std::vector<std::vector<uint8_t>>& dst0,
			std::vector<std::vector<uint8_t>>& dst1);
		//递归求组合C(n,1)*C(n,1)...*C(n,1)
		//f(k)=C(n,1)
		//Muti(k)=f(1)*f(2)...*f(k)
		//n int 访问广度
		//k int 访问深度
		//深度优先遍历，由浅到深，广度遍历，由里向外
		static int DepthC(int n,
			int k, int *e, int& c,
			uint8_t(*const*const psrc)[4],
			int const* colc,
			std::vector<uint8_t> const& vec,
			std::vector<std::vector<uint8_t>>& dst0,
			std::vector<std::vector<uint8_t>>& dst1);
		//求组合C(n,k)
		//n int 访问广度
		//k int 访问深度
		//深度优先遍历，由浅到深，广度遍历，由里向外
		static int FuncC(int n, int k,
			uint8_t(*const*const psrc)[4], int const r,
			std::vector<std::vector<uint8_t>>& dst);
		//递归求组合C(n,k)
		//n int 访问广度
		//k int 访问深度
		//深度优先遍历，由浅到深，广度遍历，由里向外
		static int FuncC(int n, int k, int *e, int& c,
			uint8_t(*const*const psrc)[4], int const r,
			std::vector<std::vector<uint8_t>>& dst);
	private:
		//是否连号n张牌
		static bool CheckConsecCards(uint8_t const* src, int len);
		//是否同花n张牌
		static bool CheckSameColorCards(uint8_t const* src, int len);
		//至尊青龙/一条龙(十三水)/十二皇族
		static HandTy CheckDragonRoyal(uint8_t const* src, int len);
		//凑一色：全是红牌(方块/红心)或黑牌(黑桃/梅花)
		static HandTy CheckAllOneColor(uint8_t const* src, int len);
		//全大：全是8至A的牌型
		static HandTy CheckAllBig(uint8_t const* src, int len);
		//全小：全是2至8的牌型
		static HandTy CheckAllSmall(uint8_t const* src, int len);
	};
};

#endif