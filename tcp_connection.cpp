#include "tcp_connection.h"
#include "logging.h"
#include "funs.h"
#include "utils.h"
#include "format_helper.h"
#include "msg_def.h"
#include "project.h"
static const auto CONNECTION_DEADLINE_TOLERANCE = std::chrono::seconds(3*60);

TcpConnection::TcpConnection(boost::asio::io_service &io_service,
	const ServerConfig::ProjectConfig& config, std::weak_ptr<Project> project)
	: config_(config)
	, io_service_(io_service)
	, socket_(io_service)
	, strand_(io_service)
	, timer_(io_service)
	, packet_parser_(std::bind(&TcpConnection::recvData, this, std::placeholders::_1, std::placeholders::_2))
	, project_(project)
{ 

}

TcpConnection::~TcpConnection()
{
	//LOG_INFO << "tcp connection destructed: " << connection_id_;
}

void TcpConnection::doRead()
{	
	auto self = shared_from_this();
	socket_.async_read_some(
		boost::asio::buffer(buffer_, buffer_.size()),
		strand_.wrap([this, self](boost::system::error_code ec,
			std::size_t bytes_transferred)
	{
		if (!ec) {
			//LOG_INFO << __FUNCTION__ << " :" << MemoryToText(buffer_.data(), bytes_transferred);
			packet_parser_.addData(buffer_.data(), bytes_transferred);
			doRead(); 
		}
		else
		{
			doClose();
		}
	}));
}

void TcpConnection::doWrite(const char* buffer, int32_t buffer_len)
{
	//LOG_INFO << "tcp " << connection_id_ << " write: " << MemoryToText(buffer, buffer_len);
	auto self = shared_from_this();
	boost::asio::async_write(
		socket_, boost::asio::buffer(buffer,buffer_len),
		strand_.wrap([this, self](boost::system::error_code ec,
			std::size_t bytes_transferred )
	{
		if (ec)
		{
			doClose();
		}
	}));
}

void TcpConnection::doClose()
{
	if (close_)
	{
		return;
	}
	boost::system::error_code ec;
	socket_.close(ec);
	close_ = true;

	if (valid_)
	{
		if (auto project = project_.lock())
		{
			project->slaveNotify(E_TCP_DISCONNECT, shared_from_this());
		}
	}
}

void TcpConnection::start()
{
	read_time_ = std::chrono::steady_clock::now();
	boost::system::error_code ec;
	timer_.expires_from_now(CONNECTION_DEADLINE_TOLERANCE, ec);
	LOG_ON_ERROR(ec);
	auto self = shared_from_this();
	timer_.async_wait(strand_.wrap(std::bind(&TcpConnection::onTimer, self, std::placeholders::_1)));
	io_service_.post(strand_.wrap(
		[self,this]()
		{
			doRead();
		}
	));
}

void TcpConnection::stop()
{
	doClose();
}

void TcpConnection::onTimer(boost::system::error_code ec)
{
	if (!ec)
	{
		auto timeGap = std::chrono::steady_clock::now() - read_time_;
		if (timeGap > CONNECTION_DEADLINE_TOLERANCE)
		{
			return doClose();
		}

		timer_.expires_from_now(CONNECTION_DEADLINE_TOLERANCE, ec);
		LOG_ON_ERROR(ec);
		timer_.async_wait(strand_.wrap(std::bind(&TcpConnection::onTimer, shared_from_this(), std::placeholders::_1)));
	}
}


int32_t TcpConnection::sendData(const char* buffer, int32_t buffer_len)
{
	//LOG_INFO << __FUNCTION__ << " :" << MemoryToText(buffer, buffer_len);
	doWrite(buffer, buffer_len);
	return 1;
}

void TcpConnection::recvData(const char* buffer, int32_t buffer_len)
{
	LOG_INFO << __FUNCTION__ << " :" << MemoryToText(buffer, buffer_len);
	read_time_ = std::chrono::steady_clock::now();
	auto project = project_.lock();
	if (!project)
	{
		return;
	}
	if (!valid_)
	{
		valid_ = true;
		project->slaveNotify(E_TCP_CONNECT, shared_from_this());
	}
	project->slaveData(buffer, buffer_len);
}