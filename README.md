# QPAlgorithm

    C++ 棋牌算法封装，牌九，扎金花，十三水...
		cfg 文件读取
		funcC 组合函数类
		pb2Json protobuf/json格式转换
		
		PJ 		牌九算法	
		s13s	十三水算法，也是最复杂的一个
		zjh		炸金花算法实现
		
		十三水算法接口说明：
			//枚举牌型测试
			static void TestEnumCards();
			
			//枚举牌型测试
			//filename char const* 文件读取手牌 cardsList.ini
			static void TestEnumCards(char const* filename);
			
			//玩家发牌测试
			static void TestPlayerCards();
			
			//protobuf测试
			static void TestProtoCards();
			
			//手动摆牌测试
			static void TestManualCards();
			
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
				
				