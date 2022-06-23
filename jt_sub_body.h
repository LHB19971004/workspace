#pragma once
#include "msg_def.h"
#include "std.h"
#include "parse_helper.h"

struct Jt809SubBody
{
public:
	virtual void parse(ProjectVersion version, ParseHelper& parser) = 0;
};

struct JtMsgPostQueryReq : public Jt809SubBody
{
	virtual void parse(ProjectVersion version, ParseHelper& parser) override;
	static uint16_t getSubype() { return DOWN_PLATFORM_MSG_POST_QUERY_REQ; };
	uint8_t object_type;
	string object_id;
	uint32_t info_id;
	string info_content;
};

struct JtMsgInfoReq : public Jt809SubBody
{
	virtual void parse(ProjectVersion version, ParseHelper& parser) override;
	static uint16_t getSubype() { return DOWN_PLATFORM_MSG_INFO_REQ; };
	uint8_t object_type;
	string object_id;
	uint32_t info_id;
	string info_content;
};
