#pragma once
#if _WIN32
#include <sdkddkver.h>
#endif
// c++ headers
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <thread>
#include <atomic>
#include <stdint.h>
#include <iostream>
#include <tuple>
#include <queue>
#include <memory>
#include <unordered_map>
#include <set>
#include <condition_variable>
#include <mutex>

// boost headers
#include <boost/asio.hpp>
#include <boost/make_unique.hpp>
//#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
//#include <boost/beast/websocket/ssl.hpp>
#include <boost/endian/conversion.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/asio/detail/thread_group.hpp>
#include "boost/scoped_array.hpp"

#ifdef _WIN32
#include <sql.h>
#include <sqlext.h>
#include <odbcss.h>
#endif
using namespace std;
using tcp = boost::asio::ip::tcp;
using udp = boost::asio::ip::udp;

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define FILE_LOCATION __FILE__ ": " STR(__LINE__)
#define LOG_ON_ERROR(ec) do { if (ec) {LOG_ERROR << FILE_LOCATION << " " << ec.message();} } while(0)