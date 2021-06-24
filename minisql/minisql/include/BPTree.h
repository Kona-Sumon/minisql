#ifndef __BPlusTree__
#define __BPlusTree__
#include <vector>
#include <stdio.h>
#include <string.h>
#include <iostream>
// #include "BufferManager.h"
// #include "Minisql.h"
#include <string>
using namespace std;

static BufferManager bm;
//**********************TreeNode***************************//

template <typename T> 
class TreeNode{
public:
    unsigned int count; 
    TreeNode* parent;
    vector <T> keys;
    vector <TreeNode*> childs;
    vector <int> vals;
    int degree;
    TreeNode* nextLeafNode;
    bool isLeaf;

    TreeNode(int degree,bool newLeaf=false);
    ~TreeNode();

    bool isRoot();
    bool search(T key,unsigned int &index);
    TreeNode* splite(T &key);
    unsigned int add(T &key);
    unsigned int add(T &key,int val); 
    //输入：key值对应的index
    //输出：bool
    //功能：在结点中删除index对应的key值
    bool deleteKeyByIndex(unsigned int index);
    //返回下一个相邻叶结点指针
    TreeNode* nextLeaf();
    //输入:起始index,终止key，返回结果vector容器
    //输出:到达终止key返回true，否则返回flase
    //功能：返回一定范围的value容器
    bool findRange(unsigned int index, T& key, vector<int>& vals);
    bool findRange(unsigned int index, vector<int>& vals);
};


//**********************BplusTree***************************//

template <typename T>
class BPlusTree
{
private:
    typedef TreeNode<T>* Tree;

    struct searchNodeParse
    {
        Tree pNode; 
        unsigned int index;
        bool ifFound;
    };
private:
    string fileName;
    Tree root;
    Tree leafHead;
    unsigned int keyCount;
    unsigned int level;
    unsigned int nodeCount;
    // fileNode* file;
    int keySize;
    int degree;
    
public:
    BPlusTree(string m_name,int keySize,int degree);
    ~BPlusTree();

    int search(T& key);
    bool insertKey(T &key,int val);
    bool deleteKey(T &key);
    
    void dropTree(Tree node);
    void searchRange(T &key1, T &key2, std::vector<int>& vals, int type);
    
    void readFromDiskAll();
    void writtenbackToDiskAll();
    void readFromDisk(blockNode* btmp);

private:
    void init_tree();// init the tree
    bool adjustAfterinsert(Tree pNode);
    bool adjustAfterDelete(Tree pNode);
    void findToLeaf(Tree pNode,T key,searchNodeParse &snp);
    void getFile(string filePath);
    int getBlockCount(string tname);
};

/* the implement of BPlusTree function */
//******** The definition of the functions of the class TreeNode **********

//构造函数
template <class T>
TreeNode<T>::TreeNode(int m_degree,bool newLeaf):count(0),parent(NULL),nextLeafNode(NULL),isLeaf(newLeaf),degree(m_degree)
{
    for(unsigned int i = 0;i < degree+1;i ++)
    {
        childs.push_back(NULL);
        keys.push_back(T());
        vals.push_back(int());
    }
    childs.push_back(NULL);
}

//析构函数
template <class T>
TreeNode<T>::~TreeNode() {}

//功能：判断是否为根结点
template <class T>
bool TreeNode<T>::isRoot()
{
    if(parent != NULL) 
        return false;
    else 
        return true;
}

