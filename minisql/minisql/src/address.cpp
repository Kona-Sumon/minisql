/**
 * Address.cpp
 * ∂®“Â Address ¿‡
 */

#include "CatalogManager.h"
#include <string>

Address::Address() {
    //do nothing
}

Address::Address(std::string fileName,int blockOffset,int byteOffset) {
    this->fileName = fileName;
    this->blockOffset = blockOffset;
    this->byteOffset = byteOffset; 
}

int Address::compareTo(Address address) {
    if(this->fileName == address.fileName) { //first compare file name
        if (this->blockOffset == address.blockOffset) { //then compare block offset
            return this->byteOffset - address.byteOffset; // finally compare byte offset
        } 
        else {
            return this->blockOffset - address.blockOffset;
        }
     } 
    else {
        return 1;
     }

}

std::string Address::get_file_name() {
    return this->fileName;
}
int Address::get_block_offset() {
    return this->blockOffset;
}
int Address::get_byte_offset() {
    return this->byteOffset;
}

void Address::set_file_name(std::string fileName) {
    this->fileName = fileName;
}
void Address::set_block_offset(int blockOffset) {
    this->blockOffset = blockOffset;
}
void Address::set_byte_offset(int byteOffset) {
    this->byteOffset = byteOffset;
}
