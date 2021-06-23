#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
using namespace std;

#include "Interpreter.h"


/*
getOneWord函数用来获取一个单词
输出：一个单词result
*/
string Interpreter::getOneWord()
{
	string result;	//result用来存储返回的字符串
	int begin, end;	//begin表示开始下标, end表示终止的后一个下标
	while( ( s[temp] == ' ' || s[temp] == '\n' || s[temp] == '\t' ) && s[temp] != 0 )
		//处理到所需的正常result开始位置
	{
		(temp)++;
	}
	begin = temp;
	if( s[temp] == '(' || s[temp] == ',' || s[temp] == ')' )
	{
		(temp)++;
		end = temp;
		result = s.substr(begin, end - begin);//获取单词字符串
		return result;
	}
	else if( s[temp] == 39 )//单引号
	{
		(temp)++;
		while( s[temp] != 39 && s[temp] != 0 )
			(temp)++;
		if( s[temp] == 39 )
		{
			begin++;
			end = temp;
			(temp)++;
			result = s.substr(begin, end - begin);
			return result;
		}
		else
		{
			result = "";
			return result;
		}
	}
	else
	{
		while( s[temp] != ' ' && s[temp] != '(' && s[temp] != 10 && s[temp] != 0 && s[temp] != ')' && s[temp] != ',' )//一般正常遍历获取单词末尾对应下标
			(temp)++;
		end = temp;
		if( begin != end )
			result = s.substr( begin, end - begin );
		else
			result = "";
		return result;
	}
}

