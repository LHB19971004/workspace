#pragma once
#include "std.h"
#include "config.h"

struct Jt809Body;
struct Jt809Frame
{
	uint32_t msg_length;
	uint32_t msg_sn;
	uint16_t msg_id;
	uint32_t center_id;
	uint8_t version[3];
	uint8_t encrypt_flag;
	uint32_t encrypt_key;
	time_t recv_time;
	vector<char> body_buffer;
	shared_ptr<Jt809Body> body;
	uint16_t crc_value;
};

struct Jt809BodySend;
class Jt809Formater
{
public:
	Jt809Formater(const ServerConfig::ProjectConfig& config) :config_(config) {};
	int32_t parse(const char* buff, int32_t buff_len, Jt809Frame& frame);
	int32_t format(const Jt809BodySend* packet_body, boost::scoped_array<char>& buffer, int32_t& buffer_len);
	int32_t format(uint16_t msg_type, const char* body, uint32_t body_len, boost::scoped_array<char>& buffer, int32_t& buffer_len);
private:
	int32_t encode(char *buff, int buff_len);
	int32_t decode(char *buff, int buff_len, uint32_t Key);
private:
	const ServerConfig::ProjectConfig& config_;
};