#include "msg_proc.h"
#include "project.h"
#include "msg_def.h"
#include "jt_body.h"
#include "odbc_conn.h"
#include "jt_send.h"
#include "format_helper.h"
#include "utils.h"
#include "funs.h"

MsgProc::MsgProc(const ServerConfig::ProjectConfig& config, std::weak_ptr<Project> project)
	: config_(config)
	, project_(project)
{
}

MsgProc::~MsgProc()
{

}

void MsgProc::proc(const Jt809Frame& frame)
{
	switch (frame.msg_id)
	{
	case DOWN_CONNECT_REQ:
	{
		char result = 0;
		sendMsg(DOWN_CONNECT_RSP, &result, 1, false);
	}
	break;
	case DOWN_DISCONNECT_REQ:
	{
		sendMsg(DOWN_DISCONNECT_RSP, NULL, 0, false);
	}
	break;
	case DOWN_LINKTEST_REQ:
	{
		sendMsg(DOWN_LINKTEST_RSP, NULL, 0, false);
	}
	break;
	case DOWN_EXG_MSG:           //从链路动态信息交互
	case DOWN_PLATFORM_MSG:		//从链路平台间消息交互消息
	case DOWN_WARN_MSG:			//从链路报警交互消息
	case DOWN_CTRL_MSG:			//从链路车辆监管消息
	case DOWN_BASE_MSG:			//从链路静态信息交互消息
	{
		procSub(frame);
	}
	break;
	default:
		break;
	}
}

void MsgProc::procSub(const Jt809Frame& frame)
{
	if (!frame.body)
	{
		return;
	}
	auto sub_body = frame.body->getSubBody();
	if (!sub_body)
	{
		return;
	}
	auto sub_type = frame.body->getSubType();

	switch (sub_type)
	{
	case DOWN_PLATFORM_MSG_POST_QUERY_REQ:
	{
		const JtMsgPostQueryReq* sub = dynamic_cast<JtMsgPostQueryReq*>(sub_body.get());
		onPlatQuery(sub);
	}
	break;
	case DOWN_PLATFORM_MSG_INFO_REQ:
	{
		const JtMsgInfoReq* sub = dynamic_cast<JtMsgInfoReq*>(sub_body.get());
		JtMsgInfoAck ack;
		ack.info_id = sub->info_id;
		sendMsg(&ack);
	}
	break;
	}
}

int32_t MsgProc::sendMsg(const Jt809BodySend* packet_body, bool main_link /* = true*/)
{
	if (auto project = project_.lock())
	{
		return project->sendMsg(packet_body, main_link);
	}
	return 0;
}

int32_t MsgProc::sendMsg(uint16_t msg_type, const char* body, uint32_t body_len, bool main_link/* = true*/)
{
	if (auto project = project_.lock())
	{
		return project->sendMsg(msg_type, body, body_len, main_link);
	}
	return 0;
}

void MsgProc::onPlatQuery(const JtMsgPostQueryReq* query)
{
#ifdef _WIN32
	//write db
	OdbcProc::instance().writeSentryInfo(config_.project_id, config_.project_name, *query);
#endif
}