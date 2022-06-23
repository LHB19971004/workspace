#include "jt_sub_body.h"
#include "parse_helper.h"

void JtMsgPostQueryReq::parse(ProjectVersion version, ParseHelper& parser)
{
	object_type = parser.getUInt8();
	object_id = parser.getString(12,false);
	info_id = parser.getUInt32();
	auto info_len = parser.getUInt32();
	info_content = parser.getString(info_len);
}

void JtMsgInfoReq::parse(ProjectVersion version, ParseHelper& parser)
{
	object_type = parser.getUInt8();
	object_id = parser.getString(12, false);
	info_id = parser.getUInt32();
	auto info_len = parser.getUInt32();
	info_content = parser.getString(info_len);
}