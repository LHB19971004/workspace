#pragma once
#ifdef _WIN32
#include "std.h"
#include "thread_pool.h"
#include "config.h"
#include "jt_body.h"

class OdbcProc
{
public:
	using Callback = std::function<void(uint8_t, const string&, uint32_t, const string&)>;
	static OdbcProc& instance();
	int32_t init();
	int32_t uninit();
	void getSentry(uint32_t plat_id, Callback callback);
	void getSentryAsync(uint32_t plat_id, Callback callback);

	int32_t writeSentryInfo(uint32_t plat_id, const string& plat_name, const JtMsgPostQueryReq& req);
private:
	OdbcProc();
	~OdbcProc();

	string formatConnStr(const ServerConfig::DbConfig& config);
private:
	std::unique_ptr<ThreadPool> thread_pool_;
	std::map<uint32_t, std::map<uint32_t, JtMsgPostQueryReq>> map_;
	std::mutex mutex_;
	bool start_ = false;
};

#endif