//输入：key值，(index)
//输出：bool
//功能：判断结点内是否存在某key值，并通过index引用返回该值index
//如不存在，则返回最接近的index
//并通过函数返回bool值说明是否找到
template <class T>
bool TreeNode<T>::search(T key,unsigned int &index)
{
    if(count == 0 )  //结点内key数量为0
    {
        index = 0;
        return false;
    }
    else
    {
        //判断key值是否超过本结点内最大值(key不在本结点内)
        if(keys[count-1] < key)
        {
            index = count;
            return false; 
        }
        else if(keys[0] > key) //判断key值是否小于本结点内最小值(key不在本结点内)
        {
            index = 0;
            return false;
        } // end of test
        else if(count <= 20) //结点内key数量较少时直接线性遍历搜索即可
        {
            for(unsigned int i = 0;i < count;i ++)
            {
                if(keys[i] == key)
                {
                    index = i;
                    return true;
                }
                else if(keys[i] < key)
                {
                    continue;
                }
                else if(keys[i] > key)
                {
                    index = i;
                    return false;
                }
            }
        } 
        else if(count > 20)//结点内key数量过多时采用二分搜索
        {
            unsigned int left = 0, right = count - 1, pos = 0;
            while(right>left+1)
            {
                pos = (right + left) / 2;
                if(keys[pos] == key)
                {
                    index = pos;
                    return true;
                }
                else if(keys[pos] < key)
                {
                    left = pos;
                }
                else if(keys[pos] > key)
                {
                    right = pos;
                }
            }
            
            if(keys[left] >= key)
            {
                index = left;
                return (keys[left] == key);
            }
            else if(keys[right] >= key)
            {
                index = right;
                return (keys[right] == key);
            }
            else if(keys[right] < key)
            {
                index = right ++;
                return false;
            }
        }
    }
    return false;
}

//输出：TreeNode指针
//功能：将一个结点分裂成两个结点(与B+树的分裂有关)
//新结点为本结点的下一个结点
//同时功过key引用返回去到上层的key值
template <class T>
TreeNode<T>* TreeNode<T>::splite(T &key)
{
    unsigned int minmumNode = (degree - 1) / 2;//最小结点数量
    TreeNode* newNode = new TreeNode(degree,this->isLeaf);//创建新结点
    
    if(isLeaf) //当前结点为叶结点情况
    {
        key = keys[minmumNode + 1];
        for(unsigned int i = minmumNode + 1;i < degree;i ++) //将右半部分key值拷贝至新结点内
        {
            newNode->keys[i-minmumNode-1] = keys[i];
            keys[i] = T();
            newNode->vals[i-minmumNode-1] = vals[i];
            vals[i] = int();
        }

        newNode->nextLeafNode = this->nextLeafNode; //将新结点放置到本结点右边
        this->nextLeafNode = newNode;
        newNode->parent = this->parent; 

        newNode->count = minmumNode;//调整两结点内key数量
        this->count = minmumNode + 1;
    }
    else if(!isLeaf)//非叶结点情况
    {
        key = keys[minmumNode];
        for(unsigned int i = minmumNode + 1;i < degree+1;i ++)//拷贝子结点指针至新结点
        {
            newNode->childs[i-minmumNode-1] = this->childs[i];
            newNode->childs[i-minmumNode-1]->parent = newNode;
            this->childs[i] = NULL;
        }
        for(unsigned int i = minmumNode + 1;i < degree;i ++)//拷贝key值至新结点
        {
            newNode->keys[i-minmumNode-1] = this->keys[i];
            this->keys[i] = T();
        }
        this->keys[minmumNode] = T();//调整结点相互位置关系
        newNode->parent = this->parent;

        newNode->count = minmumNode;//调整结点内key数量
        this->count = minmumNode;
    }
    return newNode;
}

//输入：key值
//输出：添加的position
//功能：在枝干结点中添加key值，并返回添加的位置
template <class T>
unsigned int TreeNode<T>::add(T &key)
{
    if(count == 0) //本结点内无key
    {
        keys[0] = key;
        count ++;
        return 0;
    }
    else
    {
        unsigned int index = 0; //查找是否Key值已经存在
        bool exist = search(key, index);
        if(exist){}
        else //不存在，可以进行插入
        {
            //调整其他key值
            for(unsigned int i = count;i > index;i --)
                keys[i] = keys[i-1];
            keys[index] = key;
            //调整子结点指针情况
            for(unsigned int i = count + 1;i > index+1;i --)
                childs[i] = childs[i-1];
            childs[index+1] = NULL;
            count ++;
            return index;
        }
    }
}

