#include <stdint.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <set>
#include <memory>
#include <tuple>
#include <json/json.h>
#include <json/writer.h>


using namespace std;


struct RabbitMQObject
{
	typedef unique_ptr<RabbitMQObject> Ptr;
	virtual ~RabbitMQObject() {};
	virtual Json::Value ToJson() const = 0;
	virtual string GetName() const = 0;
	virtual RabbitMQObject *Clone() const = 0;
};





struct  RabbitMQAiTemp : RabbitMQObject
{
	string _wid;
	//车辆id
	int _vehicle_id;
	//经度
	double _lon;
	//纬度
	double _lat;
	//冷机状态 0:关 1:开 其他:无'
	int _cold_status;
	//温度1
	double _t1;
	//温度2
	double _t2;
	//温度3
	double _t3;
	//温度4
	double _t4;
	//温度1记录时间
	time_t _t1_time;
	//温度2记录时间
	time_t _t2_time;
	//温度3记录时间
	time_t _t3_time;
	//温度4记录时间
	time_t _t4_time;
	//gps的时间;注意格式必须为:/Date(1488244333000+0800)/
	time_t _gps_time;
	//ps的更新时间;注意格式必须为:/Date(1488244333000+0800)/	
	time_t _update_date;
	vector<double> _vec_temp;
	RabbitMQAiTemp()
		:_lon(0)
		, _lat(0)
		, _t1(0)
		, _t2(0)
		, _t3(0)
		, _t4(0)
		, _t1_time(0)
		, _t2_time(0)
		, _t3_time(0)
		, _t4_time(0)
		, _gps_time(0)
		, _update_date(0)
	{

	}
	string GetName() const;
	Json::Value ToJson() const;
	RabbitMQObject *Clone() const;
};

struct  RabbitMQAiTrackBase : RabbitMQObject
{
	string _wid;
	//车辆id
	int _vehicle_id;
	//经度
	double _lon;
	//纬度
	double _lat;
	//定位状态;0 无,1 1D,2 2D ,3 3D
	int _lo_status;
	//天线状态;0无,1天线正常，2天线断开，3天线短路,4天线状态异常
	int _line_status;
	//ACC状态;0 无,1关,2开
	int _acc_status;
	//设备状态：除了LoStatus,LineStatus,ACCStatus三个状态的其它所有设备状态，并以逗号分隔
	string _status;
	//车辆行使里程数
	double _odometer;
	//行使方向;用角度表示（0~360）
	int _direction;
	//车辆高度
	int _height;
	//车辆行使速度
	int _speed;
	//gps的时间;注意格式必须为:/Date(1488244333000+0800)/
	time_t _gps_time;
	//ps的更新时间;注意格式必须为:/Date(1488244333000+0800)/	
	time_t _update_date;

	int _not_track;
	int _corp_id;
	string _extra_status;
	int _transfer;
	RabbitMQAiTrackBase()
		:_lon(0)
		, _lat(0)
		, _lo_status(0)
		, _line_status(0)
		, _acc_status(0)
		, _odometer(0)
		, _direction(0)
		, _height(0)
		, _speed(0)
		, _gps_time(0)
		, _update_date(0)
		, _not_track(0)
		, _corp_id(0)
		, _transfer(0)
	{

	}

	string GetName() const;
	Json::Value ToJson() const;
	RabbitMQObject *Clone() const;
};



//将温度，湿度，传感器等存入到轨迹中
struct RabbitMQAiTrack
{
	RabbitMQAiTrackBase _base;
	std::unordered_map<string, RabbitMQObject::Ptr>  _properties;
	void  Push(const RabbitMQObject& obj)
	{
		_properties[obj.GetName()] = RabbitMQObject::Ptr(obj.Clone());
	}
	void Push(RabbitMQAiTemp &ai_temp)
	{
		const RabbitMQObject& obj = ai_temp;
		Push(obj);
	}
	Json::Value ToJson() const;
	string ToJsonStr() const;
	bool HasMember(const string& name)const;
};
