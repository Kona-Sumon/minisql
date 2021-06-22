/**
 * BufferException.cpp
 * ∂®“Â BufferException ¿‡
 */

#include "BufferManager.h"
#include <string>

BufferException::BufferException(std::string str) : err(str) {}

const std::string& BufferException::what()
{
	return err;
}