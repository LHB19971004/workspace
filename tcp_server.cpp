#include "tcp_server.h"
#include "app.h"
#include "logging.h"
#include "funs.h"
#include "utils.h"
#include "tcp_connection.h"

TcpServer::TcpServer(boost::asio::io_service& io_service, const std::string &addr, uint16_t port,
	const ServerConfig::ProjectConfig& config, std::weak_ptr<Project> project)
	: config_(config)
	, addr_(addr)
	, port_(port)
	, io_service_(io_service)
	, acceptor_(io_service_, tcp::endpoint(boost::asio::ip::address::from_string(addr), port))
	, socket_(io_service)
	, strand_(io_service)
	, project_(project)
{

}

TcpServer::~TcpServer()
{
}

void TcpServer::run()
{
	doAccept();
}

void TcpServer::stop()
{
	done_ = true;

	boost::system::error_code ec;
	acceptor_.cancel(ec);
	LOG_INFO << "TcpServer stopped";
}

void TcpServer::doAccept()
{
	auto &is = io_service_;
	auto p = std::make_shared<TcpConnection>(io_service_, config_, project_);
	acceptor_.async_accept(p->socket(), [this, p, &is](const boost::system::error_code& ec)
	{
		if (!ec)
		{
			is.post([p]()
			{
				p->start();
			});
		}
		else
		{
			//
			LOG_ERROR << "TcpServer::doAccept failed. " << ec.message();
		}

		if (!done_ && ec != boost::asio::error::operation_aborted)
		{
			doAccept();
		}
	});
}