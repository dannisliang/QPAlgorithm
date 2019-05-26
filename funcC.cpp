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

#include "funcC.h"

//求组合C(n,k)
CFuncC::CFuncC() :c(0) {
}

//深度遍历实现找规律
int CFuncC::Depth() {
	//深度优先遍历，由浅到深，广度遍历，由里向外
	printf("\n------------\n");
	int c = 0;
	int e[6] = { 0 };
	e[0] = 4;
	for (int i = 4; i > 0; --i) {
		e[1] = i;//深度为1
		for (int j = 4; j > 0; --j) {
			e[2] = j;//深度为2
			for (int k = 4; k > 0; --k) {
				e[3] = k;//深度为3
				for (int x = 4; x > 0; --x) {
					e[4] = x;//深度为4
					//for (int y = 4; y > 0; --y) {
					//	e[5] = y;//深度为5
						//for (int z = 4; z > 0; --z) {
							//e[6] = z;//深度为6
							++c;
							for (int m = 1; m <= e[0]; ++m) {
								printf("%d", e[m]);
							}
							printf(" ");
						//}
					//}
				}
			}
		}
	}
	printf("\nc = %d\n\n", c);
	return c;
}

//求组合C(n,k)，返回数组集合
//arr((i0),...) = F(C(n,1))
//arr((i0,i1),...) = F(C(n,2))
//arr((i0,i1,i2),...) = F(C(n,3))
//arr((i0,i1,...ik),...) = F(C(n,k))
int CFuncC::FuncC(int n, int k, std::vector<std::vector<int>>& vec) {
	c = 0;
	e[0] = k;
	vec.clear();
	return StaC(n, k, vec);
}

//求组合C(n,1)*C(n,1)...*C(n,1)
//f(k)=C(n,1)
//Muti(k)=f(1)*f(2)...*f(k)
//n int 访问广度
//k int 访问深度
//深度优先遍历，由浅到深，广度遍历，由里向外
int CFuncC::DepthVisit(int n, int k) {
	c = 0;
	e[0] = k;
	return DepthC(n, k);
}

//递归求组合C(n,k)
//n int 访问广度
//k int 访问深度
//深度优先遍历，由浅到深，广度遍历，由里向外
int CFuncC::StaC(int n, int k, std::vector<std::vector<int>>& vec) {
	for (int i = n; i >= k; --i) {
		assert(k > 0);
		e[k] = i;
		if (k > 1) {
			//递归C(i-1,k-1)
			StaC(i - 1, k - 1, vec);
		}
		else {
			++c;
			for (int j = e[0]; j > 0; --j) {
				//printf("%d", e[j]);
				v.push_back(e[j] - 1);
			}
			//printf(",");
			vec.push_back(v);
			v.clear();
		}
	}
	//返回组合数
	return c;
}

//递归求组合C(n,1)*C(n,1)...*C(n,1)
//f(k)=C(n,1)
//Muti(k)=f(1)*f(2)...*f(k)
//n int 访问广度
//k int 访问深度
//深度优先遍历，由浅到深，广度遍历，由里向外
int CFuncC::DepthC(int n, int k) {
	for (int i = n; i > 0; --i) {
		assert(k > 0);
		e[k] = i;
		if (k > 1) {
			//递归C(n,k-1)
			DepthC(n, k - 1);
		}
		else {
			++c;
			for (int j = e[0]; j > 0; --j) {
				printf("%d", e[j]);
			}
			printf(" ");
		}
	}
	//返回组合数
	return c;
}

void CFuncC::Print(std::vector<std::vector<int>>& vec) {
	for (std::vector<std::vector<int>>::iterator it = vec.begin();
		it != vec.end(); ++it) {
		for (std::vector<int>::iterator ir = it->begin();
			ir != it->end(); ++ir) {
			printf("%d", *ir);
		}
		printf(" ");
	}
	printf("\n");
}

void CFuncC::Test() {
	CFuncC C;
	std::vector<std::vector<int>> vec;
	while (1) {
		int n, k;
		cin >> n >> k;
		int c = C.FuncC(n, k, vec);
		printf("\n%d\n", c);
		//CFuncC::Print(vec);
	}
}

void CFuncC::Test1() {
	int c0 = CFuncC::Depth();
	CFuncC C;
	int c = C.DepthVisit(4, 4);
	printf("\nc0=%d c=%d\n\n", c0, c);
}