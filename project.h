#pragma once
#include <memory>
#include "config.h"
#include <boost/noncopyable.hpp>
#include "packet_parser.h"
#include "jt_frame.h"
#include "jt_send.h"
class TcpClient;
class MsgService;
class TcpServer;
class TcpConnection;
class DevDataMgr;
class MsgProc;
class Project : public std::enable_shared_from_this<Project>, private boost::noncopyable
{
public:
	Project(boost::asio::io_service& io_service, ServerConfig::ProjectConfig const &config);
	~Project();

	bool run();
	int32_t sendMsg(const Jt809BodySend* body, bool main_link = true);
	int32_t sendMsg(const JtPos& pos, string& raw_data);
	int32_t sendMsg(uint16_t msg_type, const char* body, uint32_t body_len, bool main_link = true);
	int32_t getDevInfo(int32_t vehicle_id, string& vehicle_no, uint8_t& vehicle_color);

	void mainNotify(int32_t tcp_state);
	void mainData(const char* buff, int32_t buff_len);
	void slaveNotify(int32_t tcp_state, shared_ptr<TcpConnection> connection);
	void slaveData(const char* buff, int32_t buff_len);
	void getStatus(uint32_t& up_link, uint32_t& down_link, uint32_t& packet_rate);
	bool isLogin() { return login_; };
private:
	void onFrame(Jt809Frame& frame, const char* buff, int32_t buff_len);
	void onSlaveFrame(Jt809Frame& frame, const char* buff, int32_t buff_len);
private:
	ServerConfig::ProjectConfig config_;
	boost::asio::io_service& io_service_;
	shared_ptr<TcpClient> main_link_;
	unique_ptr<TcpServer> tcp_server_;
	shared_ptr<TcpConnection> slave_link_;
	shared_ptr<MsgService> msg_service_;
	shared_ptr<MsgProc> msg_proc_;
	shared_ptr<DevDataMgr> dev_data_mgr_;
	Jt809Formater formater_;
	bool login_ = false;
};