//输入：key值
//输出：添加的position
//功能：在叶结点中添加key值，并返回添加的位置
template <class T>
unsigned int TreeNode<T>::add(T &key,int val)
{
    if(!isLeaf)
    {
        return -1;
    }
    if(count == 0)//结点内没有key值
    {
        keys[0] = key;
        vals[0] = val;
        count ++;
        return 0;
    }
    else //正常插入
    {
        unsigned int index = 0;
        bool exist = search(key, index);
        if(exist){}
        else //逐个调整key值
        {
            for(unsigned int i = count;i > index;i --)
            {
                keys[i] = keys[i-1];
                vals[i] = vals[i-1];
            }
            keys[index] = key;
            vals[index] = val;
            count ++;
            return index;
        }
    }
}

//输入：key值对应的index
//输出：bool
//功能：在结点中删除index对应的key值
template <class T>
bool TreeNode<T>::deleteKeyByIndex(unsigned int index)
{
    if(index > count) {//index错误，超过本结点范围
        return false;
    } else { //正常进行删除
        if (isLeaf) { //叶结点情况
			//逐个调整key值
            for (unsigned int i = index; i < count-1; i++) {
                keys[i] = keys[i+1];
                vals[i] = vals[i+1];
            }
            keys[count-1] = T();
            vals[count-1] = int();
        } else { //枝干结点情况
			//调整key值
            for(unsigned int i = index; i < count-1; i++)
                keys[i] = keys[i+1];

			//调整子结点指针
            for(unsigned int i = index+1;i < count;i ++)
                childs[i] = childs[i+1];

            keys[count-1] = T();
            childs[count] = NULL;
        }
        count--;
        return true;
    }
    return false;
}

//返回下一个相邻叶结点指针
template <class T>
TreeNode<T>* TreeNode<T>::nextLeaf()
{
	return nextLeafNode;
}

//输入，起始index,终止key，返回结果vector容器
//输出:到达终止key返回true，否则返回flase
//功能：返回一定范围的value容器
template <class T>
bool TreeNode<T>::findRange(unsigned int index, T& key, vector<int>& valsout)
{
	unsigned int i;
	for (i = index; i < count && keys[i] <= key; i++)
		valsout.push_back(vals[i]);

	if (keys[i] >= key)
		return true;
	else
		return false;
}

//输入，起始index,终止key，返回结果vector容器
//输出:到达终止key返回true，否则返回flase
//功能：返回一定范围的value容器
template <class T>
bool TreeNode<T>::findRange(unsigned int index, vector<int>& valsout)
{
	unsigned int i;
	for (i = index; i < count; i++)
		valsout.push_back(vals[i]);

	return false;
}

//******** The definition of the functions of the class BPlusTree **********

//构造函数
//用于构造一颗新的树，确定m_name,key的size，树的度
//初始化各个变量
//同时调用其他函数为本树分配内存
template <class T>
BPlusTree<T>::BPlusTree(string m_name,int keysize,int m_degree):fileName(m_name),keyCount(0),level(0),nodeCount(0),root(NULL),leafHead(NULL),keySize(keysize),file(NULL),degree(m_degree)
{
    init_tree();
    readFromDiskAll();
}

//析构函数
//释放相应的内存
template <class T>
BPlusTree<T>:: ~BPlusTree()
{
    dropTree(root);
    keyCount = 0;
    root = NULL;
    level = 0;
}

//初始化B+树，并分配内存空间
template <class T>
void BPlusTree<T>::init_tree()
{
    root = new TreeNode<T>(degree,true);
    keyCount = 0;
    level = 1;
    nodeCount = 1;
    leafHead = root;
}

//用于查找某key值所处的叶结点位置
template <class T>
void BPlusTree<T>::findToLeaf(Tree pNode,T key,searchNodeParse& snp)
{
    unsigned int index = 0;
    if(pNode->search(key,index)) //在对应结点内查找key值
    {
        if(pNode->isLeaf)//若此结点是叶结点，则查找成功
        {
            snp.pNode = pNode;
            snp.index = index;
            snp.ifFound = true;
        }
        else //此结点不是子结点，查找它的下一层
        {
            pNode = pNode -> childs[index + 1];
            while(!pNode->isLeaf)
            {
                pNode = pNode->childs[0];
            }
            //因为已找到key值，所以其最底层叶结点index[0]即为该key
            snp.pNode = pNode;
            snp.index = 0;
            snp.ifFound = true;
        }
        
    }
    else //本结点内未找到该key
    {
        //若此结点已经是叶结点则查找失败
        if(pNode->isLeaf)
        {
            snp.pNode = pNode;
            snp.index = index;
            snp.ifFound = false;
        }
        else//递归寻找下一层
        {
            findToLeaf(pNode->childs[index],key,snp);
        }
    }
}

