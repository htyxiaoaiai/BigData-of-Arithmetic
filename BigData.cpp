#include"BigData.h"
#include<iostream>
#include<assert.h>
#include<string.h>

//反转字符串
void BigData::Reverse(string & str)
{
	size_t left = 1;
	size_t right = str.size() - 1;
	while (left < right)
	{
		swap(str[left++], str[right--]);
	}
}

//构造函数
BigData::BigData(INT64 value)
	:_value(value)
{
	if (_value != UN_INIT)
	{
		char cSymbol = '+';
		INT64 tmp = _value;
		if (_value < 0)
		{
			cSymbol = '-';
			tmp = -_value;
		}
		//因为没有开辟空间，不能通过数组下标使用
		_data.append(1, cSymbol);
		char number = 0;
		while (tmp)
		{
			number = tmp % 10 + '0';
			tmp = tmp / 10;
			_data.append(1, number);//在字符串后追加字符
		}
		Reverse(_data);
	}
}

BigData::BigData(char * data)
{
	assert(data);
	//"123456" "+1134355435" "-44655465783" "0654546"  "8786545sfa345" "+000654564"
	//"a55453466" "    12345465"

	//消除多余的空格
	while (isspace(*data))
	{
		data++;
	}

	char cSymbol = *data;//符号位
	//处理符号，跳过符号位
	if ('+' == cSymbol || '-' == cSymbol)
	{
		data++;
	}

	//如果开始的字符是合法字符
	if (*data >= '0'&& *data <= '9')
	{
		//消除多于的0
		while (*data == '0')
		{
			data++;
		}

		//多开辟一个字节存放符号位
		_data.resize(strlen(data) + 1);
		if ('-' == cSymbol)
		{
			_data[0] = cSymbol;
		}
		else
		{
			_data[0] = '+';
		}

		//存储数据
		int i = 1;
		while (*data >= '0'&& *data <= '9')
		{
			_data[i++] = *data++;
		}

		//没有溢出
		if (!isOverflowINT64())
		{
			_value = 0;
			for (size_t i = 1; i < _data.size(); i++)
			{
				_value = _value * 10 + _data[i] - '0';
			}
			if (_data[0] == '-')
			{
				_value *= -1;
			}
		}

	}
	else
	{
		//开始的字符为非法字符
		_value = UN_INIT;
		//字符串中什么元素都没有
	}
}

//判断字符串是否溢出
bool BigData::isOverflowINT64()const
{
	string cmp("+9223372036854775807");
	if (_data[0] == '-')
	{
		cmp = "-9223372036854775808";
	}

	if (cmp.size() > _data.size() ||
		(cmp.size() == _data.size() && _data <= cmp))
	{
		return false;
	}
	return true;
}

//输出运算符重载
ostream& operator<<(ostream& os, const BigData& bigData)
{
	//没有溢出
	if (!bigData.isOverflowINT64())
	{
		cout << bigData._value;
	}
	//溢出
	else
	{
		char * value = (char*)bigData._data.c_str();
		bool flag = true;//标记是否为负数

		//跳过正号和多余的0
		while (*value == '+' || *value == '0')
		{
			value++;
			flag = false;
		}
		//数字为负的话则
		while (*value == '-' || *value == '0')
		{
			value++;
			flag = true;
		}
		//输出符号位
		if (flag)
		{
			cout << '-';
		}
		//输出数字位
		cout << value;

	}
	return os;
}

BigData BigData::operator+(const BigData& bigdata)
{
	//同号用加法
	//异号可以转换为减法

	//两个数都没有溢出
	if (!isOverflowINT64() && !bigdata.isOverflowINT64())
	{
		//同号
		if (_data[0] == bigdata._data[0])
		{
			//相加的结果没有溢出
			if ((_data[0] == '+'&& MAX_INT64 - _value > bigdata._value)
				|| (_data[0] == '-'&& MIN_INT64 - _value < bigdata._value))
			{
				return BigData(_value + bigdata._value);
			}
			//相加的结果溢出
			else
			{
				return Add(_data, bigdata._data);
			}
		}
		//异号,肯定不会溢出
		else
		{
			return BigData(_value + bigdata._value);
		}

	}
	//至少有一个数溢出
	else
	{
		return Add(_data, bigdata._data);
	}
}

