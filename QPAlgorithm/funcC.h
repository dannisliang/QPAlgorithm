//
// Created by andy_ro@qq.com
// 			5/26/2019
//
#ifndef FUNC_C_H
#define FUNC_C_H

#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <memory.h>
#include <list>
#include <vector>

//求组合C(n,k)
class CFuncC {
public:
	static void Print(std::vector<std::vector<int>>& vec);
	static void Test();
	static void Test1();
public:
	CFuncC();
	//深度遍历实现找规律
	static int Depth();
	//求组合C(n,k)，返回数组集合
	//arr((i0),...) = F(C(n,1))
	//arr((i0,i1),...) = F(C(n,2))
	//arr((i0,i1,i2),...) = F(C(n,3))
	//arr((i0,i1,...ik),...) = F(C(n,k))
	int FuncC(int n, int k, std::vector<std::vector<int>>& vec);
	//求组合C(n,1)*C(n,1)...*C(n,1)
	//f(k)=C(n,1)
	//Muti(k)=f(1)*f(2)...*f(k)
	//n int 访问广度
	//k int 访问深度
	//深度优先遍历，由浅到深，广度遍历，由里向外
	int DepthVisit(int n, int k);
private:
	//递归求组合C(n,k)
	//f(k)=C(n,1)
	//Muti(k)=f(1)*f(2)...*f(k)
	//n int 访问广度
	//k int 访问深度
	//深度优先遍历，由浅到深，广度遍历，由里向外
	int StaC(int n, int k, std::vector<std::vector<int>>& vec);
	//递归求组合C(n,1)*C(n,1)...*C(n,1)
	//n int 访问广度
	//k int 访问深度
	//深度优先遍历，由浅到深，广度遍历，由里向外
	int DepthC(int n, int k);
private:
	int c;
	static int const KDEPTH = 13;
	int e[KDEPTH + 1];
	std::vector<int> v;
};

#endif		 


