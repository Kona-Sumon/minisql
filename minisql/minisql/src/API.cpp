/*
API.cpp
APIģ��������ϵͳ�ĺ��ģ�����Ҫ����Ϊ�ṩִ��SQL���Ľӿڣ���Interpreter����á�
�ýӿ���Interpreter��������ɵ������ڲ���ʾΪ���룬����Catalog Manager�ṩ����Ϣȷ��ִ�й���
������Record Manager��Index Manager��Catalog Manager�ṩ����Ӧ�ӿڽ���ִ�У���󷵻�ִ�н����Interpreterģ�顣
�����ߣ�all
���ڣ�2021/6/20
*/
#include "API.h"

using namespace std;

//�������������Ҷ�api����������¼ӷ������������ڲ���
void API::createTable(string tableName, vector<Attribute>* attributeList, string primaryKey, int primaryKeyLocation)
{
	cout << "create table" << endl;
	cout << tableName << endl;
	vector<Attribute>::iterator p = attributeList->begin();
	while (p != attributeList->end())
	{
		cout << p->name << " " << p->type << " " << p->ifUnique << " " << p->index << endl;
		p++;
	}
	cout << primaryKey << endl;
	cout << primaryKeyLocation << endl;
}
void API::createIndex(string indexName, string tableName, string attributeName)
{
	cout << "create index" << endl;
	cout << indexName << endl;
	cout << tableName << endl;
	cout << attributeName << endl;
}
void API::dropTable(string word)
{
	cout << "drop table" << endl;
	cout << word << endl;
}
void API::dropIndex(string word)
{
	cout << "drop index" << endl;
	cout << word << endl;
}
void API::selectALL(string tableName)
{
	cout << "select * all" << endl;
	cout << tableName << endl;
}
void API::selectALL(string tableName, vector<string>* seletedAttributes)
{
	cout << "select some all" << endl;
	cout << tableName << endl;
	vector<string>::iterator p = seletedAttributes->begin();
	while (p != seletedAttributes->end())
	{
		cout << p->data() << endl;
		p++;
	}
}
void API::selectSome(string tableName, vector<Where>* WhereVector)
{
	cout << "select * where" << endl;
	cout << tableName << endl;
	vector<Where>::iterator p = WhereVector->begin();
	while (p != WhereVector->end())
	{
		cout << p->attributeName << " " << p->value << " " << p->operate << endl;
		p++;
	}
}
void API::selectSome(string tableName, vector<string>* seletedAttributes, vector<Where>* WhereVector)
{
	cout << "select some where" << endl;
	cout << tableName << endl;
	vector<string>::iterator p = seletedAttributes->begin();
	while (p != seletedAttributes->end())
	{
		cout << p->data() << endl;
		p++;
	}
	vector<Where>::iterator q = WhereVector->begin();
	while (q != WhereVector->end())
	{
		cout << q->attributeName << " " << q->value << " " << q->operate << endl;
		p++;
	}
}
void API::insertOne(string tableName, vector<string>* values)
{
	cout << "insert one" << endl;
	cout << tableName << endl;
	vector<string>::iterator p = values->begin();
	while (p != values->end())
	{
		cout << p->data() << endl;
		p++;
	}
}
void API::deleteALL(string tableName)
{
	cout << "delete all" << endl;
	cout << tableName << endl;
}
void API::deleteSome(string tableName, vector<Where>* WhereVector)
{
	cout << "delete where" << endl;
	cout << tableName << endl;
	vector<Where>::iterator p = WhereVector->begin();
	while (p != WhereVector->end())
	{
		cout << p->attributeName << " " << p->value << " " << p->operate << endl;
		p++;
	}
}