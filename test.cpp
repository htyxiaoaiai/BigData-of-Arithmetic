#include"BigData.h"
#include<iostream>
using namespace std;

void Test()
{
	//string str;
	//str.resize(2);
	//str[0] = '1';
	//str[1] = '2';
	////str[3] = '\0';
	//char * head = (char* )str.c_str();
	//cout << head << endl;
}
void TestAdd()
{
	BigData data1("-4657209874920027687290802856093");
	//BigData data(-223372036854775808);
	BigData data2(2);
	//BigData data2(4548212345453213245);
	cout << data1 + data2 << endl;
}

void TestSub()
{
	BigData data1("99999999999999999999999999999");
	//BigData data(-223372036854775808);
	BigData data2("1999999999999999999999");
	//BigData data2(4548212345453213245);
	cout << data1 - data2 << endl;
}

void TestMul()
{
	BigData data1(2345746513257486510);
	//BigData data(-223372036854775808);
	BigData data2((INT64)0);
	//BigData data2(4548212345453213245);
	cout << data1 * data2 << endl;
}

void TestDiv()
{
	BigData data1("2222222222222222222222222222222");
	//BigData data(-223372036854775808);
	BigData data2(33);
	//BigData data2(4548212345453213245);
	cout << data1 / data2 << endl;
}
int main()
{
	//Test();
	//TestAdd();
	//TestSub();
	TestMul();
	//TestDiv();
	getchar();
	return 0;
}