#pragma once
#include "std.h"
#include "funs.h"
class FormatHelper
{
public:
	FormatHelper(int32_t size):offset_(0){ packet_.resize(size);};
	FormatHelper():offset_(0){ packet_.resize(1024);};
	
	void putUInt64(uint64_t value);
	void putUInt32(uint32_t value);
	void putUInt16(uint16_t value);
	void putUInt8(uint8_t value);
	void putString(const string& value, int32_t str_len);
	void putString(const string& value);
	void putHexString(const string& value);
	void putBuffer(const vector<char>& value);
	void putBuffer(const void* buffer, int32_t buffer_len);
	void putCrc16();
	void put809Time(time_t gps_time);
	void putBcdTime(time_t gps_time);
	vector<char> getPacket() {packet_.resize(offset_); offset_ = 0; return move(packet_);}
private:
	inline void put(const void* buffer, int32_t buffer_len) {
		while (packet_.size() < offset_ + buffer_len)
		{
			packet_.resize(packet_.size() * 2);
		}
		inl_memcpy_data_2dst(packet_.data(), offset_, buffer, buffer_len);
	}
private:
	int32_t offset_;
	vector<char> packet_;
};