//字符串的相加
BigData BigData::Add(string left, string right)
{
	BigData ret;//计算结果
	//同号
	if (left[0] == right[0])
	{
		//约定左操作数的位数始终大于等于右操作数
		size_t LSize = left.size();
		size_t RSize = right.size();


		//右操作数的位数大于左操作数，交换位置
		if (LSize < RSize)
		{
			swap(left, right);
			swap(LSize, RSize);
		}

		//多开辟一位，以防有进位出现
		ret._data.resize(LSize + 1);
		//保存符号位
		ret._data[0] = left[0];

		//开始假设进位为0
		char Carry = 0;
		//从最低位开始相加
		for (size_t i = 1; i < LSize; i++)
		{
			//相同位数的相加
			if (i < RSize)
			{
				ret._data[LSize - i + 1] = (left[LSize - i] - '0' + right[RSize - i] - '0' + Carry) % 10 + '0';
				Carry = (left[LSize - i] - '0' + right[RSize - i] - '0' + Carry) / 10;
			}
			//不同位数的相加
			else
			{
				ret._data[LSize - i + 1] = (left[LSize - i] - '0' + Carry) % 10 + '0';
				Carry = (left[LSize - i] - '0' + Carry) / 10;
			}

		}
		ret._data[1] = Carry + '0';
		return ret;
	}
	//异号
	else
	{
		//调用字符串的减法
		return Sub(left, right);
	}
}



//-号的重载
BigData BigData::operator-(const BigData& bigdata)
{
	//异号则使用加法
	//同号则使用减法

	//两个数都没有溢出
	if (!isOverflowINT64() && !bigdata.isOverflowINT64())
	{
		//同号，则不会溢出
		if (_data[0] == bigdata._data[0])
		{
			return BigData(_value - bigdata._value);
		}
		//异号
		else
		{
			BigData tmp = bigdata;
			//左操作数为正右操作数为负
			if (_data[0] == '+')
			{
				tmp._data[0] = '+';
			}
			//左操作数为负，右操作数为正
			else
			{
				tmp._data[0] = '-';

			}
			tmp._value = -bigdata._value;
			return *this + tmp;
		}
	}
	//至少有一个数溢出
	else
	{
		//异号使用加法
		if (_data[0] != bigdata._data[0])
		{
			BigData tmp = bigdata;
			//左操作数为正右操作数为负
			if (_data[0] == '+')
			{
				tmp._data[0] = '+';
			}
			//左操作数为负，右操作数为正
			else
			{
				tmp._data[0] = '-';

			}
			return Add(_data, tmp._data);
		}
		//同号使用减法
		return Sub(_data, bigdata._data);
	}
}

//同号字符串的相减
BigData BigData::Sub(string left, string right)
{
	int LSize = left.size();
	int RSize = right.size();
	//符号位
	char cSymbol = left[0];
	//保证左操作数大于右操作数
	if (LSize < RSize
		|| (LSize == RSize && left < right))
	{
		swap(left, right);
		swap(LSize, RSize);
		//符号位与初始的符号位相反
		if (left[0] == '+')
		{
			cSymbol = '-';
		}
		else
		{
			cSymbol = '+';
		}
	}

	BigData ret;
	ret._data.resize(LSize);
	ret._data[0] = cSymbol;
	//相减
	for (int Idx = 1; Idx < LSize; ++Idx)
	{
		//取左操作数
		char cRet = left[LSize - Idx] - '0';
		//取右操作数
		if (Idx < RSize)
		{
			cRet -= right[RSize - Idx] - '0';
		}
		//需要借位，多次借位也能处理
		if (cRet < 0)
		{
			left[LSize - Idx - 1] -= 1;
			cRet += 10;
		}
		ret._data[LSize - Idx] = cRet + '0';
	}
	return ret;
}

