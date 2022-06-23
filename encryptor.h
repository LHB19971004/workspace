#pragma once
#include "config.h"
class Encryptor
{
public:
	Encryptor(const ServerConfig::ProjectConfig::Encrypt& config);
	~Encryptor();

	int32_t encode(char *buff, int buff_len);
	int32_t decode(char *buff, int buff_len, uint32_t Key);
private:
	ServerConfig::ProjectConfig::Encrypt config_;
};

