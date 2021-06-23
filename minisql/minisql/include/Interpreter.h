/*
Interpreter.h
Interpreterģ��ֱ�����û���������Ҫʵ�����¹��ܣ�
1.	�������̿��ƣ�������������ʼ�� ->������������������ʾ��������ѭ�� -> �˳�������
2.	���ղ������û�������������������ڲ����ݽṹ��ʾ��ͬʱ���������﷨��ȷ�Ժ�������ȷ�ԣ�
����ȷ���������API���ṩ�ĺ���ִ�в���ʾִ�н�����Բ���ȷ��������ʾ������Ϣ��
�����ߣ�ʷ���
���ڣ�2021/6/19
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

//������
class Attribute
{
public:
	string name; //��������
	int type; //�������ͣ�-1����float, 0����int, ������n����char(n)
	bool ifUnique;
	string index;  //ȱʡΪ�������ַ�������ʾû������
	Attribute() {}
	Attribute(string n, int t, bool i, string index="")
	{
		name = n;
		type = t;
		ifUnique = i;
	}
	string getIndex()  //�ж��Ƿ������������ؿ��ַ�����������
	{ 
		return index; 
	}
	void print()//���������Ϣ�����֣����ͣ��Ƿ�unique, �Ƿ�������
	{
		cout << "name:" << name << ";type:" << ";ifUnique: " << ifUnique << ";index:" << index << endl;
	}
};

class SyntaxException {};//�쳣��

//where���������ȽϺ��� 
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
	template<class T>//�ȽϺ���
	bool judge(T data);
};
#endif // !_INTREPRETER_H_


