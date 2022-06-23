#pragma once
#include "std.h"
#include "packet_parser.h"
#include "jt_frame.h"

class Project;
class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
	TcpConnection(boost::asio::io_service &io_service,
			const ServerConfig::ProjectConfig& config, std::weak_ptr<Project> project);
	~TcpConnection();
	tcp::socket &socket() { return socket_; }
	void start();
	void stop();
	int32_t sendData(const char* buffer, int32_t buffer_len);
private:
	void recvData(const char* buffer, int32_t buffer_len);
	void doWrite(const char* buffer, int32_t buffer_len);
	void doRead();
	void doClose();
	void onTimer(boost::system::error_code ec);
private:
	const ServerConfig::ProjectConfig& config_;
	boost::asio::io_service& io_service_;
	tcp::socket socket_;
	boost::asio::io_service::strand strand_;
	boost::asio::steady_timer timer_;
	std::chrono::steady_clock::time_point read_time_;
	std::array<char, 8192> buffer_;
	bool close_ = false;
	PacketParser packet_parser_;
	std::weak_ptr<Project> project_;
	bool valid_ = false;
};
