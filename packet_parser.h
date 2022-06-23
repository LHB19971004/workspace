#pragma once
#include "std.h"
#include "jt_frame.h"

class Project;
class PacketParser
{
public:
    using Callback = std::function<void(const char *buff, int32_t buff_len)>;

    PacketParser(Callback callback);
    ~PacketParser(){};
public:
    int32_t addData(const char* data, int32_t data_len);
private:
	int32_t judgeFullPacket(const char* data, int32_t data_len);
private:
    std::array<char, 8192> buffer_;
    int32_t buffer_len_ = 0;
	Callback callback_;
};
