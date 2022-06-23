#pragma once

#include "std.h"

using namespace boost;
namespace http = beast::http;

class AsyncHTTPClient : public std::enable_shared_from_this<AsyncHTTPClient>
{
public:
	enum Method 
	{
		MT_POST,
		MT_GET
	};
	using Param = std::map<std::string, std::string>;
	using Callback = std::function<void(const std::string&)>;
	
	explicit AsyncHTTPClient(asio::io_service& ioService);
	~AsyncHTTPClient();

	void run(Method method, const std::string &host, uint16_t port, 
		const std::string &target, const Param &param, Callback callback=NULL);
	void run(Method method, const std::string &host, uint16_t port, 
		const std::string &target, const string &js_param, Callback callback = NULL);
private:
	void doResolve();
	void doConnect();
	void doWrite();
	void doRead();
	void doClose();
	std::string encodeParams();
private:
	Method method_;
	std::string host_;
	uint16_t port_;
	std::string target_;
	Param paramMap_;
	std::string js_param_;
	beast::flat_buffer buffer_;
	http::request<http::string_body> request_;
	http::response<http::string_body> response_;
	tcp::resolver resolver_;
	tcp::resolver::results_type results_;
	tcp::socket socket_;
	asio::io_service& ioService_;
	Callback callback_;
};

