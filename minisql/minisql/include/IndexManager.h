#ifndef __IndexManager__
#define __IndexManager__

#include <stdio.h>
#include <map>
#include <string>
#include <sstream>
#include "BPTree.h"

class API;

class Attribute //属性类 
{
	public:
		string name; //属性名字
		int type; //属性类型，-1——float, 0——int,正整数n——char型，n表示多少个char
		bool ifUnique;
		string index;  //缺省为""空字符串，表示没有索引
		Attribute(string n,int t, bool i)//构造函数，name=n,type=t,ifUnique=i
		{
	        name = n;
	        type = t;
	        ifUnique = i;
	        index = ""; 
        }
	    string indexNameGet(){return index;} //判断是否有索引，返回空字符串或索引名
		
		void print()//输出属性信息，名字，类型，是否unique 
		{
			cout << "name:" << name << ";type:" << ";ifUnique: " << ifUnique << ";index:" << index << endl;	
		}	 	    
};

class IndexInfo//index信息类，索引名，表名，属性名，类型
{
	public:
		string indexName;
        string tableName;
        string Attribute;
        int type;
		IndexInfo(string i,string t,string a,int ty)//构造函数 
		{
			indexName = i;
            tableName = t;
            Attribute = a;
            type = ty;
		}
};

class IndexManager{
private:
    typedef map<string,BPlusTree<int> *> intMap;
    typedef map<string,BPlusTree<string> *> stringMap;
    typedef map<string,BPlusTree<float> *> floatMap;
    
    int static const TYPE_FLOAT = 0;
    int static const TYPE_INT = -1;
    // other values mean the size of the char.Eg, 4 means char(4);
    
    API *api; // to call the functions of API
    
    intMap indexIntMap;
    stringMap indexStringMap;
    floatMap indexFloatMap;

    struct keyTmp{
        int type;
        int intTmp;
        float floatTmp;
        string stringTmp;
    };
    struct keyTmp kt;
    
    int getDegree(int type);
    
    int getKeySize(int type);
    
    void setKey(int type,string key);
    
    
public:
    IndexManager(API *api);
    ~IndexManager();
    
    void createIndex(string filePath,int type);
    
    void dropIndex(string filePath,int type);
    
    int searchIndex(string filePath,string key,int type);
    
    void insertIndex(string filePath,string key,int blockOffset,int type);
    
    void deleteIndexByKey(string filePath,string key,int type);

    void SearchRange(string filePath, keyTmp kt1, keyTmp kt2, vector<int>& vals);
};

#endif