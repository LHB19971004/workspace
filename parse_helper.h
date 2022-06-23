#pragma once
#include "std.h"
#include "boost/stacktrace.hpp"
class ParseHelper
{
public:
	ParseHelper(const char* buffer, int32_t buffer_len);
	~ParseHelper();
	uint64_t getUInt64();
	uint32_t getUInt32();
	uint16_t getUInt16();
	uint8_t getUInt8();
	string getString(uint32_t str_len, bool zero_end = true);
	string getHexString(uint32_t str_len);
	time_t getBcdTime();
	vector<char> getBuffer(uint32_t buffer_len);
	const char* getCurBufferPtr() { return buffer_ + offset_; };

	bool getPacketLenEnough(int32_t buff_len) { return buff_len <= buffer_len_ - offset_;};
	void addOffset(int32_t offset) { offset_ += offset;};
	void setOffset(int32_t offset) { offset_ = offset;};
	int32_t getOffset() {return offset_;};
	int32_t getLeftBufferLen() {return buffer_len_ - offset_;};
private:
	int32_t offset_;
	//vector<char> packet_;
	const char* buffer_;
	int32_t buffer_len_;
};

class ParseException
{
public:
	enum ErrorType
	{
		ET_BUFFER_NOT_ENOUGH,
		ET_BCD_TIME_ERROR,
		ET_BUFFER_NOT_USED_UP,
	};
	string message()
	{
		switch(err_)
		{
			case ET_BUFFER_NOT_ENOUGH:
				return "buffer not enough";
			case ET_BCD_TIME_ERROR:
				return "invalid BCD time";
			case ET_BUFFER_NOT_USED_UP:
				return "buffer is not used up";
			default:
				return "unknown error";
		}
	};
public:
	ParseException(){};

	ParseException(ErrorType err)
	{
		err_ = err;
	};
	ErrorType err_;
};