/*
change函数用来改变语句为内部逻辑
输出：0表示有语法错误，1表示正确转换，-1表示退出, 2表示执行脚本文件
*/
int Interpreter::change()
{
	map<string, int> hash1, hash2, hash3;//用来记录哈希值

	hash1["create"] = 0;
	hash1["drop"] = 1;
	hash1["select"] = 2;
	hash1["insert"] = 3;
	hash1["delete"] = 4;
	hash1["quit"] = 5;
	hash1["execfile"] = 6;

	hash2["table"] = 0;
	hash2["index"] = 1;

	string word;
	word = getOneWord();
	if (hash1.count(word) != 0)
	{
		switch (hash1[word])
		{
		case 0://create
		{
			word = getOneWord();
			if (hash2.count(word) != 0)
			{
				switch (hash2[word])
				{
				case 0://create table
				{
					string primaryKey = "";//主键 
					string tableName = "";//表名
					word = getOneWord();
					if (!word.empty())
						tableName = word;
					else
					{
						cout << "Syntax Error for no table name" << endl;//报错,没有表名 
						return 0;
					}
					word = getOneWord();//获取下一个单词 
					if (word.empty() || word != "(")
					{
						cout << "Syntax Error for expected '('" << endl;
						return 0;
					}
					else
					{
						word = getOneWord();
						vector<Attribute> attributeList;
						while (!word.empty() && word != "primary" && word != ")")
						{
							int type;
							string attributeName = word;//获取属性名
							bool ifUnique = false;//默认为不是Unique 
							word = getOneWord();
							if (word == "int")
								type = 0;
							else if (word == "float")
								type = -1;
							else if (word == "char")
							{
								word = getOneWord();
								if (word != "(")
								{
									cout << "Syntax Error for unknown data type" << endl;
									return 0;
								}
								word = getOneWord();
								istringstream stringtoInt(word);
								if (!(stringtoInt >> type))
								{
									cout << "Syntax Error for illegal number in char()" << endl;
									return 0;
								}
								word = getOneWord();
								if (word != ")")
								{
									cout << "Syntax Error for unknown data type" << endl;
									return 0;
								}
							}
							else
							{
								cout << "Syntax Error for unknown or missing data type" << endl;
								return 0;
							}
							word = getOneWord();
							if (word == "unique")//若为unique
							{
								ifUnique = true;
								word = getOneWord();
							}
							Attribute attr(attributeName, type, ifUnique);
							attributeList.push_back(attr);//将属性对象放入属性向量 
							if (word != ",")
							{
								if (word != ")")
								{
									cout << "Syntax Error for ','" << endl;
									return 0;
								}
								else break;
							}
							word = getOneWord();
						}
						int primaryKeyLocation = 0;
						if (word == "primary")//处理primaryKey
						{
							word = getOneWord();
							if (word != "key")
							{
								cout << "Syntax Error for \"primary key\"" << endl;
								return 0;
							}
							else
							{
								word = getOneWord();
								if (word == "(")
								{
									word = getOneWord();
									int i;
									for (i = 0; i < attributeList.size(); i++)
									{
										if (word == attributeList[i].name)//若该属性为主键,则设置为unique
										{
											attributeList[i].ifUnique = true;
											break;//记录主键位置
										}
									}
									if (i == attributeList.size())//找不到主键报错
									{
										cout << "Syntax Error for primary key does not exist in attributes" << endl;
										return 0;
									}
									primaryKeyLocation = i;
									word = getOneWord();
									if (word != ")")
									{
										cout << "Syntax Error for expected ')'" << endl;
										return 0;
									}
								}
								else
								{
									cout << "Syntax Error for expected ')'" << endl;
									return 0;
								}
								word = getOneWord();
								if (word != ")")
								{
									cout << "Syntax Error for expected ')'" << endl;
									return 0;
								}
							}
						}
						else if (word.empty())
						{
							cout << "Syntax Error for expected ')'" << endl;
							return 0;
						}
						else if (primaryKeyLocation == -1)
						{
							cout << "Syntax Error for no primary key" << endl;
							return 0;
						}
						//ap->tableCreate(tableName, &attributeList, primaryKey, primaryKeyLocation);//获取相应信息后调用API函数创建表
						return 1;
					}
					break;
				}
				case 1://create index
				{
					string indexName = "";
					string tableName = "";
					string attributeName = "";
					word = getOneWord();
					if (!word.empty())
						indexName = word;
					else
					{
						cout << "Syntax Error for no index name" << endl;
						return 0;
					}
					word = getOneWord();
					try
					{
						if (word.empty() || word != "on")
							throw SyntaxException();
						word = getOneWord();
						if (word.empty())
							throw SyntaxException();
						tableName = word;
						word = getOneWord();
						if (word.empty() || word != "(")
							throw SyntaxException();
						word = getOneWord();
						if (word.empty())
							throw SyntaxException();
						attributeName = word;
						word = getOneWord();
						if (word.empty() || word != ")")
							throw SyntaxException();
						//ap->indexCreate(indexName, tableName, attributeName);//调用API函数创建索引
						return 1;
					}
					catch (SyntaxException&)
					{
						cout << "Syntax Error" << endl;
						return 0;
					}
					break;
				}
				}
			}
			else
			{
				cout << "Syntax Error for create " << word << endl;
				return 0;
			}
			break;
		}//create语句部分

		case 1://drop
		{
			word = getOneWord();
			if (word == "table")//删除表
			{
				word = getOneWord();
				if (!word.empty())
				{
					//ap->tableDrop(word);//调用API函数删除表
					return 1;
				}
				else
				{
					cout << "Syntax Error for no table name" << endl;
					return 0;
				}
			}

			else if (word == "index")//删除索引
			{
				word = getOneWord();
				if (!word.empty())
				{
					//ap->indexDrop(word);//调用API函数删除索引
					return 1;
				}
				else
				{
					cout << "Syntax Error for no index name" << endl;
					return 1;
				}
			}
			else
			{
				cout << "Syntax Error for unknown or no drop type" << endl;
				return 0;
			}
			break;
		}//drop语句部分

		case 2://select
		{
			vector<string> seletedAttributes;
			string tableName;
			word = getOneWord();
			if (word != "*")//一般select语句
			{
				while (word != "from" && word != ";")
				{
					seletedAttributes.push_back(word);
					word = getOneWord();
				}
				if (word == ";")
				{
					cout << "Syntax Error for no 'from'" << endl;
					return 0;
				}
			}
			else//select*语句
			{
				word = getOneWord();
			}
			if (word != "from")
			{
				cout << "Syntax Error for no 'from'" << endl;
				return 0;
			}
			word = getOneWord();
			if (!word.empty())
				tableName = word;
			else
			{
				cout << "Syntax Error for no table name" << endl;
				return 0;
			}
			word = getOneWord();
			if (word.empty())//后不跟where条件
			{
				if (seletedAttributes.size() == 0) {//select*语句显示表所有信息
					//ap->recordShow(tableName);//调用API函数显示
				}
				else 
					//ap->recordShow(tableName, &seletedAttributes);//显示表中关于属性向量里属性的信息
				return 1;
			}
			else if (word == "where")//后跟where条件
			{
				string attributeName;
				string value;//被比较值
				int operate;
				vector< Where > WhereVector;//Where对象列表
				word = getOneWord();
				while (1)
				{
					try
					{
						if (word.empty())
							throw SyntaxException();
						attributeName = word;
						word = getOneWord();
						if (word == "=")
							operate = 0;
						else if (word == "<>")
							operate = 1;
						else if (word == "<")
							operate = 2;
						else if (word == ">")
							operate = 3;
						else if (word == "<=")
							operate = 4;
						else if (word == ">=")
							operate = 5;
						else
							throw SyntaxException();
						word = getOneWord();
						if (word.empty())
							throw SyntaxException();
						value = word;//被比较值
						Where c(attributeName, value, operate);//初始化Where对象
						WhereVector.push_back(c);//Where对象存储在Where向量里
						word = getOneWord();
						if (word.empty())
							break;
						if (word != "and")
							throw SyntaxException();
						word = getOneWord();//获取下一个条件
					}
					catch (SyntaxException&)
					{
						cout << "Syntax Error" << endl;
						return 0;
					}
				}
				if (seletedAttributes.size() == 0)//select*条件查询
				{
					//ap->recordShow(tableName, NULL, &WhereVector);
				}
				else
					//ap->recordShow(tableName, &seletedAttributes, &WhereVector);//特定属性select条件查询
				return 1;
			}
			break;
		}//select语句部分

		case 3://insert
		{
			string tableName;
			vector<string> values;
			word = getOneWord();
			try
			{
				if (word != "into")
					throw SyntaxException();
				word = getOneWord();
				if (word.empty())
					throw SyntaxException();
				tableName = word;
				word = getOneWord();
				if (word!= "values")
					throw SyntaxException();
				word = getOneWord();
				if (word!= "(")
					throw SyntaxException();
				word = getOneWord();
				while (!word.empty() && word != ")")//获取属性值
				{
					values.push_back(word);
					word = getOneWord();
					if (word == ",")
						word = getOneWord();
				}
				if (word != ")")
					throw SyntaxException();
			}
			catch (SyntaxException&)
			{
				cout << "Syntax Error" << endl;
				return 0;
			}
			//ap->recordInsert(tableName, &values);//调用API函数插入一条记录
			return 1;
			break;
		}//insert语句部分

		case 4://delete
		{
			string tableName;
			word = getOneWord();
			if (word != "from")
			{
				cout << "Syntax Error for expected 'from'" << endl;
				return 0;
			}
			word = getOneWord();
			if (!word.empty())
				tableName = word;
			else
			{
				cout << "Syntax Error for expected table name" << endl;
				return 0;
			}

			word = getOneWord();
			if (word.empty())//删除表的所有记录
			{
				//ap->recordDelete(tableName);//调用API函数
				return 1;
			}
			else if (word== "where")//条件选择
			{
				string attributeName;
				string value;
				int operate;
				vector<Where> WhereVector;
				word = getOneWord();
				while (1) {
					try {
						if (word.empty())
							throw SyntaxException();
						attributeName = word;
						word = getOneWord();
						if (word == "=")
							operate = 0;
						else if (word == "<>")
							operate = 1;
						else if (word == "<")
							operate = 2;
						else if (word == ">")
							operate = 3;
						else if (word == "<=")
							operate = 4;
						else if (word == ">=")
							operate = 5;
						else
							throw SyntaxException();
						word = getOneWord();
						if (word.empty())
							throw SyntaxException();
						value = word;
						word = getOneWord();
						Where c(attributeName, value, operate);
						WhereVector.push_back(c);
						if (word.empty()) //单条件比较
							break;
						if (word != "and")
							throw SyntaxException();
						word = getOneWord();
					}
					catch (SyntaxException&)
					{
						cout << "Syntax Error" << endl;
						return 0;
					}
				}
				//ap->recordDelete(tableName, &WhereVector);//调用API函数删除符合条件的记录
				return 1;
				break;
			}
		}//delete语句部分


		case 5:
		{
			return -1;
			break;
		}

		case 6:
		{
			word = getOneWord();
			if (word.empty())
				return 0;
			file = word;
			cout << "Open the " << file << "file" << endl;
			return 2;
			break;
		}
		}
	}
	else
	{
		if (word != "")
			cout << "Error, command " << word << " not found" << endl;
		return 0;
	}
}

template<class T>
bool Where::judge(T data)
{
	stringstream ss;
	ss << value;
	T mydata;
	ss >> mydata;
	switch (operate)
	{
	case 0:
		return data == mydata;
		break;
	case 1:
		return data != mydata;
		break;
	case 2:
		return data < mydata; 
		break;
	case 3:
		return data > mydata;
		break;
	case 4:
		return data <= mydata; 
		break;
	case 5:
		return data >= mydata;
		break;
	default:
		cout << "No operater" << endl;
		return false;
		break;
	}
}


//测试用
int main()
{
	string str, word;
	getline(cin, str);
	Interpreter i(str);
	word = i.getOneWord();
	cout << word << endl;
	while (!word.empty())
	{
		word = i.getOneWord();
		cout << word << endl;
	}
}