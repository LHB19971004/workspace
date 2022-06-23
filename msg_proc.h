#pragma once
#include "std.h"
#include "jt_frame.h"
#include "msg_def.h"
#include "jt_sub_body.h"
#include "config.h"

class Project;
class MsgProc :public std::enable_shared_from_this<MsgProc>
{
public:
	MsgProc(const ServerConfig::ProjectConfig& config, std::weak_ptr<Project> project);
	~MsgProc();

	void proc(const Jt809Frame& frame);
private:
	void procSub(const Jt809Frame& frame);
	void onPlatQuery(const JtMsgPostQueryReq* query);
	int32_t sendMsg(const Jt809BodySend* packet_body, bool main_link = true);
	int32_t sendMsg(uint16_t msg_type, const char* body, uint32_t body_len, bool main_link = true);
private:
	ServerConfig::ProjectConfig config_;
	std::weak_ptr<Project> project_;
};
