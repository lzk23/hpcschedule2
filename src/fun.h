
#ifndef _FUN_H_
#define _FUN_H_

#include <string>
#include <string.h>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <queue>
#include <time.h>
#include <iostream>
#include <iterator>
#include <regex>

using namespace std;

static vector<int> str_to_int(string input){
    vector<int> nums;
    // string->char *
    char *s_input = (char *)input.c_str();
    const char * split = ",";
    // 以逗号为分隔符拆分字符串
    char *substr = strtok(s_input, split); //#include <string.h>

    int num;
    while(substr != NULL)
    {
        // char * -> int
        sscanf(substr, "%d", &num);
        nums.push_back(num);
        substr=strtok(NULL, split);
    }
    return nums;
}

static string vector_int_to_str(vector<int> &input, const char * split){
      stringstream ss;
      string str;
      copy(input.begin(), input.end(), ostream_iterator<int>(ss, split));
      str = ss.str();
      return str;
    }

static string vector_str_to_str(vector<string> &input, const char * split){
    stringstream ss;
    string str;
    copy(input.begin(), input.end(), ostream_iterator<string>(ss, split));
    str = ss.str();
    return str;
}

inline void Stringsplit(const string& str, const string& split, vector<string>& res) //正则表达式来提取某一行的数据
{
	//std::regex ws_re("\\s+"); // 正则表达式,匹配空格 
	std::regex reg(split);		// 匹配split
	std::sregex_token_iterator pos(str.begin(), str.end(), reg, -1);
	decltype(pos) end;              // 自动推导类型 
	for (; pos != end; ++pos)
	{
		res.push_back(pos->str());
	}
    //     string split = "\\s+";
    //     Stringsplit(lineStr, split, lineArray);
}

#endif