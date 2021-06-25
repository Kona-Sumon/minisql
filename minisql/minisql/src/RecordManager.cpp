#include "BufferManager.h"
#include "RecordManager.h"
#include "CatalogManager.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <algorithm>
using namespace RecordManager;
BufferManager BM;
/**
 * @brief Create a table for a specified table name. Throw RecordManagerException if error occurs.
 *
 * @param TableName The given table name
 */
void RecordManager::CreateTable(const string& TableName)
{
    fstream TableFile(TableName, ios::out | ios::binary);
    if(!TableFile) 
        throw RecordManagerException("Error in table creation: file generating failed");
    TableFile.close();
    Block FirstBlock;
    try
    {    
        FirstBlock = BM.readBlockFromFile(TableName, 0);
    }
    catch (BufferException& e)
    {
       throw RecordManagerException("Error in table creation: " + e.what());
    }
    FirstBlock.writeInt(0, -1);
}

/**
 * @brief Drop the table with the given name.Throw a RecordManagerException if error occurs.
 * 
 * @param TableName The name of the table to drop
 */
void RecordManager::DropTable(const string& TableName)
{
    if(EOF == remove(TableName.c_str())) 
        throw RecordManagerException("Error in table dropping: file deletion failed");
    BM.invalidateFileName(TableName);
}

/**
 * @brief Check whether a record satisfies a condition
 * 
 * @param RecordToCheck The record
 * @param Condition The condition
 * @return true if satisfies
 * @return false if doesn't satisfy
 */
bool RecordManager::CheckSatisfaction(const Record& RecordToCheck, const Condition& Condition)
{
    Attribute AttributeToCheck = RecordToCheck.GetAttributeByName(Condition.GetName());
    switch (Condition.GetOperation())
    {
        case RecordManager::EQUAL_TO:
            if(AttributeToCheck.GetValue() == Condition.GetValue())
                return true;
            else return false;
        case RecordManager::NOT_EQUAL_TO:
            return !(AttributeToCheck.GetValue() == Condition.GetValue());
        case RecordManager::GREATER_THAN:
            switch (Condition.GetType())
            {
                case RecordManager::CHAR:
                    return AttributeToCheck.GetValue() > Condition.GetValue();
                case RecordManager::INT:
                    return stoi(AttributeToCheck.GetValue()) > stoi(Condition.GetValue());
                case RecordManager::FLOAT:
                    return stof(AttributeToCheck.GetValue()) > stof(Condition.GetValue());
            }
        case RecordManager::LESS_THAN:
            switch (Condition.GetType())
            {
                case RecordManager::CHAR:
                    return AttributeToCheck.GetValue() < Condition.GetValue();
                case RecordManager::INT:
                    return stoi(AttributeToCheck.GetValue()) < stoi(Condition.GetValue());
                case RecordManager::FLOAT:
                    return stof(AttributeToCheck.GetValue()) < stof(Condition.GetValue());
            }
        case RecordManager::GREATER_OR_EQUAL_THAN:
            switch (Condition.GetType())
            {
                case RecordManager::CHAR:
                    return AttributeToCheck.GetValue() >= Condition.GetValue();
                case RecordManager::INT:
                    return stoi(AttributeToCheck.GetValue()) >= stoi(Condition.GetValue());
                case RecordManager::FLOAT:
                    return stof(AttributeToCheck.GetValue()) >= stof(Condition.GetValue());
            }
        case RecordManager::LESS_OR_EQUAL_THAN:
            switch (Condition.GetType())
            {
                case RecordManager::CHAR:
                    return AttributeToCheck.GetValue() <= Condition.GetValue();
                case RecordManager::INT:
                    return stoi(AttributeToCheck.GetValue()) <= stoi(Condition.GetValue());
                case RecordManager::FLOAT:
                    return stof(AttributeToCheck.GetValue()) <= stof(Condition.GetValue());
            }
    }
}

/**
 * @brief Given the attribute name, find the attribute in this record
 * 
 * @param Name The attribute name
 * @return Attribute& - The attribute object looked for
 */
Attribute& Record::GetAttributeByName(const string& Name) const
{
    auto i = Attributes.begin();
    while(i->GetName() != Name && i !=Attributes.end())
    {
        i++;
    }
    if(i == Attributes.end())
        throw RecordManagerException("Error in selection: no such attribute");
    return const_cast<Attribute &>(*i);
}

/**
 * @brief Add the attribute to the end of this record
 * @param Attr The attribute to add
 */
void Record::AppendAttribute(const Attribute& Attr)
{
    Attributes.push_back(Attr);
}

/**
 * @brief Find records that satisfy given conditions from a specified table
 * @param TableName The table name
 * @param NumRecords The number of records in this table
 * @param Sizes The sizes of attributes of a record in this table in storage
 * @param Where The conditions
 * @param Types The Type of each attribute
 * @param Names The name of each attribute
 * @return The records obtained
 */
