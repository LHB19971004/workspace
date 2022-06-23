#ifdef _WIN32
#include "odbc_conn.h"
#include "odbc_pool/odbc_pool.h"
#include "odbc_writer/odbc_writer.h"
#include "stdlib.h"
#include "config.h"
#include "utils.h"
#include "logging.h"
#include <odbc_exec/OdbcRead.h>
OdbcProc::OdbcProc()
{
	
}

OdbcProc::~OdbcProc()
{

}

OdbcProc& OdbcProc::instance()
{
	static OdbcProc _;
	return _;
}

int32_t OdbcProc::uninit()
{
	if (!start_)
	{
		return 1;
	}
	thread_pool_.reset();
	OdbcWriterUninit();
	OdbcPoolUninit();
	start_ = false;
	return 1;
}

static int32_t CsdirGetDBConn(const char *tag, COdbConnScoped &db_conn)
{
	return  OdbcPoolGetConn(tag, db_conn, 0);
}

int32_t OdbcProc::init()
{
	const auto& configs = Config::instance().getServerConfig().db_configs;
	if (configs.empty())
	{
		return 1;
	}

	if (!OdbcPoolInit(NULL))
	{
		printf_s("\n\r数据库资源池初始化失败。");
		assert(0);
		return 0;
	}
	for (auto& it : configs)
	{
		const auto& config = it.second;
		auto con_str = Utils::stringToWstring(formatConnStr(config));
		if (!OdbcPoolAdd(config.tag.c_str(), con_str.c_str()))
		{
			assert(0);
			return 0;
		}
	}

	OdbcWriteParam param;
	param._param.db_count = configs.size();
	int32_t index = 0;
	for (auto& it : configs)
	{
		const auto& config = it.second;
		strcpy(param._param.db_tags[index], config.tag.c_str());
		param._param.thread_count[index++] = 1;
	}
	param._param._max_data_type = 0;
	param._param._fn_get_dbconn = CsdirGetDBConn;
	param._param._proc_count = 1000;
	if (!OdbcWriterInit(param))
	{
		return 0;
	}
	thread_pool_ = boost::make_unique<ThreadPool>(4);
	start_ = true;
	return 1;
}

string OdbcProc::formatConnStr(const ServerConfig::DbConfig& config)
{
	//odbc格式
	return string("driver=")
		+ config.provider
		+ string(";server=")
		+ config.server
		+ string(";UID=")
		+ config.user
		+ string(";PWD=")
		+ config.password
		+ string(";DATABASE=")
		+ config.database
		+ string(";");
}

void OdbcProc::getSentry(uint32_t plat_id, Callback callback)
{
	if (!start_)
	{
		return;
	}
	{
		std::lock_guard<std::mutex> _(mutex_);
		if (map_[plat_id].empty())
		{
			return;
		}
	}
	COdbConnScoped scoped_conn;
	if (OdbcPoolGetConn("db_alarm", scoped_conn, 0))
	{
		COdbcStmt odbc_stmt;
		if (!ObdcPoolGetStmt(scoped_conn.GetHdbc(), odbc_stmt))
		{
			return;
		}
		SQLHDBC stmt = odbc_stmt.GetStmt();
		try
		{
			OdbcRead odbc_read(stmt);
			static char sql[] = "{call prPlatFormSentryAnswerInfoSel (?)}";
			odbc_read.ORPrepare(sql);
			odbc_read.ORBindInt(plat_id);
			odbc_read.ORExecute();
			while (odbc_read.ORGetNext())
			{
#define ODBC_GET_STR(odbc, buf) do { uint32_t len = sizeof(buf); if(!odbc.ORGetString(buf, len)) continue; } while(0)

				char tmp[1024] = { 0 };
				ODBC_GET_STR(odbc_read, tmp);
				auto msg_id = atoi(tmp);
				memset(tmp, 0, sizeof(tmp));

				ODBC_GET_STR(odbc_read, tmp);
				string msg_str = tmp;

				
				if (callback)
				{
					JtMsgPostQueryReq req;
					{
						std::lock_guard<std::mutex> _(mutex_);
						auto& query_map = map_[plat_id];
						auto it = query_map.find(req.info_id);
						if (it == query_map.end())
						{
							return;
						}
						req = it->second;
						query_map.erase(it);
					}
					callback(req.object_type, req.object_id, req.info_id, msg_str);
				}
			}
		}
		catch (OREcept &e)
		{
			LOG_ERROR << __FUNCTION__ << " failed, " << e._msg << ", code " << e._code << ", " << e._state_str;
		}
	}
}

void OdbcProc::getSentryAsync(uint32_t plat_id, Callback callback)
{
	if (!start_)
	{
		return;
	}
	{
		std::lock_guard<std::mutex> _(mutex_);
		if (map_[plat_id].empty())
		{
			return;
		}
	}
	auto task = std::bind(&OdbcProc::getSentry, this, plat_id, callback);
	thread_pool_->post(task);
}

int32_t OdbcProc::writeSentryInfo(uint32_t plat_id, const string& plat_name, const JtMsgPostQueryReq& req)
{
	if (!start_)
	{
		return 0;
	}
	char db_char[20] = "db_alarm";
	char(*db_tags)[20];
	db_tags = &db_char;
	OdbcQueryInfor query_infor;
	query_infor._plat_id = plat_id;
	if (plat_name.size() < sizeof(query_infor._plat_name))
	{
		strcpy((char*)query_infor._plat_name, plat_name.c_str());
	}
	query_infor._query_id = req.info_id;
	if (req.info_content.size() < sizeof(query_infor._query_str))
	{
		strcpy((char*)query_infor._query_str, req.info_content.c_str());
	}
	
	if (!OdbcWriterQueryInfor(1, db_tags, query_infor, NULL, 0))
	{
		return 0;
	}
	std::lock_guard<std::mutex> _(mutex_);
	map_[plat_id].insert({ req.info_id , req });
	return 1;
}

#endif