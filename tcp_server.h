#pragma once
#include "std.h"
#include "config.h"
#include "jt_frame.h"

class Project;
class TcpServer : public std::enable_shared_from_this<TcpServer>
{
public:
	TcpServer(boost::asio::io_service& io_service, const std::string &addr, uint16_t port,
		const ServerConfig::ProjectConfig& config, std::weak_ptr<Project> project);
	~TcpServer();

	void run();
	void stop();
private:
	void doAccept();
private:
	const ServerConfig::ProjectConfig& config_;
	boost::asio::io_service& io_service_;
	bool done_ = false;
	tcp::acceptor acceptor_;
	tcp::socket socket_;
	boost::asio::io_service::strand strand_;
	std::string addr_;
	uint16_t port_;
	std::weak_ptr<Project> project_;
};
