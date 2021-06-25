/*
API.h
APIģ��������ϵͳ�ĺ��ģ�����Ҫ����Ϊ�ṩִ��SQL���Ľӿڣ���Interpreter����á�
�ýӿ���Interpreter��������ɵ������ڲ���ʾΪ���룬����Catalog Manager�ṩ����Ϣȷ��ִ�й���
������Record Manager��Index Manager��Catalog Manager�ṩ����Ӧ�ӿڽ���ִ�У���󷵻�ִ�н����Interpreterģ�顣
�����ߣ�all
���ڣ�2021/6/20
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
	void createTable(string tableName, vector<Attribute>* attributeList, string primaryKey, int primaryKeyLocation);//������
	void createIndex(string indexName, string tableName, string attributeName);//��������
	void dropTable(string word);//ɾ����
	void dropIndex(string word);//ɾ������
	void selectALL(string tableName);//select * no where
	void selectALL(string tableName, vector<string>* seletedAttributes);//select some no where
	void selectSome(string tableName, vector<Where>* WhereVector);//select * where
	void selectSome(string tableName, vector<string>* seletedAttributes, vector<Where>* WhereVector);//select some where
	void insertOne(string tableName, vector<string>* values);//����һ����¼
	void deleteALL(string tableName);//ɾ�����м�¼
	void deleteSome(string tableName, vector<Where>* WhereVector);//ɾ�����ּ�¼
};
#endif // !_API_H_