#include<CatalogManager.h>
#include<vector>

import INDEXMANAGER.Index;
import CATALOGMANAGER.Attribute;

import java.util.Vector;


Table::Table(std::string tableName, std::string primaryKey, std::vector<Attribute> attributeVector) {
        this->tableName = tableName;
        this->primaryKey = primaryKey;
        this->indexVector = new std::vector<Index>();
        this->indexNum = 0;
        this->attributeVector = attributeVector;
        this->attributeNum = attributeVector.size();
        this->rowNum = 0;
        for (int i = 0; i < attributeVector.size(); i++) {
            if (attributeVector[i].attributeName.equals(primaryKey))
                attributeVector[i].isUnique = true;
            this->rowLength += attributeVector[i].type.get_length();
        }
    }

Table::Table(std::string tableName, std::string primaryKey, std::vector<Attribute> attributeVector, std::vector<Index> indexVector, int rowNum) {
        this->tableName = tableName;
        this->primaryKey = primaryKey;
        this->attributeVector = attributeVector;
        this->indexVector = indexVector;
        this->indexNum = indexVector.size();
        this->attributeVector = attributeVector;
        this->attributeNum = attributeVector.size();
        this->rowNum = rowNum;
        for (int i = 0; i < attributeVector.size(); i++) {
            this->rowLength += attributeVector[i].type.get_length();
        }
    }