//*号的重载
BigData BigData::operator*(const BigData& bigdata)
{
	//有任意一个操作数为0直接返回0，提高效率，并且防止被0除
	if ((isOverflowINT64() && _value == 0) || (bigdata.isOverflowINT64() && bigdata._value == 0))
	{
		return BigData((INT64)0);
	}
	//两个数都没有溢出
	if (!isOverflowINT64() && !bigdata.isOverflowINT64())
	{
		//相乘的结果没有溢出
		if ((_data[0] == bigdata._data[0] && MAX_INT64 / _value >= bigdata._value) ||
			(_data[0] != bigdata._data[0] && _data[0] == '-' && MIN_INT64 / _value >= bigdata._value) ||
			(_data[0] != bigdata._data[0] && _data[0] == '+' && MIN_INT64 / _value <= bigdata._value))
		{
			return BigData(_value*bigdata._value);
		}
		//相乘的结果溢出
		else
		{
			return Mul(_data, bigdata._data);
		}
	}
	//至少有一个数字溢出
	else
	{
		return Mul(_data, bigdata._data);
	}
}

//字符串的相乘
BigData BigData::Mul(string left, string right)
{
	int LSize = left.size();
	int RSize = right.size();
	//约定左操作数较小，cpu的跨层切换比较少，效率较高
	if (LSize > RSize)
	{
		swap(left, right);
		swap(LSize, RSize);
	}
	//符号位
	char cSymbol = '+';
	if (left[0] != right[0])
	{
		cSymbol = '-';
	}
	//为结果开辟空间并将字符串初始化为0
	BigData ret;
	ret._data.resize(LSize + RSize - 1, '0');
	int DataLen = ret._data.size();
	ret._data[0] = cSymbol;
	int cStep = 0;//进位
	//字符串相乘
	//左字符串的每一位与右字符串的每一位相乘，并且移位相加
	int offset = 0;//移位
	for (int Lidx = 1; Lidx < LSize; Lidx++)
	{
		cStep = 0;//每乘一位就将进位赋为0
		char cLeft = left[LSize - Lidx] - '0';
		char cRet = cLeft;
		//如果左操作数的某一位为0则不用相乘直接将移位计数器左移一位
		if (cLeft == 0)
		{
			offset++;
			continue;
		}
		int Ridx = 1;
		for (Ridx = 1; Ridx < RSize; Ridx++)
		{
			//相乘
			cRet = cLeft*(right[RSize - Ridx] - '0');
			//加上一次的结果
			cRet += (ret._data[DataLen - offset - Ridx] - '0');
			//加进位
			cRet += cStep;
			//保存结果
			ret._data[DataLen - offset - Ridx] = cRet % 10 + '0';
			//求进位
			cStep = cRet / 10;

		}
		//高位的进位
		ret._data[DataLen - offset - Ridx] = cStep + '0';
		//移位
		offset++;
	}
	return ret;
}


