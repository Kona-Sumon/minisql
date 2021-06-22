/**
 * Block.cpp
 * 定义 Block 类
 */

#include "BufferManager.h"
#include <string>
#include <cstdlib>
#include <fstream>

Block::Block():
	file_name(""), ofs(0), is_dirty(false), 
	is_valid(false), is_pinned(false), LRU_flag(false)
{
	block_data = new byte[BLOCK_SIZE]();
}

Block::Block(Block& b):
	file_name(b.file_name), ofs(b.ofs), is_dirty(b.is_dirty), 
	is_valid(b.is_valid), is_pinned(b.is_pinned), LRU_flag(b.LRU_flag)
{
	block_data = new byte[BLOCK_SIZE];
	memcpy(block_data, b.block_data, BLOCK_SIZE * sizeof(byte));
}

Block& Block::operator=(const Block& b)
{
	if (this != &b) {
		file_name = b.file_name;
		ofs = b.ofs;
		is_dirty = b.is_dirty;
		is_valid = b.is_valid;
		is_pinned = b.is_pinned;
		LRU_flag = b.LRU_flag;
		delete[] block_data;
		block_data = new byte[BLOCK_SIZE];
		memcpy(block_data, b.block_data, BLOCK_SIZE * sizeof(byte));
	}
	return *this;
}


Block::~Block()
{
	if (is_valid && is_dirty) {
		writeBackToFile();
	}
	delete[] block_data;
}

byte* Block::getData()
{
	return block_data;
}


void Block::writeByte(int ofs, byte data)
{
	if (ofs < 0 || ofs >= BLOCK_SIZE)
		throw BufferException("Byte index out of range.");
	block_data[ofs] = data;
	is_dirty = true;
	LRU_flag = true;
}

byte Block::readByte(int ofs)
{
	if (ofs < 0 || ofs >= BLOCK_SIZE)
		throw BufferException("Byte index out of range.");
	LRU_flag = true;
	return block_data[ofs];
}

void Block::writeInt(int ofs, int val)
{
	// 使用大端
	byte* array = reinterpret_cast<byte*>(&val);

	for (int i = 0; i < 4; i++) {
		writeByte(ofs + i, array[i]);
	}
}

void Block::writeFloat(int ofs, float val)
{
	// 使用大端
	byte* array = reinterpret_cast<byte*>(&val);

	for (int i = 0; i < 4; i++) {
		writeByte(ofs + i, array[i]);
	}
}

void Block::writeString(int ofs, std::string str)
{
	int i;
	std::string::iterator it;
	for (i = 0, it = str.begin(); it != str.end(); i++, it++) {
		writeByte(ofs + i, *it);
	}
	writeByte(ofs + str.length(), '\0'); // 存储终止符号
}

int Block::readInt(int ofs)
{
	char array[4];
	for (int i = 0; i < 4; i++) {
		array[i] = readByte(ofs + i);
	}
	int* val = reinterpret_cast<int*>(array);
	return *val;
}

float Block::readFloat(int ofs)
{
	char array[4];
	for (int i = 0; i < 4; i++) {
		array[i] = readByte(ofs + i);
	}
	float* val = reinterpret_cast<float*>(array);
	return *val;
}

std::string Block::readString(int ofs, int length)
{
	char array[MAX_STR_LEN];
	for (int i = 0; i < length; i++) {
		array[i] = readByte(ofs + i);
	}
	array[length] = '\0';
	return std::string(array);
}


void Block::readFromFile(std::string file_name, int ofs)
{
	std::ifstream file;
	file.open(file_name, std::ios::in);
	if (!file) {
		throw BufferException(std::string("Cannot open file \"") + file_name + std::string("\"."));
		return;
	}

	try {
		file.seekg(ofs * BLOCK_SIZE, std::ios::beg);
		file.read(block_data, BLOCK_SIZE);
		file.close();
	}
	catch (const std::ios_base::failure& e) {
		throw BufferException(std::string("Out of range when reading \"") + file_name + std::string("\"."));
	}

	resetModes();
	setFileName(file_name);
	setOfs(ofs);
	validate();
}

void Block::writeBackToFile()
{
	std::ofstream file;
	file.open(file_name, std::ios::in);
	if (!file) {
		throw BufferException(std::string("Cannot open file \"") + file_name + std::string("\"."));
		return;
	}

	try {
		file.seekp(ofs * BLOCK_SIZE, std::ios::beg);
		file.write(block_data, BLOCK_SIZE);
		file.close();
	}
	catch (const std::ios_base::failure& e) {
		throw BufferException(std::string("Out of range when writing \"") + file_name + std::string("\"."));
	}
}

bool Block::isDirty() { return is_dirty; }
bool Block::isPinned() { return is_pinned; }
bool Block::isValid() { return is_valid; }
bool Block::getLRUFlag() { return LRU_flag; }
std::string Block::getFileName() { return file_name; }
int Block::getOfs() { return ofs; }

void Block::dirty() { is_dirty = true; }
void Block::pin() { is_pinned = true; }
void Block::unpin() { is_pinned = false; }
void Block::validate() { is_valid = true; }
void Block::invalidate() { is_valid = false; }
void Block::setLRUFlag(int val) { LRU_flag = val; }
void Block::setFileName(std::string _file_name) { file_name = _file_name; }
void Block::setOfs(int _ofs) { ofs = _ofs; }
void Block::resetModes()
{
	is_dirty = is_pinned = is_valid = LRU_flag = false;
}