//输入：key值及其value
//输出：bool
//功能：在树中插入一个key值
//返回是否插入成功
template <class T>
bool BPlusTree<T>::insertKey(T &key,int val)
{
    searchNodeParse snp;
    if(!root) init_tree();//根结点不存在
    findToLeaf(root,key,snp);//查找插入值是否存在
    if(snp.ifFound)//已存在
    {
        cout << "插入key失败" << key <<endl;
        return false;
    }
    else//不存在
    {
        snp.pNode->add(key,val);
        //插入后结点满，需要进行调整
        if(snp.pNode->count == degree)
        {
            adjustAfterinsert(snp.pNode);
        }
        keyCount ++;
        return true;
    }
}

//用于插入某key后若结点满，对整棵树进行调整
template <class T>
bool BPlusTree<T>::adjustAfterinsert(Tree pNode)
{
    T key;
    Tree newNode = pNode->splite(key);
    nodeCount ++;
    
    if(pNode->isRoot()) //当前结点为根结点情况
    {
        Tree root = new TreeNode<T>(degree,false);
        if(root == NULL)
        {
            cout << "不能为新树分配空间" << endl;
            exit(1);
        }
        else
        {
            level ++;
            nodeCount ++;
            this->root = root;
            pNode->parent = root;
            newNode->parent = root;
            root->add(key);
            root->childs[0] = pNode;
            root->childs[1] = newNode;
            return true;
        }
    }
    else //当前结点非根结点
    {
        Tree parent = pNode->parent;
        unsigned int index = parent->add(key);
        
        parent->childs[index+1] = newNode;
        newNode->parent = parent;
        //递归进行调整
        if(parent->count == degree)
            return adjustAfterinsert(parent);
        
        return true;
    }
}

//输入：key值
//输出：key所在位置
//功能：查找
template <class T>
int BPlusTree<T>::search(T& key)
{
    if(!root) return -1;
    searchNodeParse snp;
    findToLeaf(root, key, snp);
    if(!snp.ifFound)
    {
        return -1;
    }
    else
    {
        return snp.pNode->vals[snp.index];
    }
    
}

//输入：key值
//输出：bool
//功能：在树中删除一个key值
//返回是否删除成功
template <class T>
bool BPlusTree<T>::deleteKey(T &key)
{
    searchNodeParse snp;
    if(!root)//根结点不存在
    {
        cout << "删除失败，没有node" << fileName << endl;
        return false;
    }
    else
    {
        findToLeaf(root, key, snp);//查找位置
        if(!snp.ifFound)//找不到该key
        {
            cout << "删除失败，没有key" << fileName << endl;
            return false;
        }
        else //正常找到进行删除
        {
            if(snp.pNode->isRoot())//当前为根结点
            {
                snp.pNode->deleteKeyByIndex(snp.index);
                keyCount --;
                return adjustAfterDelete(snp.pNode);
            }
            else
            {
                if(snp.index == 0 && leafHead != snp.pNode) //key存在于枝干结点上
                {
                    //到上一层去更新枝干层
                    unsigned int index = 0;
                    
                    Tree now_parent = snp.pNode->parent;
                    bool if_found_inBranch = now_parent->search(key,index);
                    while(!if_found_inBranch)
                    {
                        if(now_parent->parent)
                            now_parent = now_parent->parent;
                        else
                        {
                            break;
                        }
                        if_found_inBranch = now_parent->search(key,index);
                    }
                    
                    now_parent -> keys[index] = snp.pNode->keys[1];
                    
                    snp.pNode->deleteKeyByIndex(snp.index);
                    keyCount--;
                    return adjustAfterDelete(snp.pNode);
                    
                }
                else //同时必然存在于叶结点
                {
                    snp.pNode->deleteKeyByIndex(snp.index);
                    keyCount--;
                    return adjustAfterDelete(snp.pNode);
                }
            }
        }
    }
}

