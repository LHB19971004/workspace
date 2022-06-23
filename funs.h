#pragma once
#include "std.h"

inline void reverse_word(uint16_t &x)
{
	x = (int16_t)(((x & 0xff00) >> 8) | ((x & 0x00ff) << 8));
}

inline void reverse_dword(uint32_t &x)
{
	x = ((x & 0xff000000) >> 24) | ((x & 0x00ff0000) >> 8) | ((x & 0x0000ff00) << 8) | ((x & 0x000000ff) << 24);
}

inline void reverse_uint64(uint64_t& arg64)
{
	uint32_t low = (uint32_t) (arg64 & 0x00000000FFFFFFFFLL);
	uint32_t high = (uint32_t) ((arg64 & 0xFFFFFFFF00000000LL) >> 32);
	reverse_dword(low);
	reverse_dword(high);
	arg64 = (uint64_t) high + (((uint64_t) low) << 32);
}


inline void inl_memcpy_data(void *dst, const char *src, int32_t &offset, int32_t size)
{
	memcpy(dst, src + offset, size);
	offset += size;
}

inline void inl_memcpy_data_2dst(void *dst, int32_t &offset, const void *src, int32_t size)
{
	memcpy((char *)dst + offset, src, size);
	offset += size;
}



int32_t byte_dec_to_bcd(uint8_t dec, uint8_t &bcd);
int32_t byte_bcd_to_dec(uint8_t &dec, uint8_t bcd);


int32_t word_dec_to_bcd(uint16_t dec, uint16_t &bcd);
int32_t word_bcd_to_dec(uint16_t bcd, uint16_t &dec);





int32_t BytesDec2Bcd(uint8_t *buff, int32_t buff_len);
int32_t BytesBcd2Dec(uint8_t *buff, int32_t buff_len);


inline char byte4bit_to_ascii(uint8_t byte4bit);



int32_t memery_to_text(char *text_buff, int32_t text_buff_len,
	const uint8_t *memery, int32_t memery_len);


int32_t text_to_memery(uint8_t *memery, int32_t memery_len,
	const char *text_buff, int32_t text_buff_len);

uint8_t ascii_to_byte4bit(char ascii);


int IsBetween(int Value, int Max, int Min);
int IsValidTm(const tm &tmTime);





int32_t EscapeBytes(const char *buff, int32_t buff_len,
	char *dest, int32_t dest_size, int32_t &dest_len,
	uint8_t find_chars[], int32_t escape_chars_count,
	uint8_t escape_char, uint8_t xor_byte);





int32_t RecoverBytes(const char *buff, int32_t buff_len,
	char *dest, int32_t dest_size, int32_t &dest_len,
	uint8_t escape_char, uint8_t xor_byte);


uint8_t XorParity8(const char *buff, int32_t buff_len);



int32_t FindByte(const char *buff, int32_t buff_len, uint8_t byte);
int32_t FindBytes(const char *buff, int32_t buff_len, const uint8_t *bytes, int32_t bytes_len);

int32_t multi_bcd_to_dec(uint8_t bcd_data[], int32_t bytes);
uint64_t MultiBcd2Uint64(void *ptr_bcd, uint8_t byte_num);
uint32_t Uint64DectoBCD(uint64_t num, uint8_t *ptr_bcd, uint8_t buff_len);
int32_t BcdTime2Tm(const char bcd[6], tm &tm_time);
int32_t TmTime2Bcd(const tm &tm_time, uint8_t bcd[6]);



int32_t JT8FormatFrame(char *pdest_buff, int32_t dest_buff_len,
	uint16_t mes_id, const char dev_bcd[6],
	uint16_t &mes_seq,
	const char *pmes_body_buff, int16_t mes_body_buff_len, bool update_seq = true);

string MemoryToText(const char *buff, int32_t buff_len);

int32_t Transfer7D( char *new_buff, const  char *old_buff, int32_t old_len);
int32_t Restore7D(const  char* src,int32_t buff_len, char* dest);
uint32_t TransferHeadTail(const char *ptr_src, uint32_t src_len, char *ptr_dst, uint32_t dst_size);
uint32_t RestoreHeadTail(const char* ptr_src, uint32_t src_len, char *ptr_dst, uint32_t dst_size);