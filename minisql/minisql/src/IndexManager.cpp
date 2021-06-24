#include "IndexManager.h"
#include <iostream>
// #include "API.h"
#include <vector>
using namespace std;

IndexManager::IndexManager(API* m_api)
{
    api = m_api;
    vector<IndexInfo> allIndexInfo;
    api->allIndexAddressInfoGet(&allIndexInfo);
    for(vector<IndexInfo>::iterator i = allIndexInfo.begin();i != allIndexInfo.end();i ++)
    {
        createIndex(i->indexName, i->type);
    }
}

IndexManager::~IndexManager()
{
    for(intMap::iterator itInt = indexIntMap.begin();itInt != indexIntMap.end(); itInt ++)
    {
        if(itInt->second)
        {
            itInt -> second->writtenbackToDiskAll();
            delete itInt->second;
        }
    }
    for(stringMap::iterator itString = indexStringMap.begin();itString != indexStringMap.end(); itString ++)
    {
        if(itString->second)
        {
            itString ->second-> writtenbackToDiskAll();
            delete itString->second;
        }
    }
    for(floatMap::iterator itFloat = indexFloatMap.begin();itFloat != indexFloatMap.end(); itFloat ++)
    {
        if(itFloat->second)
        {
            itFloat ->second-> writtenbackToDiskAll();
            delete itFloat->second;
        }
    }
}

void IndexManager::createIndex(string filePath,int type)
{
    int keySize = getKeySize(type);
    int degree = getDegree(type);
    if(type == TYPE_INT)
    {
        BPlusTree<int> *tree = new BPlusTree<int>(filePath,keySize,degree);
        indexIntMap.insert(intMap::value_type(filePath, tree));
    }
    else if(type == TYPE_FLOAT)
    {
        BPlusTree<float> *tree = new BPlusTree<float>(filePath,keySize,degree);
        indexFloatMap.insert(floatMap::value_type(filePath, tree));
    }
    else
    {
        BPlusTree<string> *tree = new BPlusTree<string>(filePath,keySize,degree);
        indexStringMap.insert(stringMap::value_type(filePath, tree));
    }
}

void IndexManager::SearchRange(string filePath, keyTmp kt1, keyTmp kt2, vector<int>& vals)
{
    
    int tag = 0;
    //检测数据类型是否匹配
    if (kt1.type == -2) {
        tag = 1;
    } else if (kt2.type == -2) {
        tag = 2;
    }
    
    else if (kt1.type != kt2.type) {
        cout << "格式错误!" << endl;
        //return;
    }
    
    if (kt1.type == TYPE_INT) {
        intMap::iterator itInt = indexIntMap.find(filePath);
        if (itInt == indexIntMap.end()) {
            cout << "索引不存在" << endl;
            // return;
        } else
            itInt->second->searchRange(kt1.intTmp, kt2.intTmp, vals, tag);
    } else if (kt1.type == TYPE_FLOAT) {
        floatMap::iterator itFloat = indexFloatMap.find(filePath);
        if (itFloat == indexFloatMap.end()) {
            cout << "索引不存在" << endl;
            // return;
        } else
            itFloat->second->searchRange(kt1.floatTmp, kt2.floatTmp, vals, tag);
    } else {
        stringMap::iterator itString = indexStringMap.find(filePath);
        if(itString == indexStringMap.end()) {
            cout << "索引不存在" << endl;
            // return;
        }
        else
            itString->second->searchRange(kt1.stringTmp, kt2.stringTmp, vals, tag);
    }
}

void IndexManager::dropIndex(string filePath,int type)
{
    if(type == TYPE_INT)
    {
        intMap::iterator itInt = indexIntMap.find(filePath);
        if(itInt == indexIntMap.end())
        {
            cout << "索引不存在" << endl;
            //return;
        }
        else
        {
            delete itInt->second;
            indexIntMap.erase(itInt);
        }
    }
    else if(type == TYPE_FLOAT)
    {
        floatMap::iterator itFloat = indexFloatMap.find(filePath);
        if(itFloat == indexFloatMap.end())
        {
            cout <<  "索引不存在" << endl;
            //return;
        }
        else
        {
            delete itFloat->second;
            indexFloatMap.erase(itFloat);
        }
    }
    else
    {
        stringMap::iterator itString = indexStringMap.find(filePath);
        if(itString == indexStringMap.end())
        {
            cout << "索引不存在" << endl;
            //return;
        }
        else
        {
            delete itString->second;
            indexStringMap.erase(itString);
        }
    }

}

