#include "async_http_client.h"
#include "logging.h"

AsyncHTTPClient::AsyncHTTPClient(asio::io_service& ioService)
	: ioService_(ioService)
	, socket_(ioService)
	, resolver_(ioService)
{
}


AsyncHTTPClient::~AsyncHTTPClient()
{
}

void AsyncHTTPClient::run(Method method, const std::string& host, uint16_t port, 
	const std::string& target, const Param& param, Callback callback)
{
	method_ = method;
	host_ = host;
	port_ = port;
	target_ = target;
	paramMap_ = param;
	callback_ = callback;
	auto self = shared_from_this();
	ioService_.post([self, this]()
	{
		doResolve();
	});
}

void AsyncHTTPClient::run(Method method, const std::string &host, uint16_t port,
	const std::string &target, const string &js_param, Callback callback)
{
	method_ = method;
	host_ = host;
	port_ = port;
	target_ = target;
	js_param_ = js_param;
	callback_ = callback;
	auto self = shared_from_this();
	ioService_.post([self, this]()
	{
		doResolve();
	});
}

void AsyncHTTPClient::doResolve()
{
	auto self = shared_from_this();
	resolver_.async_resolve(tcp::resolver::query(host_, std::to_string(port_)), [self, this](const boost::system::error_code & ec, tcp::resolver::results_type results)
	{
		if (ec)
		{
			LOG_ERROR << "AsyncHTTPClient unable to resolve host : " << host_ <<  ": " << ec.message();
			doClose();
		}
		else
		{
			results_ = std::move(results);
			doConnect();
		}
	});
}

void AsyncHTTPClient::doConnect()
{
	auto self = shared_from_this();
	asio::async_connect(socket_, results_, [self, this](const boost::system::error_code & ec, const tcp::endpoint & endpoint)
	{
		if (ec)
		{
			LOG_ERROR << "AsyncHTTPClient unable to connect to host : " << host_ << ": " << ec.message();
			doClose();
		}
		else
		{
			doWrite();
		}
	});
}

void AsyncHTTPClient::doWrite()
{
	auto self = shared_from_this();
	auto verb = method_ == Method::MT_GET ? http::verb::get : http::verb::post;
	request_.method(verb);
	request_.target(target_);
	request_.set(http::field::host, host_);
	request_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
	if(js_param_.empty())
	{
		request_.set(http::field::content_type, "application/x-www-form-urlencoded");
		if (paramMap_.size())
		{
			request_.body() = encodeParams();
		}
	}
	else
	{
		request_.set(http::field::content_type, "application/json");
		request_.body() = js_param_;
	}
	
	request_.prepare_payload();

	http::async_write(socket_, request_, [self, this](const boost::system::error_code & ec, std::size_t bytes_transferred)
	{
		if (ec)
		{
			LOG_ERROR << "AsyncHTTPClient unable to make request : " << ec.message();
			doClose();
		}
		else
		{
			// get response
			doRead();
		}
	});
}

void AsyncHTTPClient::doRead()
{
	auto self = shared_from_this();
	http::async_read(socket_, buffer_, response_, [self, this](const boost::system::error_code & ec, std::size_t bytes_transferred)
	{
		if (ec)
		{
			LOG_ERROR << "AsyncHTTPClient unable to read response : " << ec.message();		
		}
		else
		{
			if (callback_)
				callback_(response_.body());
			// call handler
			//LOG_INFO << "AsyncHTTPClient response: " << response_.result_int() << " " << response_.body();
		}
		doClose();
	});
}

void AsyncHTTPClient::doClose()
{
	boost::system::error_code ec;
	socket_.shutdown(tcp::socket::shutdown_both, ec);

	// ?
	socket_.close(ec);
}

std::string AsyncHTTPClient::encodeParams()
{
	auto str = std::string();
	for (auto param : paramMap_)
	{
		auto elem = param.first + '=' + param.second;
		if (!str.empty())
		{
			str += "&";
		}
		str += elem;
	}
	LOG_INFO << "AsyncHTTPClient encodeParams: " << str;
	return str;
}