vector<Record> RecordManager::Select(const string& TableName, const int NumRecords, const vector<string>& Names, const vector<int>& Sizes, const vector<NumType>& Types, const vector<Condition>& Where)
{
    vector<Record> QuerySet;
    int RecordSize = 0;
    for(int Size : Sizes) RecordSize += Size;
    try
    {
        int NumCheckedRecords = 0;
        int BlockOffset = INTSIZE;
        int BlockIndex = 0;
        Block& TableBlock = BM.readBlockFromFile(TableName, BlockIndex);

        while(NumCheckedRecords < NumRecords)
        {
            if(BlockOffset + RecordSize >= BLOCK_SIZE)
            {
                BlockOffset = 0;
                TableBlock = BM.readBlockFromFile(TableName, ++BlockIndex);
            }
            if(TableBlock.readInt(BlockOffset) < 0)
            {
                Record NextRecord = ReadOneRecord(Address(TableName, BlockIndex, BlockOffset), Types, Names, Sizes);
                if(CheckSatisfactionConditionList(NextRecord, Where))
                    QuerySet.push_back(NextRecord);
                NumCheckedRecords ++;
            }
            BlockOffset += RecordSize;
        }
    }
    catch (BufferException& e)
    {
        throw RecordManagerException("Error in selection: " + e.what());
    }
    catch (invalid_argument&)
    {
        throw RecordManagerException("Error in selection: Wrong attribute type");
    }
    return QuerySet;
}

/**
 * @brief Read a record from the given table at the given address in block
 * @param Location The address of the record in the block
 * @param AttributeType
 * @param AttributeName
 * @param AttributeSize
 * @return The obtained record
 */
Record RecordManager::ReadOneRecord(Address Location, const vector<NumType>& AttributeType, const vector<string>& AttributeName, const vector<int>& AttributeSize)
{
    Record RecordRead;
    RecordRead.SetTableName(Location.get_file_name());
    try
    {
        Block &BlockToRead = BM.readBlockFromFile(Location.get_file_name(), Location.get_block_offset());
        int BlockOffset = Location.get_byte_offset();
        BlockOffset++;
        string AttrValue;
        if (AttributeType.size() != AttributeName.size())
            throw RecordManagerException("Error in reading record: number of attributes and names/types don't match");

        for (int i = 0; i < AttributeType.size(); i++) {
            switch (AttributeType[i]) {
                case RecordManager::INT:
                    AttrValue = to_string(BlockToRead.readInt(BlockOffset));
                    break;
                case RecordManager::FLOAT:
                    AttrValue = to_string(BlockToRead.readFloat(BlockOffset));
                    break;
                case RecordManager::CHAR:
                    AttrValue = BlockToRead.readString(BlockOffset, AttributeSize[i]);
                    int ActualLength = AttrValue.find_first_of('\0');
                    if (-1 != ActualLength) AttrValue = AttrValue.substr(0, ActualLength);
                    break;
            }
            BlockOffset += AttributeSize[i];
            RecordRead.AppendAttribute(Attribute(AttributeName[i], AttributeType[i], AttrValue));
        }
    }
    catch (BufferException& e)
    {
        throw RecordManagerException("Error in reading record: " + e.what());
    }
    return RecordRead;
}

/**
 * @brief Check whether a record satisfies a list of conditions
 * @param RecordToCheck
 * @param ConditionList
 * @return true if yes
 */
bool RecordManager::CheckSatisfactionConditionList(const Record &RecordToCheck, const vector<Condition> &ConditionList)
{
    for(const auto & i : ConditionList)
    {
        if(!CheckSatisfaction(RecordToCheck, i)) return false;
    }
    return true;
}

/**
 * @brief Delete the record satisfying the given conditions in a table.
 * @param TableName
 * @param NumRecords The number of records in the table
 * @param Names The attribute names of the table
 * @param Sizes The sizes of attributes of the table
 * @param Types The types of attributes of the table
 * @param Where The conditions
 * @return The deleted records
 */