//用于删除某key后可能出现结点空情况，对整棵树进行调整
template <class T>
bool BPlusTree<T>::adjustAfterDelete(Tree pNode)
{
    unsigned int minmumKey = (degree - 1) / 2;
    //三种不需要调整的情况
    if(((pNode->isLeaf)&&(pNode->count >= minmumKey)) || ((degree != 3)&&(!pNode->isLeaf)&&(pNode->count >= minmumKey - 1)) || ((degree ==3)&&(!pNode->isLeaf)&&(pNode->count < 0))) // do not need to adjust
    {
        return  true;
    }
    if(pNode->isRoot())//当前结点为根结点
    {
        if(pNode->count > 0) //不需要调整
        {
            return true;
        }
        else //正常需要调整
        {
            if(root->isLeaf) //将成为空树情况
            {
                delete pNode;
                root = NULL;
                leafHead = NULL;
                level --;
                nodeCount --;
            }
            else //根节点将成为左头部
            {
                root = pNode -> childs[0];
                root -> parent = NULL;
                delete pNode;
                level --;
                nodeCount --;
            }
        }
    }
    else //非根节点情况
    {
        Tree parent = pNode->parent,brother = NULL;
        if(pNode->isLeaf)//当前为叶节点
        {
            unsigned int index = 0;
            parent->search(pNode->keys[0],index);
            //选择左兄弟
            if((parent->childs[0] != pNode) && (index + 1 == parent->count))
            {
                brother = parent->childs[index];
                if(brother->count > minmumKey)
                {
                    for(unsigned int i = pNode->count;i > 0;i --)
                    {
                        pNode->keys[i] = pNode->keys[i-1];
                        pNode->vals[i] = pNode->vals[i-1];
                    }
                    pNode->keys[0] = brother->keys[brother->count-1];
                    pNode->vals[0] = brother->vals[brother->count-1];
                    brother->deleteKeyByIndex(brother->count-1);
                    
                    pNode->count ++;
                    parent->keys[index] = pNode->keys[0];
                    return true;
                    
                }
                else
                {
                    parent->deleteKeyByIndex(index);
                    
                    for(int i = 0;i < pNode->count;i ++)
                    {
                        brother->keys[i+brother->count] = pNode->keys[i];
                        brother->vals[i+brother->count] = pNode->vals[i];
                    }
                    brother->count += pNode->count;
                    brother->nextLeafNode = pNode->nextLeafNode;
                    
                    delete pNode;
                    nodeCount --;
                    
                    return adjustAfterDelete(parent);
                }
            }
            else
            {
                if(parent->childs[0] == pNode)
                    brother = parent->childs[1];
                else
                    brother = parent->childs[index+2];
                if(brother->count > minmumKey)
                {
                    pNode->keys[pNode->count] = brother->keys[0];
                    pNode->vals[pNode->count] = brother->vals[0];
                    pNode->count ++;
                    brother->deleteKeyByIndex(0);
                    if(parent->childs[0] == pNode)
                        parent->keys[0] = brother->keys[0];
                    else
                        parent->keys[index+1] = brother->keys[0];
                    return true;
                    
                }
                else
                {
                    for(int i = 0;i < brother->count;i ++)
                    {
                        pNode->keys[pNode->count+i] = brother->keys[i];
                        pNode->vals[pNode->count+i] = brother->vals[i];
                    }
                    if(pNode == parent->childs[0])
                        parent->deleteKeyByIndex(0);
                    else
                        parent->deleteKeyByIndex(index+1);
                    pNode->count += brother->count;
                    pNode->nextLeafNode = brother->nextLeafNode;
                    delete brother;
                    nodeCount --;
                    
                    return adjustAfterDelete(parent);
                }
            }
            
        }
        else//枝干节点情况
        {
            unsigned int index = 0;
            parent->search(pNode->childs[0]->keys[0],index);
            if((parent->childs[0] != pNode) && (index + 1 == parent->count)) 
            {
                brother = parent->childs[index];
                if(brother->count > minmumKey - 1)
                {
                    pNode->childs[pNode->count+1] = pNode->childs[pNode->count];
                    for(unsigned int i = pNode->count;i > 0;i --)
                    {
                        pNode->childs[i] = pNode->childs[i-1];
                        pNode->keys[i] = pNode->keys[i-1];
                    }
                    pNode->childs[0] = brother->childs[brother->count];
                    pNode->keys[0] = parent->keys[index];
                    pNode->count ++;
                    parent->keys[index]= brother->keys[brother->count-1];
                    if(brother->childs[brother->count])
                    {
                        brother->childs[brother->count]->parent = pNode;
                    }
                    brother->deleteKeyByIndex(brother->count-1);
                    
                    return true;
                    
                }
                else
                {
                    brother->keys[brother->count] = parent->keys[index];
                    parent->deleteKeyByIndex(index);
                    brother->count ++;
                    
                    for(int i = 0;i < pNode->count;i ++)
                    {
                        brother->childs[brother->count+i] = pNode->childs[i];
                        brother->keys[brother->count+i] = pNode->keys[i];
                        brother->childs[brother->count+i]-> parent= brother;
                    }
                    brother->childs[brother->count+pNode->count] = pNode->childs[pNode->count];
                    brother->childs[brother->count+pNode->count]->parent = brother;
                    
                    brother->count += pNode->count;
                    
                    
                    delete pNode;
                    nodeCount --;
                    
                    return adjustAfterDelete(parent);
                }
                
            }
            else
            {
                if(parent->childs[0] == pNode)
                    brother = parent->childs[1];
                else
                    brother = parent->childs[index+2];
                if(brother->count > minmumKey - 1)
                {
                    pNode->childs[pNode->count+1] = brother->childs[0];
                    pNode->keys[pNode->count] = brother->keys[0];
                    pNode->childs[pNode->count+1]->parent = pNode;
                    pNode->count ++;
                    if(pNode == parent->childs[0])
                        parent->keys[0] = brother->keys[0];
                    else
                        parent->keys[index+1] = brother->keys[0];
                    brother->childs[0] = brother->childs[1];
                    brother->deleteKeyByIndex(0);
                    
                    return true;
                }
                else
                {
                    pNode->keys[pNode->count] = parent->keys[index];
                    
                    if(pNode == parent->childs[0])
                        parent->deleteKeyByIndex(0);
                    else
                        parent->deleteKeyByIndex(index+1);
                    
                    pNode->count ++;
                    
                    for(int i = 0;i < brother->count;i++)
                    {
                        pNode->childs[pNode->count+i] = brother->childs[i];
                        pNode->keys[pNode->count+i] = brother->keys[i];
                        pNode->childs[pNode->count+i]->parent = pNode;
                    }
                    pNode->childs[pNode->count+brother->count] = brother->childs[brother->count];
                    pNode->childs[pNode->count+brother->count]->parent = pNode;
                    
                    pNode->count += brother->count;
                    
                    
                    delete brother;
                    nodeCount --;
                    
                    return adjustAfterDelete(parent);
                    
                }
                
            }
            
        }
        
    }
    return false;
}

