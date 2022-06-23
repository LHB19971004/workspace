#include "encryptor.h"

Encryptor::Encryptor(const ServerConfig::ProjectConfig::Encrypt& config):config_(config)
{
}

Encryptor::~Encryptor()
{
}

int32_t Encryptor::encode(char *buff, int buff_len)
{
	return decode(buff, buff_len, config_.encrypt_key);
}

int32_t Encryptor::decode(char *buff, int buff_len, uint32_t key)
{
	uint32_t& M1 = config_.m1;
	uint32_t& IA1 = config_.IA1;
	uint32_t& IC1 = config_.IC1;

	//用N模伪随机序列发生器产生伪随机字节序列    
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