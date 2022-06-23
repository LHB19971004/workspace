#include "jt_frame.h"
#include "factory.h"
#include "funs.h"
#include "parse_helper.h"
#include "logging.h"
#include "jt_body.h"
#include "format_helper.h"
#include "utils.h"
#include "jt_send.h"

template <class Child, typename ...Args>
std::shared_ptr<Jt809Body> createJt809Body(Args...args) { return std::make_shared<Child>(args...); }

#define ADD_CLASS(class_name) {class_name::getMsgType(), createJt809Body<class_name>}

static Factory<int32_t, std::shared_ptr<Jt809Body>> jt809_factory({
	ADD_CLASS(Jt809RegisterResponse),
	ADD_CLASS(Jt809DownConnectReq),
	ADD_CLASS(Jt809DownDisconnectReq),
	});

int32_t Jt809Formater::parse(const char* buff, int32_t buff_len, Jt809Frame& frame)
{
	boost::scoped_array<char> msg_buff;
	uint32_t msg_size = buff_len;
	try
	{
		msg_buff.reset(new char[msg_size]);
	}
	catch (bad_alloc &)
	{
		LOG_ERROR << "Jt809Frame parse failed: memory alloc fail";
		return 0;
	}
	char *ptr_msg = msg_buff.get();

	//��ת���ַ����лָ�
	int32_t restore_len = RestoreHeadTail(buff + 1, buff_len - 2, ptr_msg, msg_size);
	try
	{
		ParseHelper parser(ptr_msg, restore_len);
		frame.msg_length = parser.getUInt32();
		frame.msg_sn = parser.getUInt32();
		frame.msg_id = parser.getUInt16();
		frame.center_id = parser.getUInt32();
		frame.version[0] = parser.getUInt8();
		frame.version[1] = parser.getUInt8();
		frame.version[2] = parser.getUInt8();
		frame.encrypt_flag = parser.getUInt8();
		frame.encrypt_key = parser.getUInt32();
		//����26�ֽڣ�ͷ��1��+��Ϣͷ��22��+У�飨2��+β��1��
		auto body_len = frame.msg_length - 26;
		if (config_.project_version == VERSION_2019) {
			frame.recv_time = parser.getUInt64();
			body_len -= 8;
		}
		else {
			frame.recv_time = time(0);
		}
		//������
		frame.body_buffer = parser.getBuffer(body_len);
		if (frame.encrypt_flag)
		{
			decode(frame.body_buffer.data(), frame.body_buffer.size(), frame.encrypt_key);
		}
		frame.crc_value = parser.getUInt16();

		if (frame.body = jt809_factory.create(frame.msg_id))
		{
			ParseHelper parser(frame.body_buffer.data(), frame.body_buffer.size());
			frame.body->parse(config_.project_version, parser);
		}
	}
	catch (ParseException e)
	{
		frame.body.reset();
		LOG_ERROR << "Jt809Frame parse failed: " << e.message() << ", recv: " << MemoryToText(buff, buff_len);
		//todo callstack print
		return 0;
	}
	catch (boost::bad_any_cast& e)
	{
		frame.body.reset();
		LOG_ERROR << "Jt809Frame parse failed(bad_any_cast): " << e.what() << ", recv: " << MemoryToText(buff, buff_len);
		return 0;
	}
	return 1;
}

int32_t Jt809Formater::format(const Jt809BodySend* packet_body, boost::scoped_array<char>& buffer, int32_t& buffer_len)
{
	auto pkt = packet_body->toBinary(config_.project_version);
	return format(packet_body->msg_type_, pkt.data(), pkt.size(), buffer, buffer_len);
}

int32_t Jt809Formater::format(uint16_t msg_type, const char* body, uint32_t body_len, boost::scoped_array<char>& buffer, int32_t& buffer_len)
{
	if (body_len != 0 && body == NULL)
	{
		return 0;
	}
	buffer_len = body_len + 128;
	FormatHelper packet(buffer_len);
	//����26�ֽڣ�ͷ��1��+��Ϣͷ��22��+У�飨2��+β��1��
	auto total_len = body_len + 26;
	if (config_.project_version == VERSION_2019)
	{
		total_len+=8;
	}
	packet.putUInt32(total_len);
	packet.putUInt32(Utils::getSequence());
	packet.putUInt16(msg_type);
	packet.putUInt32(config_.access_code);
	packet.putBuffer(config_.version, 3);
	packet.putUInt8(config_.encrypt.encrypt_flag);
	packet.putUInt32(config_.encrypt.encrypt_key);
	if (config_.project_version == VERSION_2019)
	{
		packet.putUInt64(time(0));
	}
	if (body != NULL)
	{
		if (config_.encrypt.encrypt_flag)
		{
			boost::scoped_array<char> scope_msg_body;
			try
			{
				scope_msg_body.reset(new char[body_len]);
			}
			catch (bad_alloc &)
			{
				return 0;
			}
			char* buff = scope_msg_body.get();
			memcpy(buff, body, body_len);
			encode(buff, body_len);
			packet.putBuffer(buff, body_len);
		}
		else
		{
			packet.putBuffer(body, body_len);
		}
	}
	packet.putCrc16();
	auto pkt = packet.getPacket();
	buffer_len *= 2;

	try
	{
		buffer.reset(new char[buffer_len]);//ת������Ϊ��������
	}
	catch (bad_alloc &)
	{
		return 0;
	}
	char* msg_buffer = buffer.get();
	msg_buffer[0] = 0x5B;
	buffer_len = TransferHeadTail(pkt.data(), pkt.size(), msg_buffer + 1, buffer_len);
	msg_buffer[buffer_len + 1] = 0x5D;
	buffer_len += 2;
	return 1;
}

int32_t Jt809Formater::encode(char *buff, int buff_len)
{
	return decode(buff, buff_len, config_.encrypt.encrypt_key);
}

int32_t Jt809Formater::decode(char *buff, int buff_len, uint32_t key)
{
	uint32_t M1 = config_.encrypt.m1;
	uint32_t IA1 = config_.encrypt.IA1;
	uint32_t IC1 = config_.encrypt.IC1;

	//��Nģα������з���������α����ֽ�����    
	unsigned short idx = 0;
	if (key == 0) key = 1;
	uint32_t m_key = M1;
	if (0 == m_key) key = 1;

	while (idx < buff_len)
	{
		key = IA1 * (key % m_key) + IC1;
		buff[idx++] ^= (unsigned char)((key >> 20) & 0xFF);
	}
	return 1;
}