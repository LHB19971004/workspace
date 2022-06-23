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
	//����id
	int _vehicle_id;
	//����
	double _lon;
	//γ��
	double _lat;
	//���״̬ 0:�� 1:�� ����:��'
	int _cold_status;
	//�¶�1
	double _t1;
	//�¶�2
	double _t2;
	//�¶�3
	double _t3;
	//�¶�4
	double _t4;
	//�¶�1��¼ʱ��
	time_t _t1_time;
	//�¶�2��¼ʱ��
	time_t _t2_time;
	//�¶�3��¼ʱ��
	time_t _t3_time;
	//�¶�4��¼ʱ��
	time_t _t4_time;
	//gps��ʱ��;ע���ʽ����Ϊ:/Date(1488244333000+0800)/
	time_t _gps_time;
	//ps�ĸ���ʱ��;ע���ʽ����Ϊ:/Date(1488244333000+0800)/	
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
	//����id
	int _vehicle_id;
	//����
	double _lon;
	//γ��
	double _lat;
	//��λ״̬;0 ��,1 1D,2 2D ,3 3D
	int _lo_status;
	//����״̬;0��,1����������2���߶Ͽ���3���߶�·,4����״̬�쳣
	int _line_status;
	//ACC״̬;0 ��,1��,2��
	int _acc_status;
	//�豸״̬������LoStatus,LineStatus,ACCStatus����״̬�����������豸״̬�����Զ��ŷָ�
	string _status;
	//������ʹ�����
	double _odometer;
	//��ʹ����;�ýǶȱ�ʾ��0~360��
	int _direction;
	//�����߶�
	int _height;
	//������ʹ�ٶ�
	int _speed;
	//gps��ʱ��;ע���ʽ����Ϊ:/Date(1488244333000+0800)/
	time_t _gps_time;
	//ps�ĸ���ʱ��;ע���ʽ����Ϊ:/Date(1488244333000+0800)/	
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



//���¶ȣ�ʪ�ȣ��������ȴ��뵽�켣��
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