//输入：树的根结点
//功能：删除整棵树并释放内存空间，主要用在析构函数中
template <class T>
void BPlusTree<T>::dropTree(Tree node)
{
    if(!node) return;
    if(!node->isLeaf) //if it has child
    {
        for(unsigned int i=0;i <= node->count;i++)
        {
            dropTree(node->childs[i] );
            node->childs[i] = NULL;
        }
    }
    delete node;
    nodeCount --;
    return;
}

//输入：key1，key2，返回vals的容器
//功能：返回范围搜索结果，将value放入vals容器中
template <class T>
void BPlusTree<T>::searchRange(T& key1, T& key2, std::vector<int>& vals, int tag)
{
	//空树
	if (!root)
		return;

	if (tag == 2) {
		searchNodeParse snp1;
		findToLeaf(root, key1, snp1);

		bool finished = false;
		Tree pNode = snp1.pNode;
		unsigned int index = snp1.index;

		do {
			finished = pNode->findRange(index, vals);
			index = 0;
			if (pNode->nextLeafNode == NULL)
				break;
			else
				pNode = pNode->nextLeaf();
		} while (!finished);
	} else if (tag == 1) {
		searchNodeParse snp2;
		findToLeaf(root, key2, snp2);

		bool finished = false;
		Tree pNode = snp2.pNode;
		unsigned int index = snp2.index;

		do {
			finished = pNode->findRange(index, vals);
			index = 0;
			if (pNode->nextLeafNode == NULL)
				break;
			else
				pNode = pNode->nextLeaf();
		} while (!finished);
	} else {
		searchNodeParse snp1, snp2;
	    findToLeaf(root, key1, snp1);
		findToLeaf(root, key2, snp2);
		bool finished = false;
		unsigned int index;

		if (key1 <= key2) {
			Tree pNode = snp1.pNode;
			index = snp1.index;
			do {
				finished = pNode->findRange(index, key2, vals);
				index = 0;
				if (pNode->nextLeafNode == NULL)
					break;
				else
					pNode = pNode->nextLeaf();
			} while (!finished);
		} else {
			Tree pNode = snp2.pNode;
			index = snp2.index;
			do {
				finished = pNode->findRange(index, key1, vals);
				index = 0;
				if (pNode->nextLeafNode == NULL)
					break;
				else
					pNode = pNode->nextLeaf();
			} while (!finished);
		}
	}

	sort(vals.begin(),vals.end());
    vals.erase(unique(vals.begin(), vals.end()), vals.end());
	return;
}

