#include "parse_helper.h"
#include "funs.h"
#include "std.h"
#include "logging.h"
#define THROW_IF_ERROR(len) \
if(this->buffer_len_ < this->offset_+len) \
{\
	throw(ParseException(ParseException::ET_BUFFER_NOT_ENOUGH));\
}\


ParseHelper::ParseHelper(const char* buffer, int32_t buffer_len)
	:offset_(0)
	,buffer_(buffer)
	,buffer_len_(buffer_len)
{
}

ParseHelper::~ParseHelper()
{
	if (offset_ != buffer_len_)
	{
		LOG_ERROR << "offset "<< offset_ << " != buffer_len " << buffer_len_; 
		//throw(ParseException(ParseException::ET_BUFFER_NOT_USED_UP));
	}
}

uint64_t ParseHelper::getUInt64()
{
	THROW_IF_ERROR(8);
	uint64_t tmp;
	inl_memcpy_data(&tmp,buffer_,offset_,8);
	reverse_uint64(tmp);
	return tmp;
}

uint32_t ParseHelper::getUInt32()
{
	THROW_IF_ERROR(4);
	uint32_t tmp;
	inl_memcpy_data(&tmp,buffer_,offset_,4);
	reverse_dword(tmp);
	return tmp;
}

uint16_t ParseHelper::getUInt16()
{
	THROW_IF_ERROR(2);
	uint16_t tmp;
	inl_memcpy_data(&tmp,buffer_,offset_,2);
	reverse_word(tmp);
	return tmp;
}

uint8_t ParseHelper::getUInt8()
{
	THROW_IF_ERROR(1);
	uint8_t tmp;
	inl_memcpy_data(&tmp,buffer_,offset_,1);
	return tmp;
}

string ParseHelper::getString(uint32_t str_len, bool zero_end /* = true */)
{
	THROW_IF_ERROR(str_len);
	const char* buffer = buffer_+offset_;
	offset_ += str_len;
	string ret(buffer,str_len);
	if (zero_end)
		return ret.c_str();
	else
		return ret;
}

string ParseHelper::getHexString(uint32_t str_len)
{
	THROW_IF_ERROR(str_len);
	boost::scoped_array<char> scoped_buff;
	int32_t scoped_buff_size = str_len * 2 + 1;
	try
	{
		scoped_buff.reset(new char[scoped_buff_size]);
	}catch(std::bad_alloc &)
	{
		assert(0);
		return "";
	}
	const char* buffer = buffer_+offset_;
	offset_ += str_len;
	memery_to_text(scoped_buff.get(), scoped_buff_size, (const uint8_t *)buffer, str_len);
	scoped_buff[scoped_buff_size - 1] = 0;
	return scoped_buff.get();
}

vector<char> ParseHelper::getBuffer(uint32_t buffer_len)
{
	THROW_IF_ERROR(buffer_len);
	const char* buffer = buffer_+offset_;
	offset_ += buffer_len;
	vector<char> ret;
	ret.resize(buffer_len);
	memcpy(ret.data(), buffer, buffer_len);
	return move(ret);
}

time_t ParseHelper::getBcdTime()
{
	THROW_IF_ERROR(6);
	const char* buffer = buffer_+offset_;
	offset_ += 6;
	tm tm_time;
	BcdTime2Tm(buffer,tm_time);
	if(!IsValidTm(tm_time))
		throw(ParseException(ParseException::ET_BCD_TIME_ERROR));
	return mktime(&tm_time);
}