#include "packet_parser.h"
#include "funs.h"
#include "logging.h"
#include "utils.h"
#include "project.h"
const uint8_t PACKET_HEAD[] = { 0x5B };
const uint8_t PACKET_TAIL[] = { 0x5D };

PacketParser::PacketParser(Callback callback)
	: callback_(callback)
{
};
int32_t PacketParser::addData(const char* data, int32_t data_len)
{
	// write to buffer
	if (buffer_.size() - buffer_len_ < data_len)
	{
		return 0;
	}

	char* buff = &buffer_[0];
	memcpy((char *)buff + buffer_len_, data, data_len);
	buffer_len_ += data_len;

	int32_t left_begin = 0;
	int32_t left_len = buffer_len_ - left_begin;
	int32_t offset = 0;
	uint32_t find_head_start = 0;		//这里是绝对偏移
	uint32_t find_tail_start = 0;		//这里是绝对偏移
	//uint32_t is_packet  = 0;
	//uint32_t tail_cnt   = 0;

	int32_t packet_count = 0;
	while(offset < buffer_len_)
	{
		int32_t   head_pos  = 0;

		//找到包头
		if(-1 == (head_pos = FindBytes(buff + find_head_start,buffer_len_ - find_head_start,
			PACKET_HEAD,1)))
		{
			break;
		}
		head_pos += find_head_start;		//获取到绝对偏移，以方便计算

		//找到尾
		find_tail_start = head_pos + 1;
		int32_t tail_cnt  = 0;
		int32_t is_packet = 0;
		int32_t tail_pos = 0;
		while(!is_packet)
		{
			if(-1 == (tail_pos = FindBytes(buff + find_tail_start,
				buffer_len_ - find_tail_start,
				PACKET_TAIL,1)))
			{
				break;
			}
			tail_pos += find_tail_start;		//获取到绝对偏移，以方便计算
			//包尾数++
			tail_cnt++;

			//包的起始
			const char *packet_buff = buff+head_pos;
			//包长为：偏移+原来离头的偏移 + tail的长度
			int32_t packet_len = tail_pos - head_pos + 1;
			is_packet = judgeFullPacket(packet_buff, packet_len);
			find_tail_start = tail_pos + 1;
		}
		if(is_packet)
		{
			//如果包头不为0，那么意味着数据不纯，必须得丢掉这部分数据
			if(head_pos > find_head_start)
			{
				const char *drop_buff = buff + find_head_start;
				int32_t drop_buff_len = head_pos - find_head_start;
				//drop
                LOG_ERROR << "PacketParser drop: " << MemoryToText(drop_buff, drop_buff_len);
			}

			//包的起始
			const char *packet_buff = buff+head_pos;
			//包长为：偏移+原来离头的偏移 + tail的长度
			int32_t packet_len = tail_pos - head_pos + 1;
			packet_count++;
			callback_(packet_buff, packet_len);
			//
			//调整offset			
			offset = tail_pos + 1;
			left_begin = tail_pos + 1;
			left_len   = buffer_len_ - left_begin;
			find_head_start = tail_pos + 1;
			////退出小循环，再找包头
			//break;
		}else if(0 == tail_cnt)
		{
			break;
		}else
		{
			find_head_start = head_pos + 1;
		}
	}

    if (left_begin != 0)
	{
		memmove((char *)&buffer_[0], buff + left_begin, left_len);
        buffer_len_ = left_len;
	}
	if (packet_count == 0)
	{
		LOG_ERROR << "PACKET error " << MemoryToText(data, data_len);
	}
	return 1;
}

int32_t PacketParser::judgeFullPacket(const char* buffer, int32_t buff_len)
{
	//至少26字节：头（1）+消息头（22）+校验（2）+尾（1）
	if (buff_len < 26)
	{
		return 0;
	}
	boost::scoped_array<char> msg_buff;
	uint32_t msg_size = buff_len;
	try
	{
		msg_buff.reset(new char[msg_size]);
	}
	catch (bad_alloc &)
	{
		LOG_ERROR << "Jt808Frame generate failed: memory alloc fail";
		return 0;
	}
	char *ptr_msg = msg_buff.get();
	//对转义字符进行恢复
	int32_t restore_len = RestoreHeadTail(buffer+1 , buff_len-2, ptr_msg, buff_len-2);
	//至少24个字节 消息头(22)+校验位(2)
	if (restore_len < 24)
	{
		return 0;
	}
	uint32_t msg_len;
	memcpy(&msg_len, ptr_msg, 4);
	reverse_dword(msg_len);

	if (msg_len != restore_len+2) {
		return 0;
	}
	uint16_t parity_calc = Utils::crc16(ptr_msg,restore_len-2);

	uint16_t parity;
	memcpy(&parity, ptr_msg + restore_len - 2, 2);
	reverse_word(parity);
	if(parity != parity_calc)
	{
		return 0;
	}
    return 1;
}