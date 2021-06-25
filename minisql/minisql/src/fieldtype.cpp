/**
 * FieldType.cpp
 * ∂®“Â FieldType ¿‡
 */

#include "CatalogManager.h"
#include <string>

FieldType::FieldType() {
    //do noting
}

FieldType::FieldType(NumType type) {
    this->type = type; //set type ( for integer and float number )
    this->length = 1;
}

FieldType::FieldType(NumType type, int length) {
    this->type = type; //set type and length ( for char )
    this->length = length;
}

NumType get_type() {
    return this->type;
}

int FieldType::get_length() {
    switch(this->type) {
        case CHAR:
            return this->length * CHARSIZE;
        case INT:
            return INTSIZE;
        case FLOAT:
            return FLOATSIZE;
        default: //undefined type
            return 0;
    }
}

void FieldType::set_type(NumType type) {
    this->type = type;
}
void FieldType::set_length(int length) {
    this->length = length;
}
