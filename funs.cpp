#include "funs.h"
#include <assert.h>
#include <stdlib.h>
#include <boost/scoped_array.hpp>
#include "logging.h"

int32_t byte_dec_to_bcd(uint8_t dec, uint8_t &bcd)
{
	if (dec > 99)
	{
		return 0;
	}
	uint8_t high = dec / 10;
	uint8_t low = dec % 10;
	bcd = high * 16 + low;
	return 1;
}
int32_t byte_bcd_to_dec(uint8_t &dec, uint8_t bcd)
{
	if (bcd > 0x99)
	{
		return 0;
	}
	uint8_t high = bcd / 16;
	uint8_t low = bcd % 16;
	dec = high * 10 + low;
	return 1;
}


int32_t word_dec_to_bcd(uint16_t dec, uint16_t &bcd)
{
	uint8_t bcd_high;
	byte_dec_to_bcd(dec / 100, bcd_high);
	uint8_t bcd_low;
	byte_dec_to_bcd(dec % 100, bcd_low);
	bcd = (bcd_high << 8) & (bcd_low << 0);
	return 1;
}
int32_t word_bcd_to_dec(uint16_t bcd, uint16_t &dec)
{
	uint8_t dec_high;
	byte_bcd_to_dec(dec_high, (bcd & 0xff00) >> 8);
	uint8_t dec_low;
	byte_bcd_to_dec(dec_low, bcd & 0x00ff);
	dec = dec_high * 100 + dec_low;
	return 1;
}




int32_t BytesDec2Bcd(uint8_t *buff, int32_t buff_len)
{
	for (int32_t i = 0; i < buff_len; i++)
	{
		const uint8_t value = buff[i];
		if (!byte_dec_to_bcd(value, buff[i]))
		{
			return 0;
		}
	}
	return 1;
}
int32_t BytesBcd2Dec(uint8_t *buff, int32_t buff_len)
{
	for (int32_t i = 0; i < buff_len; i++)
	{
		const uint8_t value = buff[i];
		if (!byte_bcd_to_dec(buff[i], value))
		{
			return 0;
		}
	}
	return 1;
}


inline char byte4bit_to_ascii(uint8_t byte4bit)
{
	char ret_value = 0;
	if (byte4bit > 9)
	{
		ret_value = static_cast<char>(byte4bit - 10 + 'A');		
	}
	else
	{
		ret_value = static_cast<char>(byte4bit + '0');			
	}
	return ret_value;
}



int32_t memery_to_text(char *text_buff, int32_t text_buff_len,
	const uint8_t *memery, int32_t memery_len)
{
	if (text_buff == NULL
		|| text_buff_len == 0
		|| memery == NULL
		|| memery_len == 0)
	{
		assert(0);
		return 0;
	}
	int32_t offset = 0;
	do
	{
		
		uint8_t high_part = (memery[offset] & 0xf0) >> 4;
		
		uint8_t low_part = memery[offset] & 0x0f;

		text_buff[offset * 2] = byte4bit_to_ascii(high_part);
		text_buff[offset * 2 + 1] = byte4bit_to_ascii(low_part);

		offset++;

		
	} while (offset < memery_len && offset * 2 < text_buff_len - 1);

	
	text_buff[offset * 2] = 0;

	return offset * 2;
}



uint8_t ascii_to_byte4bit(char ascii)
{
	uint8_t byte4bit = 0;
	if (ascii > '9')
	{
		
		byte4bit = static_cast<uint8_t>(ascii + 10 - 'A');
	}
	else
	{
		
		byte4bit = static_cast<uint8_t>(ascii - '0');
	}
	return byte4bit;
}

int32_t text_to_memery(uint8_t *memery, int32_t memery_len,
	const char *text_buff, int32_t text_buff_len)
{
	if (memery == NULL
		|| memery_len == 0
		|| text_buff == 0
		|| text_buff_len == 0)
	{
		return 0;
	}
	int32_t offset = 0;
	do
	{
		uint8_t high_part = ascii_to_byte4bit(text_buff[offset * 2]);
		uint8_t low_part = ascii_to_byte4bit(text_buff[offset * 2 + 1]);
		memery[offset] = (high_part << 4) | low_part;

		offset++;
	} while (offset * 2 < text_buff_len && offset < memery_len);

	return offset;
}