int IndexManager::searchIndex(string filePath,string key,int type)
{
    setKey(type, key);
    if(type == TYPE_INT)
    {
        intMap::iterator itInt = indexIntMap.find(filePath);
        if(itInt == indexIntMap.end())
        {
            cout <<  "索引不存在" << endl;
            return -1;
        }
        else
        {
            return itInt->second->search(kt.intTmp);
        }
    }
    else if(type == TYPE_FLOAT)
    {
        floatMap::iterator itFloat = indexFloatMap.find(filePath);
        if(itFloat == indexFloatMap.end())
        {
            cout <<  "索引不存在" << endl;
            return -1;
        }
        else
        {
            return itFloat->second->search(kt.floatTmp);

        }
    }
    else
    {
        stringMap::iterator itString = indexStringMap.find(filePath);
        if(itString == indexStringMap.end())
        {
            cout <<  "索引不存在" << endl;
            return -1;
        }
        else
        {
            return itString->second->search(key);
        }
    }
}

void IndexManager::insertIndex(string filePath,string key,int blockOffset,int type)
{
    setKey(type, key);

    if(type == TYPE_INT)
    {
        intMap::iterator itInt = indexIntMap.find(filePath);
        if(itInt == indexIntMap.end())
        {
            cout << "索引不存在" << endl;
            // return;
        }
        else
        {
            itInt->second->insertKey(kt.intTmp,blockOffset);
        }
    }
    else if(type == TYPE_FLOAT)
    {
        floatMap::iterator itFloat = indexFloatMap.find(filePath);
        if(itFloat == indexFloatMap.end())
        {
            cout << "索引不存在" << endl;
            // return;
        }
        else
        {
            itFloat->second->insertKey(kt.floatTmp,blockOffset);
            
        }
    }
    else
    {
        stringMap::iterator itString = indexStringMap.find(filePath);
        if(itString == indexStringMap.end())
        {
            cout << "索引不存在" << endl;
            // return;
        }
        else
        {
            itString->second->insertKey(key,blockOffset);
        }
    }
}

void IndexManager::deleteIndexByKey(string filePath,string key,int type)
{
    setKey(type, key);

    if(type == TYPE_INT)
    {
        intMap::iterator itInt = indexIntMap.find(filePath);
        if(itInt == indexIntMap.end())
        {
            cout << "索引不存在" << endl;
            //return;
        }
        else
        {
            itInt->second->deleteKey(kt.intTmp);
        }
    }
    else if(type == TYPE_FLOAT)
    {
        floatMap::iterator itFloat = indexFloatMap.find(filePath);
        if(itFloat == indexFloatMap.end())
        {
            cout << "索引不存在" << endl;
            //return;
        }
        else
        {
            itFloat->second->deleteKey(kt.floatTmp);
            
        }
    }
    else
    {
        stringMap::iterator itString = indexStringMap.find(filePath);
        if(itString == indexStringMap.end())
        {
            cout << "索引不存在" << endl;
            //return;
        }
        else
        {
            itString->second->deleteKey(key);
        }
    }
}

int IndexManager::getDegree(int type)
{
    int degree = bm.getBlockSize()/(getKeySize(type)+sizeof(int));
    if(degree %2 == 0) degree -= 1;
    return degree;
}

int IndexManager::getKeySize(int type)
{
    if(type == TYPE_FLOAT)
        return sizeof(float);
    else if(type == TYPE_INT)
        return sizeof(int);
    else if(type > 0)
        return type + 1;
    else
    {
        cout << "格式错误" << endl;
        return -100;
    }
}

void IndexManager::setKey(int type,string key)
{
    stringstream ss;
    ss << key;
    if(type == this->TYPE_INT) 
    {
        this->kt.type = type;
        ss >> this->kt.intTmp;
    }
    else if(type == this->TYPE_FLOAT) 
    {
        this->kt.type = type;
        ss >> this->kt.floatTmp;
    }
    else if(type > 0)
    {
        this->kt.type = type;
        ss >> this->kt.stringTmp;
    }
    else
        cout << "格式错误" << endl;
}
