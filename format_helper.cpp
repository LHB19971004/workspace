#include "format_helper.h"
#include "funs.h"
#include "std.h"
#include "boost/scoped_array.hpp"
#include "utils.h"
void FormatHelper::putUInt64(uint64_t value)
{
	reverse_uint64(value);
	put(&value,8);
}

void FormatHelper::putUInt32(uint32_t value)
{
	reverse_dword(value);
	put(&value,4);
}

void FormatHelper::putUInt16(uint16_t value)
{
	reverse_word(value);
	put(&value,2);
}

void FormatHelper::putUInt8(uint8_t value)
{
	put(&value,1);
}

void FormatHelper::putString(const string& value, int32_t str_len)
{
	put(value.c_str(),min((int32_t)value.size(),str_len));
	if (str_len > value.size())
	{
		str_len -= value.size();
		memset(packet_.data()+offset_,0,str_len);
		offset_ += str_len;
	}
}


void FormatHelper::putString(const string& value)
{
	put(value.c_str(), value.size());
}

void FormatHelper::putHexString(const string& value)
{
	boost::scoped_array<char> scoped_buff;
	int32_t scoped_buff_size = value.size();
	try
	{
		scoped_buff.reset(new char[scoped_buff_size]);
	}catch(std::bad_alloc &)
	{
		assert(0);
		return;
	}
	char *buff = scoped_buff.get();

	int32_t buff_len = text_to_memery((uint8_t*)buff,scoped_buff_size,value.c_str(), value.size());
	
	put(buff,buff_len);
}

void FormatHelper::putBuffer(const vector<char>& value)
{
	put(value.data(),value.size());
}

void FormatHelper::putBuffer(const void* buffer, int32_t buffer_len)
{
	put(buffer,buffer_len);
}

void FormatHelper::putCrc16()
{
	auto value = Utils::crc16(packet_.data(), offset_);
	reverse_word(value);
	put( &value, 2);
}

void FormatHelper::put809Time(time_t gps_time)
{
	tm gps_tm = *localtime(&gps_time);
	uint8_t month = gps_tm.tm_mon + 1;
	uint16_t year = gps_tm.tm_year + 1900;
	reverse_word(year);
	put( &gps_tm.tm_mday, 1);
	put( &month, 1);
	put( &year, 2);
	put( &gps_tm.tm_hour, 1);
	put( &gps_tm.tm_min, 1);
	put( &gps_tm.tm_sec, 1);
}

void FormatHelper::putBcdTime(time_t gps_time)
{
	tm gps_tm = *localtime(&gps_time);
	uint8_t bcd_time[6];
	TmTime2Bcd(gps_tm, bcd_time);
	put(bcd_time, 6);
}