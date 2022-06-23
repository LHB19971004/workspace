#pragma once
#include "std.h"
#include "config.h"
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>
#include "project.h"

class Project;
struct DevParam
{
	int32_t dataTerminalId;
	string wid;
	string vehicle_no;
	int32_t vehicle_type;
	int32_t vehicle_color;
	string trans_type;//������ҵ����
	string vehicle_nationality;//������
	int32_t owers_id;//ҵ��ID
	string owers_name;//ҵ������
	string owers_tel;//ҵ����ϵ�绰
	string producer_id;//�����ն˳���Ψһ����
	string terminal_model_type;//�����ն��ͺ�
	//boost���л�
	friend class  boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & dataTerminalId;
		ar & wid;
		ar & vehicle_no;
		ar & vehicle_type;
		ar & vehicle_color;
		ar & trans_type;
		ar & vehicle_nationality;
		ar & owers_id;
		ar & owers_name;
		ar & owers_tel;
		ar & producer_id;
		ar & terminal_model_type;
	}
};

class DevDataMgr : public std::enable_shared_from_this<DevDataMgr>
{
public:
	DevDataMgr(boost::asio::io_service& io_service, const ServerConfig::ProjectConfig& config,std::weak_ptr<Project> project);
	~DevDataMgr();

	void run();
	void stop();
	int32_t getDevInfo(int32_t vehicle_id, DevParam& param);
	int32_t getDevInfo(int32_t vehicle_id, string& vehicle_no, uint8_t& vehicle_color);
	std::list<JtRegister> GetDevRegiterInfo();
private:
	void save();
	void load();
	void onHeartBeat();
	void sendQueryTotal();
	void sendQueryList(uint32_t count);
	void httpCallback(const string& response);
	int32_t updateDevInfo(DevParam& param, int32_t is_valid);
	string getFileName();
	JtRegister fomartRegister(const DevParam& param);
private:
	boost::asio::io_service& io_service_;
	boost::asio::io_service::strand strand_;
	boost::asio::steady_timer timer_;
	ServerConfig::HttpPlat config_;
	ServerConfig::ProjectConfig config_info_;
	time_t timestamp_ = 0;
	std::mutex mutex_;
	map<int32_t, DevParam> dev_map_;
	//weak project
	std::weak_ptr<Project> project_;
	string file_name_;
	bool exit_ = false;
	std::list<JtRegister> reg_list_;
};

