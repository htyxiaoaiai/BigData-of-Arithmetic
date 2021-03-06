#pragma once
#include<string>

using namespace std;

typedef long long INT64;//将long long 类型重命名为INT64
const INT64 MAX_INT64 = 0x7fffffffffffffff;//正数的最大值
const INT64 MIN_INT64 = 0x8000000000000000;//负数的最小值
const INT64 UN_INIT = 0xcccccccccccccccc;//随机值


class BigData
{
	friend ostream& operator<<(ostream& os, const BigData& bigData);
public:
	//构造函数
	BigData(INT64 value = UN_INIT);
	BigData(char * data);
public:
	//+号重载
	BigData operator+(const BigData& bigdata);
	//-号重载
	BigData operator-(const BigData& bigdata);
	//*号的重载
	BigData operator*(const BigData& bigdata);
	//'/'号的重载
	BigData operator/(const BigData& bigdata);
protected:
	//判断字符串是否溢出
	bool isOverflowINT64()const;
	//反转字符串
	void Reverse(string & str);
	//同号字符串相加
	BigData Add(string left, string right);
	//同号字符串相减
	BigData Sub(string left, string right);
	//字符串的相乘
	BigData Mul(string left, string right);
	//字符串的相除
	BigData Div(string left, string right);
	//循环相减求商
	char SubLoop(char* left, int LSize, char* right, int RSize);
	//比较操作数的大小
	bool IsLeftBig(char * left, int LSize, char* right, int RSize);
private:
	INT64 _value;//内置类型可以存储
	string _data;//内置类型不能存储
};