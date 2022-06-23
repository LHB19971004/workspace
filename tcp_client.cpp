#include "tcp_client.h"
#include "logging.h"
#include "format_helper.h"
#include "funs.h"
#include "utils.h"
#include "project.h"

TcpClient::TcpClient(boost::asio::io_service& io_service, const std::string &addr, uint16_t port,
	const ServerConfig::ProjectConfig& config, std::weak_ptr<Project> project)
	: config_(config)
	, io_service_(io_service)
	, strand_(io_service_)
	, timer_(io_service_)
	, socket_(io_service_)
	, endpoint_(boost::asio::ip::address::from_string(addr), port)
	, parser_(std::bind(&TcpClient::recvData, this, std::placeholders::_1, std::placeholders::_2))
	, project_(project)
{

}


TcpClient::~TcpClient()
{

}

void TcpClient::run()
{
	doConnect();
}

void TcpClient::stop()
{
	stop_ = true;
	socket_.close();
}

void TcpClient::doConnect()
{
	//if (!socket_.is_open())
	{
		auto self = shared_from_this();
		socket_.async_connect(endpoint_, [this, self](const boost::system::error_code& ec)
		{
			if (!ec)
			{
				if (auto project = project_.lock())
				{
					project->mainNotify(E_TCP_CONNECT);
				}
				doRead();
			}
			else
			{
				LOG_INFO << "TcpClient " << __FUNCTION__ << " error, " << ec.message();
				doReconnect();
			}
		});
	}
}

void TcpClient::doRead()
{
	auto self = shared_from_this();
	socket_.async_read_some(
		boost::asio::buffer(buffer_, buffer_.size()),
		strand_.wrap([this, self](boost::system::error_code ec,
			std::size_t bytes_transferred)
	{
		if (!ec) {
			//LOG_INFO << __FUNCTION__ << " :" << MemoryToText(buffer_.data(), bytes_transferred);
			parser_.addData(buffer_.data(), bytes_transferred);
			doRead();
		}
		else
		{
			LOG_ERROR << "TcpClient " << __FUNCTION__ << " error, " << ec.message();
			doReconnect();
		}
	}));
}

void TcpClient::doWrite(const char* buffer, int32_t buffer_len)
{
	auto self = shared_from_this();
	boost::asio::async_write(
		socket_, boost::asio::buffer(buffer, buffer_len),
		strand_.wrap([this, self](boost::system::error_code ec,
			std::size_t bytes_transferred)
	{
		if (ec)
		{
			LOG_INFO << "TcpClient " << __FUNCTION__ << " error, " << ec.message();
			doReconnect();
		}
	}));
}

void TcpClient::doReconnect()
{
	if (stop_ || !socket_.is_open() || io_service_.stopped())
	{
		return;
	}
	socket_.close();

	if (auto project = project_.lock())
	{
		project->mainNotify(E_TCP_DISCONNECT);
	}

	auto self = shared_from_this();
	timer_.expires_from_now(std::chrono::seconds(reconnect_time_));
	timer_.async_wait(
		strand_.wrap([this, self](boost::system::error_code ec){
		if (!ec)
		{
			LOG_INFO << "Reconnecting!";
			doConnect();
		}
	}));
}

int32_t TcpClient::sendData(const char* buffer, int32_t buffer_len)
{
	//LOG_INFO << __FUNCTION__ << " :" << MemoryToText(buffer, buffer_len);
	doWrite(buffer, buffer_len);
	return 1;
}

void TcpClient::recvData(const char* buffer, int32_t buffer_len)
{
	LOG_INFO << __FUNCTION__ << " :" << MemoryToText(buffer, buffer_len);
	auto project = project_.lock();
	if (project)
	{
		project->mainData(buffer, buffer_len);
	}
}