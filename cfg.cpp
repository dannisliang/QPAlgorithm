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

#include "cfg.h"

//按行读取文件
void readFile(char const* filename, std::vector<std::string>& lines, char const* skip) {
#if 1
	{
		FILE* fp = fopen(filename, "r");
		if (fp == NULL) {
			return;
		}
		char line[512] = { 0 };
		while (!feof(fp)) {
			if (NULL != fgets(line, 512, fp)) {
				int len = strlen(line);
				if (line[len - 1] == '\r' ||
					(line[len - 2] != '\r' && line[len - 1] == '\n')) {
					//去掉\r或\n结尾
					line[len - 1] = '\0';
					--len;
				}
				else if (line[len - 2] == '\r' && line[len - 1] == '\n') {
					//0 == strcmp(&line[len-2], "\r\n")
					//去掉\r\n结尾
					line[len - 2] = '\0';
					line[len - 1] = '\0';
					len -= 2;
				}
				//printf(line);
				if (line[0] != '\0') {
					if (len >= strlen(skip) && 0 == strncmp(line, skip, strlen(skip))) {
					}
					else {
						lines.push_back(line);
					}
				}
			}
		}
		//printf("--- *** fgets:%d\n", lines.size());
		//for (std::vector<string>::iterator it = lines.begin();
		//	it != lines.end(); ++it) {
		//	printf("%s\n", it->c_str());
		//}
		fclose(fp);
	}
#else
	{
		lines.clear();
		std::fstream fs(filename);
		std::string line;
		while (getline(fs, line)) {
			int len = line.size();
			if (line[len - 1] == '\r' ||
				(line[len - 2] != '\r' && line[len - 1] == '\n')) {
				//去掉\r或\n结尾
				line[len - 1] = '\0';
				--len;
			}
			else if (line[len - 2] == '\r' && line[len - 1] == '\n') {
				//去掉\r\n结尾
				line[len - 2] = '\0';
				line[len - 1] = '\0';
				len -= 2;
			}
			//printf(line.c_str());
			if (line[0] != '\0') {
				if (len >= strlen(skip) &&
					0 == strncmp(&line.front(), skip, strlen(skip))) {
				}
				else {
					lines.push_back(line);
				}
			}
		}
		//printf("--- *** getline:%d\n", lines.size());
		//for (std::vector<string>::iterator it = lines.begin();
		//	it != lines.end(); ++it) {
		//	printf("%s\n", it->c_str());
		//}
	}
#endif
}