//'/'号的重载
BigData  BigData::operator/(const BigData& bigdata)
{
	//除数不能为0
	//断言
	if (!bigdata.isOverflowINT64() && 0 == bigdata._value)
	{
		assert(false);
	}


	//特殊情况

	//被除数等于0，或者除数的size小于被除数的size
	if (_data[0] == '0' || _data.size() < bigdata._data.size()
		|| (_data.size() == bigdata._data.size() &&
			strcmp((char*)_data.c_str() + 1, (char*)bigdata._data.c_str() + 1) < 0))
	{
		return BigData((INT64)0);
	}

	//数值部分相等
	if (_data.size() == bigdata._data.size() &&
		strcmp((char*)_data.c_str() + 1, (char*)bigdata._data.c_str() + 1) == 0)
	{
		if (_data[0] == bigdata._data[0])
		{
			return BigData(1);
		}
		else
		{
			return BigData(-1);
		}
	}

	//除1
	if (!bigdata.isOverflowINT64() && bigdata._value == 1)
	{
		return *this;
	}

	//除-1
	if (!bigdata.isOverflowINT64() && bigdata._value == -1)
	{
		BigData tmp = *this;
		if (tmp._data[0] == '+')
		{
			tmp._data[0] = '-';
		}
		else
		{
			tmp._data[0] = '+';
		}

		return tmp;
	}


	//两个数都没有溢出
	if (!isOverflowINT64() && !bigdata.isOverflowINT64())
	{
		//一般情况
		return BigData(_value / bigdata._value);
	}
	//至少有一个数溢出
	else
	{
		return Div(_data, bigdata._data);
	}
}

//字符串的相除
BigData  BigData::Div(string left, string right)
{
	//处理符号位
	char cSymbol = '+';
	if (left[0] != right[0])
	{
		cSymbol = '-';
	}
	BigData ret;
	//字符串追加
	ret._data.append(1, cSymbol);

	//开始除
	//跳过符号位
	char* pLeft = (char*)left.c_str() + 1;
	char* pRight = (char*)right.c_str() + 1;
	//除去符号位之后的字符长度
	int LSize = left.size() - 1;
	int RSize = right.size() - 1;
	//一次取得区间长度 为右边字符串的长度
	int DataLen = RSize;

	//idx与pLeft保持同步,保留pLeft的位置
	for (int Idx = 0; Idx < LSize;)
	{
		//如果遇到0//*pleft==0,跳过
		if (*pLeft == '0')
		{
			pLeft++;
			Idx++;
			ret._data.append(1, '0');
			continue;
		}
		//取到的左边的数字比右边的数字要小
		if (!IsLeftBig(pLeft, DataLen, pRight, RSize))
		{
			//DatLen不能无限制的增大，防止越界
			DataLen++;
			ret._data.append(1, '0');
			if (Idx + DataLen > LSize)
			{
				break;
			}
			continue;
		}
		else
		{
			//将计算的商追加到字符串的后面
			ret._data.append(1, SubLoop(pLeft, DataLen, pRight, RSize));
			//去除多余的0
			while (*pLeft == '0'&&DataLen > 0)
			{
				pLeft++;
				Idx++;
				DataLen--;
			}
		}
		//Datalen不能无限制的增加
		DataLen++;
		if (Idx + DataLen > LSize)
		{
			break;
		}
	}

	return ret;
}


//循环相减求商
char BigData::SubLoop(char* left, int LSize, char* right, int RSize)
{
	//计数器，商

	//特殊情况3300000000000 33 
	char count = '0';
	while (true)
	{
		if (!IsLeftBig(left, LSize, right, RSize))
		{
			break;
		}
		int LIdx = LSize - 1;
		int RIdx = RSize - 1;
		//实现-=
		while (RIdx >= 0)
		{
			char cRet = left[LIdx] - '0';
			cRet -= right[RIdx] - '0';
			//有借位
			if (cRet < 0)
			{
				left[LIdx - 1] -= 1;
				cRet += 10;
			}
			left[LIdx] = cRet + '0';
			RIdx--;
			LIdx--;
		}
		count++;
		//去掉前置的0，不能无休止的跳
		//3300000000000 33减完一次之后变为00000000000，
		while (*left == '0'&&LSize > 0)
		{
			left++;
			LSize--;
		}
	}
	return count;
}


//比较操作数的大小
//2222222 33
//LSzie为取222222中的个数
bool BigData::IsLeftBig(char * left, int LSize, char* right, int RSize)
{
	if (LSize > RSize ||
		(LSize == RSize && strncmp(left, right, LSize) >= 0))
		//使用strcmp会比较更多的元素
	{
		return true;
	}
	return false;
}