//获取文件大小
template <class T>
void BPlusTree<T>::getFile(string fileName) {
    FILE* f = fopen(fileName.c_str() , "r");
    if (f == NULL) {
        f = fopen(fileName.c_str(), "w+");
        fclose(f);
        f = fopen(fileName.c_str() , "r");
    }
    fclose(f);
    return;
}

template <class T>
int BPlusTree<T>::getBlockCount(string tname)
{
    char* p;
    int blockcount = -1;
    do {
        p = buffer_manager.getPage(tname , blockcount + 1);
        blockcount++;
    } while(p[0] != '\0');
    return blockcount;
}

template <class T>
void BPlusTree<T>::readFromDiskAll()
{
    file = bm.getFile(fileName.c_str());
    blockNode* btmp = bm.getBlockHead(file);
    while (true)
    {
        if (btmp == NULL)
        {
            return;
        }
        
        readFromDisk(btmp);
        if(btmp->ifbottom) break;
        btmp = bm.getNextBlock(file, btmp);
    }
    
}

template <class T>
void BPlusTree<T>::readFromDisk(blockNode* btmp)
{
    int valueSize = sizeof(int);
    char* indexBegin = bm.get_content(*btmp);
    char* valueBegin = indexBegin + keySize;
    T key;
    int value;
    
    while(valueBegin - bm.get_content(*btmp) < bm.get_usingSize(*btmp))
    {
        key = *(T*)indexBegin;
        value = *(int*)valueBegin;
        insertKey(key, value);
        valueBegin += keySize + valueSize;
        indexBegin += keySize + valueSize;
    }
    
}

template <class T>
void BPlusTree<T>::writtenbackToDiskAll()
{
    blockNode* btmp = bm.getBlockHead(file);
    Tree ntmp = leafHead;
    int valueSize = sizeof(int);
    while(ntmp != NULL)
    {
        bm.set_usingSize(*btmp, 0);
        bm.set_dirty(*btmp);
        for(int i = 0;i < ntmp->count;i ++)
        {
            char* key = (char*)&(ntmp->keys[i]);
            char* value = (char*)&(ntmp->vals[i]);
            memcpy(bm.get_content(*btmp)+bm.get_usingSize(*btmp),key,keySize);
            bm.set_usingSize(*btmp, bm.get_usingSize(*btmp) + keySize);
            memcpy(bm.get_content(*btmp)+bm.get_usingSize(*btmp),value,valueSize);
            bm.set_usingSize(*btmp, bm.get_usingSize(*btmp) + valueSize);
        }
        
        btmp = bm.getNextBlock(file, btmp);
        ntmp = ntmp->nextLeafNode;
    }
    while(1)
    {
        if(btmp->ifbottom)
            break;
        bm.set_usingSize(*btmp, 0);
        bm.set_dirty(*btmp);
        btmp = bm.getNextBlock(file, btmp);
    }
}

#endif