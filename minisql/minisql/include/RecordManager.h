#ifndef __RECORD_MANAGER_H
#define __RECORD_MANAGER_H

#include <exception>
#include <string>
#include <utility>
#include <vector>
#include "CatalogManager.h"

using namespace std;

class RecordManagerException : public exception
{
private:
    string ErrorMessage;
public:
    RecordManagerException()= default;
    explicit RecordManagerException(string message) : ErrorMessage(std::move(message)){}
    const char * what () const noexcept override{return ErrorMessage.c_str();}
};

extern class Address;
namespace RecordManager
{
    enum NumType{CHAR, INT, FLOAT};
    //这是catalog manager的，后面再删掉

    enum COMPARISON_CONDITION{EQUAL_TO, NOT_EQUAL_TO, GREATER_THAN, LESS_THAN, GREATER_OR_EQUAL_THAN, LESS_OR_EQUAL_THAN};

    class Attribute
    {
    private:
        string Name;
        NumType Type;
        string Value;
    public:
        Attribute(string AttrName, const NumType& AttrType, string AttrValue)
        : Name(std::move(AttrName)), Type(AttrType), Value(std::move(AttrValue)){}
        string GetName() const {return Name;}
        NumType GetType() const {return Type;}
        string GetValue() const {return Value;}
    };

    class Condition : public Attribute
    {
    private:
        COMPARISON_CONDITION Op;
    public:
        /**
         * @brief Construct a new Condition object
         * 
         * @param AttrName The name of the attribute 
         * @param AttrType The type of the attribute
         * @param Operation The operation to make comparison
         * @param Value The value to compare to
         */
        Condition(string AttrName, NumType AttrType, COMPARISON_CONDITION Operation, string Value)
        : Op(Operation), Attribute(std::move(AttrName), AttrType, std::move(Value)){}

        /**
         * @brief Get the operation type in comparison
         * 
         * @return COMPARISON_CONDITION The operation
         */
        COMPARISON_CONDITION GetOperation() const {return Op;}

    };

    class Record
    {
    private:
        string TableName;
        vector<Attribute> Attributes;
    public:
        Record()= default;
        void AppendAttribute(const Attribute& Attr);

        Attribute& GetAttributeByName(const string& Name) const;
        void SetTableName(string Name) {TableName = std::move(Name);}
        const string& GetTableName() const {return TableName;}
        unsigned GetNumAttr() const {return Attributes.size();}
        const Attribute& GetAttrByIndex(int Index) const{return Attributes[Index];}
    };

    void CreateTable(const string& TableName);
    void DropTable(const string& TableName);
    bool AddressComparison(Address& a, Address& b);
    bool CheckSatisfaction(const Record& RecordToCheck, const Condition& Condition);
    bool CheckSatisfactionConditionList(const Record& RecordToCheck, const vector<Condition>& ConditionList);
    Record ReadOneRecord(Address Location, const vector<NumType>& AttributeType, const vector<string>& AttributeName, const vector<int>& AttributeSize);
    void WriteOneRecord(Block& BlockToInsert, int BlockOffset, const vector<int>& Sizes, const Record& RecordToWrite);
    Address GetAddressByRecordOffset(const string& TableName, int RecordSize, int RecordOffset);
    int GetRecordOffsetByAddress(Address& Location, int RecordSize);
    vector<Record> Select(const string& TableName, int NumRecords, const vector<string>& Names, const vector<int>& Sizes, const vector<NumType>& Types, const vector<Condition>& Where);
    vector<Record> Delete(const string& TableName, int NumRecords, const vector<string>& Names, const vector<int>& Sizes, const vector<NumType>& Types, const vector<Condition>& Where);
    Address Insert(const string& TableName, int NumRecords, const Record& RecordToInsert, const vector<int>& Sizes);
    vector<Record> Select(vector<Address> Locations, const vector<Condition>& Where, const vector<string>& Names, const vector<int>& Sizes, const vector<NumType>& Types);
    vector<Record> Delete(vector<Address> Locations, const vector<Condition>& Where, const vector<string>& Names, const vector<int>& Sizes, const vector<NumType>& Types);
}


#endif