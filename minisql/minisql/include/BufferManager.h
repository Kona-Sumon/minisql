/**
 * BufferManager.h
 * 声明 BufferException 类、声明 Block 类、声明 BufferManager 类
 */

#pragma once
#include <string>

typedef char byte;
const int BLOCK_SIZE = 4096; // 每个 block 大小为 4 KB
const int BLOCK_NUM = 64;    // 缓冲区存放 64 个 block
const int MAX_STR_LEN = 256; // 字符串长度（包括末尾 \0）最长为 256

/**
 * 异常类
 */
class BufferException {
    std::string err;
public:
    BufferException(std::string str);
    const std::string& what();
};


/**
 * block 类
 */
class Block {

private:
    std::string file_name; // 该块来自哪个文件
    int ofs; // 该块在文件中的偏移量（单位：block）
    bool is_dirty; // 该块是否为脏（被修改过）
    bool is_valid; // 该块是否有效
    bool is_pinned; // 该块是否被锁住（不能从缓存中移除）
    bool LRU_flag; // 0 （且未锁住）表示可以立即替换，1 表示不能立即替换
    byte *block_data;

    // 向第 ofs 字节处写 1 字节
    void writeByte(int ofs, byte data);

    // 从第 ofs 处读 1 字节
    byte readByte(int ofs);

public:
    Block();
    Block(std::string _file_name, int _ofs);
    Block(Block& b);
    Block& operator=(const Block& b);

    ~Block();

    // 返回 block 内容指针
    byte* getData();

    /**
     * 读写块内容
     */
    // 向第 ofs 字节处写 1 个 int
    void writeInt(int ofs, int val);

    // 向第 ofs 字节处写 1 个 float
    void writeFloat(int ofs, float val);

    // 从第 ofs 字节开始写 1 个字符串
    void writeString(int ofs, std::string str);

    // 从第 ofs 字节处读 1 个 int
    int readInt(int ofs);

    // 从第 ofs 字节处读 1 个 float
    float readFloat(int ofs);

    // 从第 ofs 字节处读 1 个长度为 len 的字符串
    std::string readString(int ofs, int len);

    // 将 file_name 文件的第 ofs 块读入该块
    void readFromFile(std::string file_name, int ofs);

    // 将块中内容写回文件
    void writeBackToFile();

    /**
     * 读写块状态
     */
    bool isDirty();
    bool isPinned();
    bool isValid();
    bool getLRUFlag();
    std::string getFileName();
    int getOfs();

    void dirty();
    void pin();
    void unpin();
    void validate();
    void invalidate();
    void setLRUFlag(int val);
    void setFileName(std::string _file_name);
    void setOfs(int _ofs);
    void resetModes();
};

class BufferManager {

private:
    static Block buffer[BLOCK_NUM];
    static int pointer_to_free_block;
    static int pinned_count;

    // 得到 file_name 文件的第 ofs 块在 buffer 中下标，若无返回 -1
    int getIndexInBuffer(std::string file_name, int ofs);

    // 找到下一个可用的 buffer 下标
    int getFreeIndex();

    

public:
    BufferManager();
    ~BufferManager();

    // 将 file_name 文件的第 ofs 块读入 buffer，并返回 buffer 中块的引用
    Block& readBlockFromFile(std::string file_name, int ofs);

    // 将 buffer 中文件名为 file_name 的块设置为无效
    void invalidateFileName();

};