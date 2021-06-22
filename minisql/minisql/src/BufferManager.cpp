/**
 * BufferManager.cpp
 * 定义 BufferManager 类
 */

#include "BufferManager.h"
#include <fstream>

int BufferManager::pointer_to_free_block = 0;
int BufferManager::pinned_count = 0;
Block BufferManager::buffer[BLOCK_NUM];

BufferManager::BufferManager()
{

}

BufferManager::~BufferManager()
{

}

Block& BufferManager::readBlockFromFile(std::string file_name, int ofs)
{
	int idx = getIndexInBuffer(file_name, ofs);
	if (idx == -1) {
		idx = getFreeIndex();
		buffer[idx].readFromFile(file_name, ofs);
	}
	return buffer[idx];
}



int BufferManager::getIndexInBuffer(std::string file_name, int ofs)
{
	for (int i = 0; i < BLOCK_NUM; i++) {
		if (buffer[i].getFileName() == file_name && buffer[i].getOfs() == ofs) {
			return i;
		}
	}
	return -1;
}

int BufferManager::getFreeIndex()
{
	int& p = pointer_to_free_block;
	int initial_pointer = p;
	bool protect_pinned = true;

	while (true) {
		if (!buffer[p].isValid() || (!buffer[p].getLRUFlag() && !(protect_pinned && buffer[p].isPinned())))
			return p;
		if (!buffer[p].isPinned()) buffer[p].setLRUFlag(0);
		p = (p + 1) % BLOCK_NUM;
		if (p == initial_pointer) // 所有块都被钉住
		{
			if (!protect_pinned) throw BufferException("Unable to get a free index in buffer.");
			protect_pinned = false;
		}
	};
}

// 测试

#include <iostream>

int main()
{
	BufferManager m;
	std::string filename = "C:\\Users\\imbiansl\\Desktop\\test.dat";
	Block b = m.readBlockFromFile(filename, 2);
	std::cout << b.getFileName() << std::endl << b.getOfs() << std::endl;
	//b.writeInt(0, 120);
	//b.writeInt(4, 480);
	//b.writeFloat(8, 9.6);
	//b.writeString(12, "Yes!");
	//b.writeString(112, "Hello from here.");
	std::cout << b.readInt(4) << ' ' << b.readFloat(8) << ' ' << b.readString(112, 16);
	return 0;
}