vector<Record> RecordManager::Delete(const string &TableName, int NumRecords, const vector<string> &Names, const vector<int> &Sizes,const vector<NumType> &Types, const vector<Condition> &Where)
{
    int NumCheckedRecords = 0;
    vector<Record> DeletedRecordSet;
    int RecordSize = 0;
    for(int Size : Sizes) RecordSize += Size;

    try
    {
        int BlockOffset = INTSIZE;
        int BlockIndex = 0;
        Block& TableBlock = BM.readBlockFromFile(TableName, BlockIndex);
        Block& FirstBlock = TableBlock;
        FirstBlock.pin();
        int RecordOffset = 0;
        while(NumCheckedRecords < NumRecords)
        {
            if(BlockOffset + RecordSize >= BLOCK_SIZE)
            {
                BlockOffset = 0;
                TableBlock = BM.readBlockFromFile(TableName, ++BlockIndex);
            }
            if(TableBlock.readInt(BlockOffset) < 0)
            {
                Record NextRecord = ReadOneRecord(Address(TableName, BlockIndex, BlockOffset), Types, Names, Sizes);
                if(CheckSatisfactionConditionList(NextRecord, Where))
                {
                    TableBlock.writeInt(BlockOffset, 0);
                    TableBlock.writeInt(BlockOffset + 1, FirstBlock.readInt(0));
                    FirstBlock.writeInt(0, RecordOffset);
                    DeletedRecordSet.push_back(NextRecord);
                }
                NumCheckedRecords ++;
            }
            RecordOffset++;
            BlockOffset += RecordSize;
        }

        FirstBlock.unpin();
    }
    catch (BufferException& e)
    {
        throw RecordManagerException("Error in deletion: " + e.what());
    }
    catch (invalid_argument&)
    {
        throw RecordManagerException("Error in deletion: Wrong attribute type");
    }
    return DeletedRecordSet;
}

/**
 * @brief Insert a new record
 * @param TableName
 * @param NumRecords The number of existing records in the table
 * @param RecordToInsert
 * @param Sizes The sizes of attributes of this table
 * @return
 */
Address RecordManager::Insert(const string& TableName, int NumRecords, const Record &RecordToInsert, const vector<int>& Sizes)
{
    if(RecordToInsert.GetTableName() != TableName)
        throw RecordManagerException("Error in insertion: table name not match");
    try
    {
        int RecordSize = 0;
        for(int Size : Sizes) RecordSize += Size;
        Block& FirstBlock = BM.readBlockFromFile(TableName, 0);
        int AvailableOffset = FirstBlock.readInt(0);
        if(AvailableOffset == -1)
            AvailableOffset = NumRecords;

        Address NewRecordLocation = GetAddressByRecordOffset(TableName, RecordSize, AvailableOffset);
        Block& BlockToInsert = BM.readBlockFromFile(TableName, NewRecordLocation.get_block_offset());
        FirstBlock.writeInt(0, BlockToInsert.readInt(AvailableOffset + 1));
        FirstBlock.unpin();
        WriteOneRecord(BlockToInsert, NewRecordLocation.get_block_offset(), Sizes, RecordToInsert);
        return NewRecordLocation;
    }
    catch (BufferException& e)
    {
        throw RecordManagerException("Error in insertion: " + e.what());
    }
    catch (invalid_argument&)
    {
        throw RecordManagerException("Error in insertion: Wrong attribute type");
    }
}

/**
 * @brief Find the address of a record from its offset (calculated by records)
 * @param TableName
 * @param RecordSize The size of the record in the table
 * @param RecordOffset
 * @return
 */
Address RecordManager::GetAddressByRecordOffset(const string& TableName, int RecordSize, int RecordOffset)
{
    int BlockIndex, BlockOffset;
    int FirstBlockCapacity = (BLOCK_SIZE - INTSIZE) / RecordSize;
    int NormalBlockCapacity = BLOCK_SIZE / RecordSize;
    if(RecordOffset < FirstBlockCapacity)
    {
        return Address(TableName, 0, RecordSize * RecordOffset + INTSIZE);
    }
    else
    {
        BlockIndex = (RecordOffset - FirstBlockCapacity) / NormalBlockCapacity + 1;
        BlockOffset = RecordSize * (RecordOffset - FirstBlockCapacity - NormalBlockCapacity * (BlockIndex - 1));
        return Address(TableName, BlockIndex, BlockOffset);
    }
}

/**
 * @brief Write a record to a given location in buffer
 * @param BlockToInsert
 * @param BlockOffset The location's offset inside the block (calculated by bytes)
 * @param Sizes The sizes of the attributes of the table
 * @param RecordToWrite
 */
void RecordManager::WriteOneRecord(Block& BlockToInsert, int BlockOffset, const vector<int>& Sizes, const Record &RecordToWrite)
{
    BlockToInsert.writeInt(BlockOffset, -1);
    BlockOffset++;
    for (int i = 0; i < RecordToWrite.GetNumAttr(); i++)
    {
        switch (RecordToWrite.GetAttrByIndex(i).GetType())
        {
            case INT:
                BlockToInsert.writeInt(BlockOffset, stoi(RecordToWrite.GetAttrByIndex(i).GetValue()));
                break;
            case FLOAT:
                BlockToInsert.writeFloat(BlockOffset, stof(RecordToWrite.GetAttrByIndex(i).GetValue()));
                break;
            case CHAR:
                BlockToInsert.writeString(BlockOffset, RecordToWrite.GetAttrByIndex(i).GetValue());
        }
        BlockOffset += Sizes[i];
    }
}

