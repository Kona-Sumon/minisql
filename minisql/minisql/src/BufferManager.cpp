/**
 * BufferManager.cpp
 * ∂®“Â BufferManager ¿‡
 */

#include "BufferManager.h"
#include <fstream>

int BufferManager::pointer_to_free_block = BLOCK_NUM - 1;
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
		if (buffer[i].isValid() && buffer[i].getFileName() == file_name && buffer[i].getOfs() == ofs) {
			return i;
		}
	}
	return -1;
}

int BufferManager::getFreeIndex()
{
	int& p = pointer_to_free_block;
	p = (p + 1) % BLOCK_NUM;
	int initial_pointer = p;
	int round = 0;

	while (true) {
		if (!buffer[p].isValid() || (!buffer[p].getLRUFlag() && !buffer[p].isPinned())) {
			return p;
		}

		if (!buffer[p].isPinned()) buffer[p].setLRUFlag(0);

		p = (p + 1) % BLOCK_NUM;
		if (p == initial_pointer) {
			if (round == 0) {
				round++;
			}
			else if (round == 1) {
				for (int i = 0; i < BLOCK_NUM; i++) {
					buffer[i].unpin();
				}
			}
			else {
				throw BufferException("Failed to get a free index.");
			}
		}
	};
}

void BufferManager::invalidateFileName(std::string file_name)
{
	for (int i = 0; i < BLOCK_NUM; i++) {
		if (buffer[i].getFileName() == file_name) {
			buffer[i].invalidate();
		}
	}
}


// ≤‚ ‘
#include <iostream>

int main()
{
	BufferManager m;
	std::string filename = "C:\\Users\\imbiansl\\Desktop\\test.dat";
	try {
		int n = 128;
		for (int i = 0; i < n; i++) {
			Block& b = m.readBlockFromFile(filename, i);
			b.writeInt(20, i * 10);
			b.writeFloat(40, static_cast<float>(i) / 10.0);
			b.writeString(60, std::string("This is string no. ") + std::to_string(i));
			std::cout << b.readInt(20) << ' ' << b.readFloat(40) << ' ' << b.readString(60, 25) << std::endl;
		}

		for (int i = 0; i < n; i++) {
			Block& b = m.readBlockFromFile(filename, i);
			std::cout << b.readInt(20) << ' ' << b.readFloat(40) << ' ' << b.readString(60, 25) << std::endl;
		}

	}
	catch (BufferException e) {
		std::cout << e.what();
	}
	return 0;
}
