#pragma once

#include "std.h"
#include "msg_def.h"
struct Jt809BodySend
{
public:
	Jt809BodySend(uint16_t msg_type) : msg_type_(msg_type) {}
	virtual vector<char> toBinary(ProjectVersion version) const = 0;
	uint16_t msg_type_ = 0;
};

struct JtLoginReq: public Jt809BodySend
{
	JtLoginReq(uint32_t id, string pwd, string ip, uint16_t port ,uint32_t accessCode) 
		:user_id(id), password(pwd), slave_ip(ip), slave_port(port) ,access_code(accessCode), Jt809BodySend(UP_CONNECT_REQ){};
	vector<char> toBinary(ProjectVersion version) const;
	uint32_t user_id;
	string password;
	uint32_t access_code;//�¼�ƽ̨������
	string slave_ip;
	uint16_t slave_port;
};

struct JtRegister : public Jt809BodySend
{
	JtRegister() :Jt809BodySend(UP_EXG_MSG) {};
	vector<char> toBinary(ProjectVersion version) const;
	string vehicle_no;
	uint8_t vehicle_color;
	string platform_id;
	string producer_id;
	string terminal_model_type;
	string imel_id;//�����ն�ͨѶģ��IMEI��
	string terminal_id;
	string terminal_sim_code;
};

//Report vehicle static information request reply
struct JtStatic : public Jt809BodySend
{
	JtStatic() :Jt809BodySend(UP_EXG_MSG) {};
	vector<char> toBinary(ProjectVersion version) const;
	string vehicle_no;
	uint8_t vehicle_color;
	uint32_t msg_sn;//�������к�
	string transType;//������ҵ����
	int32_t carUseTypeID;	//��������
	string vehicle_nationality;//������
	string bussiness_code;//��Ӫ��Χ����
	int32_t owers_id;//ҵ��ID
	string owers_name;//ҵ������
	string owers_tel;//ҵ����ϵ�绰
};

struct JtPos: public Jt809BodySend
{
	JtPos():Jt809BodySend(UP_EXG_MSG) {};
	vector<char> toBinary(ProjectVersion version) const;

	string vehicle_no;
	uint8_t vehicle_color;
	time_t gps_time;
	double lat;
	double lon;
	uint16_t speed;
	uint16_t direction;
	uint16_t height;
	double odometer;
	uint32_t status;
	uint32_t alarm;
	string  platform_id1;//���ƽ̨Ψһ����
	uint32_t alarm1 = 0;//����״̬1
	string platform_id2;//�м����ƽ̨Ψһ����
	uint32_t alarm2 = 0;//����״̬2
	string platform_id3;//ʡ�����ƽ̨Ψһ����
	uint32_t alarm3 = 0;//����״̬3
};

struct JtHistoryPos:public JtPos
{
	vector<char> toBinary(ProjectVersion version)  const;
};

//Main link platform message exchange message
struct JtMsgPostQueryAck :public Jt809BodySend
{
	JtMsgPostQueryAck() :Jt809BodySend(UP_PLATFORM_MSG) {};
	vector<char> toBinary(ProjectVersion version) const;

	uint8_t object_type;
	string object_id;
	uint32_t info_id;
	string info_content;
};

struct JtMsgInfoAck :public Jt809BodySend
{
	JtMsgInfoAck() :Jt809BodySend(UP_PLATFORM_MSG) {};
	vector<char> toBinary(ProjectVersion version) const;
	uint32_t info_id;
};
//
struct WarnInfoGDBY : public Jt809BodySend
{
	WarnInfoGDBY() :Jt809BodySend(UP_WARN_MSG) {};
	vector<char> toBinary(ProjectVersion version) const;

	string   vehicle_no;	//����
	int8_t  vehicle_color; //������ɫ
	uint16_t warn_type;	//��������
	time_t warn_time; //����ʱ��
	time_t event_start_time;//�¼���ʼʱ��
	time_t event_end_time;//�¼�����ʱ��
	string platformId1;//������ƽ̨Ψһ����
	uint32_t drv_line_id;//��·id
	string   info_content;//�ϱ�������Ϣ����
};


struct ReportICInfo : public Jt809BodySend
{
	ReportICInfo() : Jt809BodySend(UP_EXG_MSG_REPORT_DRIVER_INFO_ACK) {};
	vector<char> toBinary(ProjectVersion version) const;
	string vehicle_no;	//����
	uint8_t vehicle_color; //������ɫ
	string driver_name;
	string id_number;
	string cert;
	string department;
	uint16_t msg_type;
	time_t expire_time;

};