#ifdef _WIN32
#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib,"SimpleAMQPClient.2.lib")
#pragma comment(lib,"GZipCodec.lib")
#pragma comment(lib, "odbc_exec.lib")
#pragma comment(lib, "odbc_pool.lib")
#pragma comment(lib, "odbc_writer.lib")
#pragma comment(lib,"librdkafkacpp.lib")
#ifdef _DEBUG
#pragma comment(lib, "json_vc71_libmtd.lib")
#else
#pragma comment(lib, "json_vc71_libmt.lib")
#endif
#endif