# QPAlgorithm

    C++ 棋牌算法封装，牌九，扎金花，十三水...
		cfg 文件读取
		funcC 组合函数类
		
		PJ 		牌九算法	
		s13s	十三水算法，也是最复杂的一个
		zjh		炸金花算法实现
		
		十三水算法接口说明：
			//枚举牌型测试
			static void TestEnumCards();
			
			//枚举牌型测试
			//filename char const* 文件读取手牌 cardsList.ini
			static void TestEnumCards(char const* filename);
			
			//手牌牌型分析(特殊牌型判断/枚举三墩组合)，算法入口 /////////
			//src uint8_t const* 一副手牌(13张)
			//n int 最大枚举多少组墩(头墩&中墩&尾墩加起来为一组)
			//hand handinfo_t& 保存手牌信息
			int AnalyseHandCards(uint8_t const* src, int len, int n, handinfo_t& hand);