int IsBetween(int Value, int Max, int Min)
{
	if (Value >= Min && Value <= Max)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int IsValidTm(const tm &tmTime)
{
	if(IsBetween(tmTime.tm_mon+1,12,1)
		&&IsBetween(tmTime.tm_mday,31,1)
		&&IsBetween(tmTime.tm_hour,23,0)
		&&IsBetween(tmTime.tm_min,59,0)
		&&IsBetween(tmTime.tm_sec,59,0)
		)
	{
		return 1;
	}else
	{
		return 0;
	}
	return 0;
}





int32_t EscapeBytes(const char *buff, int32_t buff_len,
	char *dest, int32_t dest_size, int32_t &dest_len,
	uint8_t find_chars[], int32_t find_chars_count,
	uint8_t escape_char, uint8_t xor_byte)
{
	if (dest_size < buff_len)
	{
		assert(0);
		return 0;
	}
	int32_t offset = 0;
	for (int32_t i = 0; i < buff_len; i++)
	{
		uint8_t ch = buff[i];
		
		if (-1 != FindByte(reinterpret_cast<const char *>(find_chars), find_chars_count, ch))
		{
			
			
			dest[offset++] = escape_char;
			
			dest[offset++] = ch ^ xor_byte;
		}
		else
		{
			dest[offset++] = ch;
		}
	}
	dest_len = offset;
	return 1;
}





int32_t RecoverBytes(const char *buff, int32_t buff_len,
	char *dest, int32_t dest_size, int32_t &dest_len,
	uint8_t escape_char, uint8_t xor_byte)
{
	if (buff_len > dest_size)
	{
		assert(0);
		return 0;
	}
	int32_t offset = 0;
	for (int32_t i = 0; i < buff_len; i++)
	{
		uint8_t ch = buff[i];
		
		if (ch == escape_char)
		{
			dest[offset++] = buff[++i] ^ xor_byte;
		}
		else
		{
			dest[offset++] = buff[i];
		}
	}
	if (offset > dest_size)
	{
		assert(0);
		return 0;
	}
	dest_len = offset;
	return 1;
}


uint8_t XorParity8(const char *buff, int32_t buff_len)
{
	uint8_t x = 0;
	for (int32_t i = 0; i < buff_len; i++)
	{
		x ^= buff[i];
	}
	return x;
}



int32_t FindByte(const char *buff, int32_t buff_len, uint8_t find_byte)
{
	int32_t index = -1;
	for (int32_t i = 0; i < buff_len; i++)
	{
		uint8_t byte = buff[i];
		if (byte == find_byte)
		{
			index = i;
			break;
		}
	}
	return index;
}
int32_t FindBytes(const char *buff, int32_t buff_len, const uint8_t *bytes, int32_t bytes_len)
{
	int32_t index = -1;
	
	for (int32_t i = 0; i < buff_len - bytes_len + 1; i++)
	{
		if (0 == memcmp(buff + i, bytes, bytes_len))
		{
			index = i;
			break;
		}
	}
	return index;
}

int32_t multi_bcd_to_dec(uint8_t bcd_data[], int32_t bytes)
{
	uint8_t low = 0;
	uint8_t high = 0;
	int32_t result = 0;

	int i = 0;
	for (i = 0; i < bytes; i++)
	{
		if (bcd_data[i] >= 0)
		{

			high = bcd_data[i] / 16;
			low = bcd_data[i] % 16;

			result = result * 10 + high;
			result = result * 10 + low;
		}
	}
	return result;
}

uint64_t MultiBcd2Uint64(void *ptr_bcd, uint8_t byte_num)
{
	uint64_t num = 0xffffffffffffffff;		
	if (byte_num > 10)
	{
		return (num);
	}
	char dev_id[21] = { 0 };
	int32_t  str_dev_len = memery_to_text(dev_id, sizeof(dev_id), reinterpret_cast<const uint8_t *>(ptr_bcd), byte_num);
	num = strtoull(dev_id, 0, 10);
	return (num);
}

uint32_t Uint64DectoBCD(uint64_t num, uint8_t *ptr_bcd, uint8_t buff_len)
{
	int		i;
	uint8_t temp;

	
	if (NULL == ptr_bcd)
	{
		assert(0);
		return 0;
	}

	
	memset(ptr_bcd, 0, buff_len);

	
	for (i = buff_len - 1; i >= 0; i--)
	{
		temp = num % 100;
		ptr_bcd[i] = ((temp / 10) << 4) + ((temp % 10) & 0x0F);
		num /= 100;

		if (0 == num)		
		{
			break;
		}
	}

	return (buff_len - i);
}

int32_t BcdTime2Tm(const char bcd[6], tm &tm_time)
{
	uint8_t dec;
	if (!byte_bcd_to_dec(dec, bcd[0]))
	{
		
		return 0;
	}
	tm_time.tm_year = dec + 100;

	
	if (!byte_bcd_to_dec(dec, bcd[1]))
	{
		
		return 0;
	}
	tm_time.tm_mon = dec - 1;

	
	if (!byte_bcd_to_dec(dec, bcd[2]))
	{
		
		return 0;
	}
	tm_time.tm_mday = dec;

	
	if (!byte_bcd_to_dec(dec, bcd[3]))
	{
		
		return 0;
	}
	tm_time.tm_hour = dec;
	
	if (!byte_bcd_to_dec(dec, bcd[4]))
	{
		
		return 0;
	}
	tm_time.tm_min = dec;
	
	if (!byte_bcd_to_dec(dec, bcd[5]))
	{
		
		return 0;
	}
	tm_time.tm_sec = dec;

	return 1;
}

int32_t TmTime2Bcd(const tm &tm_time, uint8_t bcd[6])
{
	if (tm_time.tm_year < (100))
	{
		
		return 0;
	}
	uint8_t dec;
	
	dec = tm_time.tm_year - 100;
	if (!byte_dec_to_bcd(dec, bcd[0]))
	{
		
		return 0;
	}

	
	dec = tm_time.tm_mon + 1;
	if (!byte_dec_to_bcd(dec, bcd[1]))
	{
		
		return 0;
	}

	
	dec = tm_time.tm_mday;
	if (!byte_dec_to_bcd(dec, bcd[2]))
	{
		
		return 0;
	}

	
	dec = tm_time.tm_hour;
	if (!byte_dec_to_bcd(dec, bcd[3]))
	{
		
		return 0;
	}

	
	dec = tm_time.tm_min;
	if (!byte_dec_to_bcd(dec, bcd[4]))
	{
		
		return 0;
	}

	
	dec = tm_time.tm_sec;
	if (!byte_dec_to_bcd(dec, bcd[5]))
	{
		
		return 0;
	}

	return 1;
}



int32_t JT8FormatFrame(char *pdest_buff, int32_t dest_buff_len,
	uint16_t mes_id, const char dev_bcd[6],
	uint16_t &mes_seq,
	const char *pmes_body_buff, int16_t mes_body_buff_len, bool update_seq /*= true*/)
{
	if (pdest_buff == NULL
		|| dest_buff_len <= mes_body_buff_len)
	{
		assert(0);
		return 0;
	}

	if (mes_body_buff_len >= 0x3ff)
	{
		assert(0);
		return 0;
	}

	
	
	boost::scoped_array<char> sp_buff;
	try
	{
		sp_buff.reset(new char[dest_buff_len]);
	}
	catch (std::bad_alloc)
	{
		assert(0);
		return 0;
	}
	char *pbuff = sp_buff.get();		
	int32_t offset = 0;

	
	reverse_word(mes_id);
	memcpy(pbuff + offset, &mes_id, 2);
	offset += 2;

	
	uint16_t mes_prop = 0;
	mes_prop = mes_body_buff_len;
	reverse_word(mes_prop);
	memcpy(pbuff + offset, &mes_prop, 2);
	offset += 2;

	
	memcpy(pbuff + offset, dev_bcd, 6);
	offset += 6;

	
	static uint16_t st_mes_seq = 0;
	if (update_seq)
	{
		mes_seq = st_mes_seq++;
	}
	uint16_t mes_seq_reversed = mes_seq;
	reverse_word(mes_seq_reversed);
	memcpy(pbuff + offset, &mes_seq_reversed, 2);
	offset += 2;

	

	
	memcpy(pbuff + offset, pmes_body_buff, mes_body_buff_len);
	offset += mes_body_buff_len;

	
	pbuff[offset] = XorParity8(pbuff, offset);
	offset++;
	
	int32_t dest_offset = 0;
	pdest_buff[dest_offset++] = 0x7e;

	
	
	uint8_t find_bytes[] = { 0x7e,0x7d };
	int32_t buff_transfer_len = 0;
	if (!EscapeBytes(pbuff, offset, pdest_buff + dest_offset, dest_buff_len - dest_offset, buff_transfer_len,
		find_bytes, sizeof(find_bytes), 0x7d, 0x7c))
	{
		assert(0);
		return 0;
	}
	
	dest_offset += buff_transfer_len;
	pdest_buff[dest_offset++] = 0x7e;

	if (dest_offset > dest_buff_len)
	{
		assert(0);
		return 0;
	}

	return dest_offset;
}

string MemoryToText(const char *buff, int32_t buff_len)
{
	boost::scoped_array<char> scoped_buff;
	int32_t scoped_buff_size = buff_len * 2 + 1;
	try
	{
		scoped_buff.reset(new char[scoped_buff_size]);
	}catch(std::bad_alloc &)
	{
		assert(0);
		return 0;
	}
	memery_to_text(scoped_buff.get(), scoped_buff_size, (const uint8_t *)buff, buff_len);
	scoped_buff[scoped_buff_size - 1] = 0;
	return scoped_buff.get();
}


int32_t Transfer7D( char *new_buff, const  char *old_buff, int32_t old_len)
{
	int32_t offset=0;
	for(int32_t i=0;i< old_len;i++)
	{
		if(old_buff[i] ==0x7d || old_buff[i] ==0x7e)
		{
			
			new_buff[offset++]=0x7D;
			new_buff[offset++]=old_buff[i] - 0x7c;

		}else
		{
			new_buff[offset++]=old_buff[i];
		}
	}
	return offset;
}


int32_t Restore7D(const  char* src,int32_t buff_len, char* dest)
{
	int32_t i=0;
	int32_t i_dest=0;
	for(;i<buff_len;++i,++i_dest)
	{
		if(*(src+i)==0x7D)
		{
			++i;
			*(dest + i_dest)=*(src + i) + 0x7c;
		}else
		{
			*(dest + i_dest)=*(src + i);
		}
	}

	return i_dest;
}

uint32_t TransferHeadTail(const char *ptr_src, uint32_t src_len, char *ptr_dst, uint32_t dst_size)
{
	

	uint32_t loop;
	uint32_t offset = 0;
	for (loop = 0; loop < src_len; loop++)
	{
		if (0x5a == ptr_src[loop])
		{
			ptr_dst[offset++] = 0x5a;
			ptr_dst[offset++] = 0x02;
		}
		else if (0x5b == ptr_src[loop])
		{
			ptr_dst[offset++] = 0x5a;
			ptr_dst[offset++] = 0x01;
		}
		else if (0x5d == ptr_src[loop])
		{
			ptr_dst[offset++] = 0x5e;
			ptr_dst[offset++] = 0x01;
		}
		else if (0x5e == ptr_src[loop])
		{
			ptr_dst[offset++] = 0x5e;
			ptr_dst[offset++] = 0x02;
		}
		else
		{
			ptr_dst[offset++] = ptr_src[loop];
		}
	}

	if (offset > dst_size)
	{
		offset = 0;
	}

	return (offset);
}

uint32_t RestoreHeadTail(const char* ptr_src, uint32_t src_len, char *ptr_dst, uint32_t dst_size)
{
	if (dst_size < src_len)
	{
		return 0;
	}
	uint32_t i;
	uint32_t recv_len = 0;
	for (i = 0; i < src_len; i++)
	{
		if ((src_len - 1) == i)
		{
			ptr_dst[recv_len++] = ptr_src[i];
			continue;
		}

		if (0x5a == ptr_src[i])
		{
			if (0x01 == ptr_src[i + 1])
			{
				i++;
				ptr_dst[recv_len++] = 0x5b;
			}
			else if (0x02 == ptr_src[i + 1])
			{
				i++;
				ptr_dst[recv_len++] = 0x5a;
			}
			else
			{
				ptr_dst[recv_len++] = ptr_src[i];
			}
		}
		else  if (0x5e == ptr_src[i])
		{
			if (0x01 == ptr_src[i + 1])
			{
				i++;
				ptr_dst[recv_len++] = 0x5d;
			}
			else if (0x02 == ptr_src[i + 1])
			{
				i++;
				ptr_dst[recv_len++] = 0x5e;
			}
			else
			{
				ptr_dst[recv_len++] = ptr_src[i];
			}
		}
		else
		{
			ptr_dst[recv_len++] = ptr_src[i];
		}
	}

	return recv_len;
}
