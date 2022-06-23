#pragma once
#include "std.h"
#include "config.h"
#include "packet_parser.h"
#include "jt_frame.h"
#include "msg_def.h"

class Project;
class TcpClient : public std::enable_shared_from_this<TcpClient>
{
public:
	using CallBack = std::function<void(int32_t)>;

	TcpClient(boost::asio::io_service& io_service, const std::string &addr, uint16_t port, 
		const ServerConfig::ProjectConfig& config, std::weak_ptr<Project> project);
	~TcpClient();
	void run();
	void stop();
	void setReconnectTime(int32_t reconnect_time) { reconnect_time_ = reconnect_time; };
	int32_t sendData(const char* buffer, int32_t buffer_len);
private:
	void recvData(const char* buffer, int32_t buffer_len);
	void doWrite(const char* buffer, int32_t buffer_len);
	void doConnect();
	void doRead();
	void doReconnect();
private:
	const ServerConfig::ProjectConfig& config_;
	boost::asio::io_service& io_service_;
	boost::asio::io_service::strand strand_;
	boost::asio::steady_timer timer_;
	tcp::socket socket_;
	tcp::endpoint endpoint_;
	std::array<char, 8192> buffer_;
	bool stop_ = false;
	bool login_ = false;
	int32_t reconnect_time_ = 10;
	PacketParser parser_;
	std::weak_ptr<Project> project_;
};
