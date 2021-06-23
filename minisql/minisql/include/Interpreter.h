/*
Interpreter.h
Interpreter模块直接与用户交互，主要实现以下功能：
1.	程序流程控制，即“启动并初始化 ->‘接收命令、处理命令、显示命令结果’循环 -> 退出”流程
2.	接收并解释用户输入的命令，生成命令的内部数据结构表示，同时检查命令的语法正确性和语义正确性，
对正确的命令调用API层提供的函数执行并显示执行结果，对不正确的命令显示错误信息。
开发者：史明昊
日期：2021/6/19
*/
#ifndef _INTREPRETER_H_
#define _INTERPRETER_H_

#include <string>
int static const TYPE_FLOAT = -1;
int static const TYPE_INT = 0;
using namespace std;
class Interpreter
{
private:
	string s;
	int temp = 0;
public:
	string file;
	Interpreter() { }
	Interpreter(string str):s(str) { }
	string getOneWord();
	int change();
};

//表属性
class Attribute
{
public:
	string name; //属性名字
	int type; //属性类型，-1——float, 0——int, 正整数n——char(n)
	bool ifUnique;
	string index;  //缺省为“”空字符串，表示没有索引
	Attribute() {}
	Attribute(string n, int t, bool i, string index="")
	{
		name = n;
		type = t;
		ifUnique = i;
	}
	string getIndex()  //判断是否有索引，返回空字符串或索引名
	{ 
		return index; 
	}
	void print()//输出属性信息，名字，类型，是否unique, 是否有索引
	{
		cout << "name:" << name << ";type:" << ";ifUnique: " << ifUnique << ";index:" << index << endl;
	}
};

class SyntaxException {};//异常类

//where语句的条件比较函数 
class Where
{
public:
	string attributeName;
	string value;
	int operate;
	Where(string a, string v, int o)
	{
		attributeName = a;
		value = v;
		operate = o;
	}
	template<class T>//比较函数
	bool judge(T data);
};
#endif // !_INTREPRETER_H_


