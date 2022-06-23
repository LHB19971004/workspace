#pragma once
#include "msg_def.h"
#include "std.h"
#include "jt_sub_body.h"
#include "parse_helper.h"
struct Jt809Body
{
public:
	virtual void parse(ProjectVersion version, ParseHelper& parser) = 0;
	virtual shared_ptr<Jt809SubBody> getSubBody() { return nullptr; };
	virtual uint16_t getSubType() { return 0; };
	virtual string getVehicleNo() { return ""; };
	virtual uint8_t getVehicleColor() { return 0; };
};

struct Jt809BodyWithSub : public Jt809Body
{
	virtual shared_ptr<Jt809SubBody> getSubBody() override  { return sub_body; };
	virtual uint16_t getSubType() override  { return sub_type; };
	virtual string getVehicleNo() override { return vehicle_no; };
	virtual uint8_t getVehicleColor() override { return vehicle_color; };
	virtual void parse(ProjectVersion version, ParseHelper& parser) override;

	string vehicle_no;
	uint8_t vehicle_color = 0;
	uint16_t sub_type = 0;
	shared_ptr<Jt809SubBody> sub_body;
};

struct Jt809RegisterResponse : public Jt809Body
{
	void parse(ProjectVersion version, ParseHelper& parser) override;
	static uint16_t getMsgType() { return UP_CONNECT_RSP; };
	uint8_t result;
	uint32_t verify_code;
};

struct Jt809DownConnectReq : public Jt809Body
{
	void parse(ProjectVersion version, ParseHelper& parser) override;
	static uint16_t getMsgType() { return DOWN_CONNECT_REQ; };
	uint32_t verify_code;
};

struct Jt809DownDisconnectReq : public Jt809Body
{
	void parse(ProjectVersion version, ParseHelper& parser) override;
	static uint16_t getMsgType() { return DOWN_DISCONNECT_REQ; };
	uint32_t verify_code;
};

struct Jt809DownExecMsg : public Jt809BodyWithSub
{
	static uint16_t getMsgType() { return DOWN_EXG_MSG; };
};


struct Jt809DownPlatformMsg : public Jt809BodyWithSub
{
	virtual void parse(ProjectVersion version, ParseHelper& parser) override;
	static uint16_t getMsgType() { return DOWN_PLATFORM_MSG; };
};

struct Jt809DownWarnMsg : public Jt809BodyWithSub
{
	static uint16_t getMsgType()  { return DOWN_WARN_MSG; };
};

struct Jt809DownBaseMsg : public Jt809BodyWithSub
{
	static uint16_t getMsgType()  { return DOWN_BASE_MSG; };
};

struct Jt809DownCtrlMsg : public Jt809BodyWithSub
{
	static uint16_t getMsgType()  { return DOWN_CTRL_MSG; };
};
