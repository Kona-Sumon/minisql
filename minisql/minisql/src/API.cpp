/*
API.cpp
API模块是整个系统的核心，其主要功能为提供执行SQL语句的接口，供Interpreter层调用。
该接口以Interpreter层解释生成的命令内部表示为输入，根据Catalog Manager提供的信息确定执行规则，
并调用Record Manager、Index Manager和Catalog Manager提供的相应接口进行执行，最后返回执行结果给Interpreter模块。
开发者：all
日期：2021/6/20
*/
#include "API.h"

using namespace std;

//框架是这样，大家对api有需求可以新加方法，下面用于测试
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