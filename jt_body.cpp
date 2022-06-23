#include "jt_body.h"
#include "parse_helper.h"
#include "factory.h"
template <class Child, typename ...Args>
std::shared_ptr<Jt809SubBody> createJt809SubBody(Args...args) { return std::make_shared<Child>(args...); }

#define ADD_CLASS(class_name) {class_name::getSubype(), createJt809SubBody<class_name>}

static Factory<int32_t, std::shared_ptr<Jt809SubBody>> jt809sub_factory({
	ADD_CLASS(JtMsgPostQueryReq),
	ADD_CLASS(JtMsgInfoReq),
	});


void Jt809RegisterResponse::parse(ProjectVersion version, ParseHelper& parser)
{
	result = parser.getUInt8();
	verify_code = parser.getUInt32();
};

void Jt809DownConnectReq::parse(ProjectVersion version, ParseHelper& parser)
{
	verify_code = parser.getUInt32();
};

void Jt809DownDisconnectReq::parse(ProjectVersion version, ParseHelper& parser)
{
	verify_code = parser.getUInt32();
};

void Jt809DownPlatformMsg::parse(ProjectVersion version, ParseHelper& parser)
{
	sub_type = parser.getUInt16();
	auto sub_len = parser.getUInt32();
	if (sub_body = jt809sub_factory.create(sub_type)) {
		sub_body->parse(version, parser);
	}
};

void Jt809BodyWithSub::parse(ProjectVersion version, ParseHelper& parser)
{
	vehicle_no = parser.getString(21);
	vehicle_color = parser.getUInt8();
	sub_type = parser.getUInt16();
	auto sub_len = parser.getUInt32();
	if (sub_body = jt809sub_factory.create(sub_type)) {
		sub_body->parse(version, parser);
	}
};