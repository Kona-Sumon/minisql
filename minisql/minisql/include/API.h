/*
API.h
API模块是整个系统的核心，其主要功能为提供执行SQL语句的接口，供Interpreter层调用。
该接口以Interpreter层解释生成的命令内部表示为输入，根据Catalog Manager提供的信息确定执行规则，
并调用Record Manager、Index Manager和Catalog Manager提供的相应接口进行执行，最后返回执行结果给Interpreter模块。
开发者：all
日期：2021/6/20
*/
#ifndef _API_H_
#define _API_H_
#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include "Interpreter.h"
using namespace std;
class API {
public:
	void createTable(string tableName, vector<Attribute>* attributeList, string primaryKey, int primaryKeyLocation);//创建表
	void createIndex(string indexName, string tableName, string attributeName);//创建索引
	void dropTable(string word);//删除表
	void dropIndex(string word);//删除索引
	void selectALL(string tableName);//select * no where
	void selectALL(string tableName, vector<string>* seletedAttributes);//select some no where
	void selectSome(string tableName, vector<Where>* WhereVector);//select * where
	void selectSome(string tableName, vector<string>* seletedAttributes, vector<Where>* WhereVector);//select some where
	void insertOne(string tableName, vector<string>* values);//插入一条记录
	void deleteALL(string tableName);//删除所有记录
	void deleteSome(string tableName, vector<Where>* WhereVector);//删除部分记录
};
#endif // !_API_H_