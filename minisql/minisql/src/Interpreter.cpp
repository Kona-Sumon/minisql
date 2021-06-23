#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
using namespace std;

#include "Interpreter.h"


/*
getOneWord����������ȡһ������
�����һ������result
*/
string Interpreter::getOneWord()
{
	string result;	//result�����洢���ص��ַ���
	int begin, end;	//begin��ʾ��ʼ�±�, end��ʾ��ֹ�ĺ�һ���±�
	while( ( s[temp] == ' ' || s[temp] == '\n' || s[temp] == '\t' ) && s[temp] != 0 )
		//�������������result��ʼλ��
	{
		(temp)++;
	}
	begin = temp;
	if( s[temp] == '(' || s[temp] == ',' || s[temp] == ')' )
	{
		(temp)++;
		end = temp;
		result = s.substr(begin, end - begin);//��ȡ�����ַ���
		return result;
	}
	else if( s[temp] == 39 )//������
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
		while( s[temp] != ' ' && s[temp] != '(' && s[temp] != 10 && s[temp] != 0 && s[temp] != ')' && s[temp] != ',' )//һ������������ȡ����ĩβ��Ӧ�±�
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
change���������ı����Ϊ�ڲ��߼�
�����0��ʾ���﷨����1��ʾ��ȷת����-1��ʾ�˳�, 2��ʾִ�нű��ļ�
*/
int Interpreter::change()
{
	map<string, int> hash1, hash2, hash3;//������¼��ϣֵ

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
					string primaryKey = "";//���� 
					string tableName = "";//����
					word = getOneWord();
					if (!word.empty())
						tableName = word;
					else
					{
						cout << "Syntax Error for no table name" << endl;//����,û�б��� 
						return 0;
					}
					word = getOneWord();//��ȡ��һ������ 
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
							string attributeName = word;//��ȡ������
							bool ifUnique = false;//Ĭ��Ϊ����Unique 
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
							if (word == "unique")//��Ϊunique
							{
								ifUnique = true;
								word = getOneWord();
							}
							Attribute attr(attributeName, type, ifUnique);
							attributeList.push_back(attr);//�����Զ�������������� 
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
						if (word == "primary")//����primaryKey
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
										if (word == attributeList[i].name)//��������Ϊ����,������Ϊunique
										{
											attributeList[i].ifUnique = true;
											break;//��¼����λ��
										}
									}
									if (i == attributeList.size())//�Ҳ�����������
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
						//ap->tableCreate(tableName, &attributeList, primaryKey, primaryKeyLocation);//��ȡ��Ӧ��Ϣ�����API����������
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
						//ap->indexCreate(indexName, tableName, attributeName);//����API������������
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
		}//create��䲿��

		case 1://drop
		{
			word = getOneWord();
			if (word == "table")//ɾ����
			{
				word = getOneWord();
				if (!word.empty())
				{
					//ap->tableDrop(word);//����API����ɾ����
					return 1;
				}
				else
				{
					cout << "Syntax Error for no table name" << endl;
					return 0;
				}
			}

			else if (word == "index")//ɾ������
			{
				word = getOneWord();
				if (!word.empty())
				{
					//ap->indexDrop(word);//����API����ɾ������
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
		}//drop��䲿��

		case 2://select
		{
			vector<string> seletedAttributes;
			string tableName;
			word = getOneWord();
			if (word != "*")//һ��select���
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
			else//select*���
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
			if (word.empty())//�󲻸�where����
			{
				if (seletedAttributes.size() == 0) {//select*�����ʾ��������Ϣ
					//ap->recordShow(tableName);//����API������ʾ
				}
				else 
					//ap->recordShow(tableName, &seletedAttributes);//��ʾ���й����������������Ե���Ϣ
				return 1;
			}
			else if (word == "where")//���where����
			{
				string attributeName;
				string value;//���Ƚ�ֵ
				int operate;
				vector< Where > WhereVector;//Where�����б�
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
						value = word;//���Ƚ�ֵ
						Where c(attributeName, value, operate);//��ʼ��Where����
						WhereVector.push_back(c);//Where����洢��Where������
						word = getOneWord();
						if (word.empty())
							break;
						if (word != "and")
							throw SyntaxException();
						word = getOneWord();//��ȡ��һ������
					}
					catch (SyntaxException&)
					{
						cout << "Syntax Error" << endl;
						return 0;
					}
				}
				if (seletedAttributes.size() == 0)//select*������ѯ
				{
					//ap->recordShow(tableName, NULL, &WhereVector);
				}
				else
					//ap->recordShow(tableName, &seletedAttributes, &WhereVector);//�ض�����select������ѯ
				return 1;
			}
			break;
		}//select��䲿��

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
				while (!word.empty() && word != ")")//��ȡ����ֵ
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
			//ap->recordInsert(tableName, &values);//����API��������һ����¼
			return 1;
			break;
		}//insert��䲿��

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
			if (word.empty())//ɾ��������м�¼
			{
				//ap->recordDelete(tableName);//����API����
				return 1;
			}
			else if (word== "where")//����ѡ��
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
						if (word.empty()) //�������Ƚ�
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
				//ap->recordDelete(tableName, &WhereVector);//����API����ɾ�����������ļ�¼
				return 1;
				break;
			}
		}//delete��䲿��


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


//������
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