/**
 * @brief Find record satisfying conditions from given location
 * @param Locations
 * @param Where Conditions
 * @param Names Attribute names
 * @param Sizes Attribute sizes
 * @param Types Attribute types
 * @return A vector containing all obtained records
 */
vector<Record> RecordManager::Select(vector<Address> Locations, const vector<Condition> &Where, const vector<string>& Names, const vector<int>& Sizes, const vector<NumType>& Types)
{
    vector<Record> QuerySet;
    sort(Locations.begin(), Locations.end(), AddressComparison);
    int ThisBlockIndex, LastBlockIndex;
    try
    {
        Block* TableBlock;
        for (auto Addr = Locations.begin(); Addr != Locations.end(); Addr++)
        {
            ThisBlockIndex = Addr->get_block_offset();
            if(Addr == Locations.begin() || LastBlockIndex != ThisBlockIndex)
            {
                TableBlock = &(BM.readBlockFromFile(Addr->get_file_name(), Addr->get_block_offset()));
            }
            if(TableBlock->readInt(Addr->get_byte_offset()) < 0)
            {
                Record RecordRead = ReadOneRecord(*Addr, Types, Names, Sizes);
                if(CheckSatisfactionConditionList(RecordRead, Where)) QuerySet.push_back(RecordRead);
            }
            LastBlockIndex = ThisBlockIndex;
        }
        return QuerySet;
    }
    catch (BufferException& e)
    {
        throw RecordManagerException("Error in selection: " + e.what());
    }
    catch (invalid_argument&)
    {
        throw RecordManagerException("Error in selection: Wrong attribute type");
    }
}

bool RecordManager::AddressComparison(Address &a, Address &b)
{
    if(a.get_block_offset() != b.get_block_offset()) return a.get_block_offset() < b.get_block_offset();
    else return a.get_byte_offset() < b.get_byte_offset();
}

/**
 * @brief Do conditional deletion at given addresses
 * @param Locations
 * @param Where Conditions
 * @param Names
 * @param Sizes
 * @param Types
 * @return The vector containing the deleted records
 */
vector<Record> RecordManager::Delete(vector<Address> Locations, const vector<Condition> &Where, const vector<string> &Names, const vector<int> &Sizes, const vector<NumType> &Types)
{
    vector<Record> DeleteSet;
    sort(Locations.begin(), Locations.end(), AddressComparison);
    int ThisBlockIndex, LastBlockIndex;
    try
    {
        Block* TableBlock;
        Block& FirstBlock = BM.readBlockFromFile(Locations[0].get_file_name(), 0);
        FirstBlock.pin();
        int RecordSize = 0;
        for(int Size: Sizes) RecordSize += Size;
        for (auto Addr = Locations.begin(); Addr != Locations.end(); Addr++)
        {
            ThisBlockIndex = Addr->get_block_offset();
            if(Addr == Locations.begin() || LastBlockIndex != ThisBlockIndex)
            {
                TableBlock = &(BM.readBlockFromFile(Addr->get_file_name(), Addr->get_block_offset()));
            }
            if(TableBlock->readInt(Addr->get_byte_offset()) < 0)
            {
                Record RecordRead = ReadOneRecord(*Addr, Types, Names, Sizes);
                if(CheckSatisfactionConditionList(RecordRead, Where))
                {
                    DeleteSet.push_back(RecordRead);
                    TableBlock->writeInt(Addr->get_byte_offset(), 0);
                    TableBlock->writeInt(Addr->get_byte_offset() + 1, FirstBlock.readInt(0));
                    FirstBlock.writeInt(0, GetRecordOffsetByAddress(*Addr, RecordSize));
                }
            }
            LastBlockIndex = ThisBlockIndex;
        }
        FirstBlock.unpin();
        return DeleteSet;
    }
    catch (BufferException& e)
    {
        throw RecordManagerException("Error in deletion:" + e.what());
    }
    catch (invalid_argument&)
    {
        throw RecordManagerException("Error in deletion: Wrong attribute type");
    }
}

/**
 * @brief Get the offset of a record (calculated by records) by its address
 * @param Location Address
 * @param RecordSize Size of the record in storage
 * @return
 */
int RecordManager::GetRecordOffsetByAddress(Address &Location, int RecordSize)
{
    if(Location.get_block_offset() == 0)
        return (Location.get_byte_offset() - INTSIZE) / RecordSize;
    else
    {
        int FirstBlockCapacity = (BLOCK_SIZE - INTSIZE) / RecordSize;
        int NormalBlockCapacity = BLOCK_SIZE / RecordSize;
        return FirstBlockCapacity + (Location.get_block_offset() - 1) * NormalBlockCapacity + Location.get_byte_offset() / RecordSize;
    }
}

