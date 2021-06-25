/**
 * Attribute.cpp
 * ∂®“Â Attribute ¿‡
 */

#include "CatalogManager.h"
#include <string>

Attribute::Attribute(std::string attributeName, NumType type, int length, boolean isUnique) {
        this->attributeName = attributeName;
        this->type = new FieldType(type, length);
        this->isUnique = isUnique;
}

Attribute::Attribute(std::string attributeName, NumType type, boolean isUnique) {
        this->attributeName = attributeName;
        this->type = new FieldType(type);
        this->isUnique = isUnique;
}
