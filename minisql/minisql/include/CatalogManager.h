/**
 * CatalogManager.h
 * 声明 Address 类、Attribute 类、CatalogManager 类、Fieldtype类、Numtype类、Table类
 */

#pragma once
#include <string>
#include <vector>

const int CHARSIZE = 1;  //1 byte for a char
const int INTSIZE = 4;   //4 bytes for an integer
const int FLOATSIZE = 4; //4 bytes for a float number

class Address {

private:
    std::string fileName; //文件名
    int blockOffset; //文件中的块偏移
    int byteOffset;  //块中的字节偏移

public:
    Address();
    Address(std::string fileName, int blockOffset, int byteOffset);
    int compareTo(Address address);
    std::string get_file_name();
    int get_block_offset();
    int get_byte_offset();

    void set_file_name(std::string fileName);
    void set_block_offset(int blockOffset);
    void set_byte_offset(int byteOffset);
};

class Attribute {

public:
    std::string attributeName;
    FieldType type;
    bool isUnique;

    Attribute(std::string attributeName, NumType type, int length, bool isUnique);

    Attribute(std::string attributeName, NumType type, bool isUnique);

};

class FieldType {

private: 
    NumType type; //type of number
    int length; //length of char type

public: 

    FieldType();
    FieldType(NumType type);
    FieldType(NumType type, int length);

    NumType get_type();

    int get_length();

    void set_type(NumType type);
    void set_length(int length);
};

enum NumType;

class Table {

public:
    std::string tableName;
    std::string primaryKey;
    std::vector<Attribute> attributeVector;
    std::vector<Index> indexVector;
    int indexNum;
    int attributeNum;
    //public TableRow tableRow;
    int rowNum;
    int rowLength;

    Table(std::string tableName, std::string primaryKey, std::vector<Attribute> attributeVector);

    Table(std::string tableName, std::string primaryKey, std::vector<Attribute> attributeVector, std::vector<Index> indexVector, int rowNum);
};

class CatalogManager {

private:
    static LinkedHashMap<std::string, Table> tables = new LinkedHashMap<>();
    static LinkedHashMap<std::string, Index> indexes = new LinkedHashMap<>();
    static std::string tableFilename = "table_catalog";
    static std::string indexFilename = "index_catalog";
    static void initial_table();
    static bool is_index_exist(std::string indexName);
    static bool is_attribute_exist(std::string tableName, std::string attributeName);
    static void initial_table();
    static void initial_index();
    static void store_table();
    static void store_index();

public: 
    static void initial_catalog();
    static void store_catalog();

    static void show_catalog();

    static void show_index();

    static int get_max_attr_length(Table tab);

    static void show_table();

    static Table get_table(std::string tableName);

    static Index get_index(std::string indexName);

    static std::string get_primary_key(std::string tableName);

    static int get_row_length(std::string tableName);

    static int get_attribute_num(std::string tableName);

    static int get_row_num(std::string tableName);

    //check
    static bool is_primary_key(std::string tableName, std::string attributeName);

    static bool is_unique(std::string tableName, std::string attributeName);

    static bool is_index_key(std::string tableName, std::string attributeName);



    static std::string get_index_name(std::string tableName, std::string attributeName);

    static std::string get_attribute_name(std::string tableName, int i);

    static int get_attribute_index(std::string tableName, std::string attributeName);

    static FieldType get_attribute_type(std::string tableName, std::string attributeName);

    static int get_length(std::string tableName, std::string attributeName);

    static std::string get_type(std::string tableName, int i);

    static int get_length(std::string tableName, int i);

    static void add_row_num(std::string tableName);

    static void delete_row_num(std::string tableName, int num);

    static bool update_index_table(std::string indexName, Index tmpIndex);

    static bool is_attribute_exist(Vector<Attribute> attributeVector, std::string attributeName);

    //Interface
    static bool create_table(Table newTable);

    static bool drop_table(std::string tableName);

    static bool create_index(Index newIndex);

    static bool drop_index(std::string indexName);
};