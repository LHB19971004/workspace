#pragma once

enum NotifyState {
	E_TCP_CONNECT,
	E_TCP_DISCONNECT,
};

enum MsgId
{
	UP_CONNECT_REQ = 0x1001,	//����·��½������Ϣ 0x1001
	UP_CONNECT_RSP,				//����·��½Ӧ����Ϣ 0x1002
	UP_DISCONNECT_REQ,			//����·ע��������Ϣ 0x1003
	UP_DISCONNECT_RSP,			//����·ע��Ӧ����Ϣ 0x1004
	UP_LINKTEST_REQ,			//����·���ӱ���������Ϣ 0x1005
	UP_LINKTEST_RSP,			//����·���ӱ���Ӧ������ 0x1006
	UP_DISCONNECT_INFORM,		//����·�Ͽ�֪ͨ��Ϣ 0x1007 ����·
	UP_CLOSELINK_INFORM,		//�¼�ƽ̨�����ر���·֪ͨ��Ϣ ����· 0x1008

	DOWN_CONNECT_REQ = 0x9001,	//����·����������Ϣ
	DOWN_CONNECT_RSP,			//����·����Ӧ����Ϣ
	DOWN_DISCONNECT_REQ,		//����·ע��������Ϣ
	DOWN_DISCONNECT_RSP,		//����·ע��Ӧ����Ϣ
	DOWN_LINKTEST_REQ = 0x9005,			//����·���ӱ���������Ϣ
	DOWN_LINKTEST_RSP,			//����·���ӱ���Ӧ����Ϣ
	DOWN_DISCONNECT_INFORM,		//����·�Ͽ�֪ͨ��Ϣ  ����·
	DOWN_CLOSELINK_INFORM,		//�ϼ�ƽ̨�����ر���·֪ͨ��Ϣ ����· 

	UP_EXG_MSG = 0x1200,					//����·��̬��Ϣ������Ϣ
	DOWN_EXG_MSG = 0x9200,	            //����·��̬��Ϣ����
	UP_PLATFORM_MSG = 0x1300,				//����·ƽ̨����Ϣ������Ϣ
	DOWN_PLATFORM_MSG = 0x9300,			//����·ƽ̨����Ϣ������Ϣ
	UP_WARN_MSG = 0x1400,					//����·������Ϣ������Ϣ
	DOWN_WARN_MSG = 0x9400,				//����·����������Ϣ
	UP_CTRL_MSG = 0x1500,
	DOWN_CTRL_MSG = 0x9500,				//����·���������Ϣ
	UP_BASE_MSG = 0x1600,					//����·��̬��Ϣ������Ϣ
	DOWN_BASE_MSG = 0x9600,				//����·��̬��Ϣ������Ϣ
	UP_BASE_MSG_VEHICLE_ADDED_ACK = 0x1601,			//����������̬��ϢӦ��
	DOWN_BASE_MSG_VEHICLE_ADDED = 0x9601,				//����������̬��Ϣ����

	//////////SICHUAN809////
	DOWN_BASE_MSG_COMPANY_ACK = 0X9605,//�����ϱ���·������ҵ��̬��ϢӦ��
	DOWN_BASE_MSG_VEHICLE_ACK = 0X9606, //�����ϱ�������̬��ϢӦ��
	DOWN_BASE_MSG_PRACTITIONERS_ACK = 0X9607, //�����ϱ���ҵ��Ա��˾����̬��ϢӦ��
	/////////////////////////////

	UP_XIAMEN_VEHICLEMNG_MSG = 0x1F00,    //����·���������Ϣ
	DOWN_XIAMEN_VEHICLEMNG_RSP = 0x9F00,   //����·���������Ϣ

	UP_PREVENTION_MSG = 0x1C00,		//����·������ȫ���ܷ��ؽ�����Ϣ 
	DOWN_PREVENTION_MSG = 0x9C00,	//����·������ȫ���ܷ��ؽ�����Ϣ 
};

enum SubCmdId
{
	UP_EXG_MSG_REGISTER = 0x1201,				//�ϴ�����ע����Ϣ
	UP_EXG_MSG_REAL_LOCATION,				//ʵʱ�ϴ�������λ��Ϣ
	UP_EXG_MSG_HISTORY_LOCATION,			//������λ��Ϣ�Զ�����
	UP_EXG_MSG_RETURN_STARTUP_ACK,			//����������λ��Ϣ����Ӧ��
	UP_EXG_MSG_RETURN_END_ACK,				//����������λ��Ϣ����Ӧ��
	UP_EXG_MSG_APPLY_FOR_MONITOR_STARTUP,	//���뽻��ָ��������λ��Ϣ����
	UP_EXG_MSG_APPLY_FOR_MONITOR_END,		//ȡ������ָ��������λ��Ϣ����
	UP_EXG_MSG_APPLY_HISGNSSDATA_REQ,		//����������λ��Ϣ����
	UP_EXG_MSG_REPORT_DRIVER_INFO_ACK,		//�ϱ�������ʻԱ���ʶ����ϢӦ��
	UP_EXG_MSG_TAKE_EWAYBILL_ACK,			//�ϱ����������˵�Ӧ��
	UP_EXG_MSG_EXTEND_REAL_LOCATION = 0x120E, //ʵʱ�ϴ�������չ��λ��Ϣ��Ϣ
	UP_EXG_MSG_EXTEND_HISTORY_LOCATION = 0x120F, //������չ��λ��Ϣ������Ϣ


	DOWN_EXG_MSG_CAR_LOCATION = 0x9202,		//�����ϴ�������λ��Ϣ
	DOWN_EXG_MSG_HISTORY_LOCATION_ARCOSSAREA,//������λ��Ϣ�Զ�����
	DOWN_EXG_MSG_CAR_INFO,//����������̬��Ϣ
	DOWN_EXG_MSG_RETURN_STARTUP,//����������λ��Ϣ��������
	DOWN_EXG_MSG_RETURN_END,//���س�����λ��Ϣ��������
	//DOWN_EXG_MSG_RETURN_STARTDOWN_ACK,		//����������λ��Ϣ����Ӧ��
	//DOWN_EXG_MSG_RETURN_END_ACK,			//����������λ��Ϣ����Ӧ��
	DOWN_EXG_MSG_APPLY_FOR_MONITOR_STARTUP_ACK,//���뽻��ָ��������λ��ϢӦ��
	DOWN_EXG_MSG_APPLY_FOR_MONITOR_END_ACK,		//ȡ������ָ��������λ��ϢӦ��
	DOWN_EXG_MSG_APPLY_HISGNSSDATA_ACK,		//����������λ��ϢӦ��
	DOWN_EXG_MSG_REPORT_DRIVER_INFO,	   //�ϱ�������ʻԱ���ʶ����Ϣ����
	DOWN_EXG_MSG_TAKE_EWAYBILL_REQ,		  //�ϱ����������˵�Ӧ������


	UP_PLATFORM_MSG_POST_QUERY_ACK = 0x1301,	//ƽ̨�����Ϣ
	UP_PLATFORM_MSG_INFO_ACK = 0x1302,		//�·�ƽ̨�䱨��Ӧ��
	
	DOWN_PLATFORM_MSG_POST_QUERY_REQ = 0x9301,//ƽ̨�����Ϣ����
	DOWN_PLATFORM_MSG_INFO_REQ = 0x9302,		//�·�ƽ̨�䱨��Ӧ������

	UP_WARN_MSG_URGE_TODO_ACK = 0x1401,		//��������Ӧ��
	UP_WARN_MSG_INFO = 0x1402,				//�����ϱ�������Ϣ��Ϣ
	UP_WARN_MSG_OPERATION_INFO = 0x1403,		//�����ϱ�����������Ϣ
	UP_WARN_MSG_INFO_GS = 0x1404,				//�����ϱ���ȫ���ܷ��ر�����Ϣ 
	UP_WARN_MSG_FILELIST_AUTO = 0x1407,		//�������������ϱ�



	DOWN_WARN_MSG_URGE_TODO_REQ = 0x9401,		//������������
	DOWN_WARN_MSG_ADPT_INFORM_TIPS = 0x9402,	//����Ԥ��
	DOWN_WARN_MSG_EXG_INORM = 0x9403,			//ʵʱ����������Ϣ

	UP_CTRL_MSG_MONITOR_VEHICLE_ACK = 0x1501,	//�����������Ӧ��
	UP_CTRL_MSG_TAKE_PHOTO_ACK,				//��������Ӧ��
	UP_CTRL_MSG_TEXT_INFO_ACK,				//�·���������Ӧ��
	UP_CTRL_MSG_TACK_TRAVEL_ACK,			//�ϱ�������ʻ��¼Ӧ��
	UP_CTRL_MSG_EMERGENCY_MONITORING_ACK,	//����Ӧ��������ƽ̨Ӧ����Ϣ


	DOWN_CTRL_MSG_MONITOR_VEHICLE_REQ = 0x9501,		//���������������
	DOWN_CTRL_MSG_TAKE_PHOTO_REQ,					//������������
	DOWN_CTRL_MSG_TEXT_INFO,                            //�����ı���Ϣ�·�
	DOWN_CTRL_MSG_TACK_TRAVEL_REQ,					//�ϱ�������ʻ��¼����
	DOWN_CTRL_MSG_EMERGENCY_MONITORING_REQ,			//����Ӧ��������ƽ̨������Ϣ

	//UP_BASE_MSG_VEHICLE_ADDED_ACK=0x1601,			//����������̬��ϢӦ��
	//DOWN_BASE_MSG_VEHICLE_ADDED=0x9601,				//����������̬��Ϣ����

	//����·���������Ϣ
	UP_XIAMEN_VEHICLEMNG_MSG_VEH = 0x1F01,//�ϴ�����������������������Ϣ������
	UP_XIAMEN_VEHICLEMNG_MSG_MAINTENANCE = 0x1F02,//�ϴ�����ά�޼�¼��Ϣ
	UP_XIAMEN_VEHICLEMNG_MSG_COMPONENTCHG = 0x1F03,//�ϴ�������Ҫ����������¼��Ϣ
	UP_XIAMEN_VEHICLEMNG_MSG_GRADE = 0x1F04,//�ϴ������ȼ�������¼��Ϣ
	UP_XIAMEN_VEHICLEMNG_MSG_CHANGE = 0x1F05,//�ϴ����������¼��Ϣ
	UP_XIAMEN_VEHICLEMNG_MSG_TRANSACTION = 0x1F06,//�ϴ������춯��¼��Ϣ
	UP_XIAMEN_VEHICLEMNG_MSG_ACCIDENT = 0x1F07,//�ϴ�������ͨ�¹ʼ�¼��Ϣ
	UP_XIAMEN_VEHICLEMNG_MSG_DRIVERINFO = 0x1F08,//�ϴ���ҵ��Ա��Ϣ��Ϣ
	UP_XIAMEN_VEHICLEMNG_MSG_DRIVERVITAE = 0x1F09,//�ϴ���ҵ��Ա������Ϣ
	UP_XIAMEN_VEHICLEMNG_MSG_DRIVING = 0x1F0A,//�ϴ�������ʻ��ҵ��Աǩ��ǩ����Ϣ��Ϣ
	UP_XIAMEN_VEHICLEMNG_MSG_ILLEGAL = 0x1F0B,//�ϴ�������ʻԱΥ��Υ����ʻ��¼��Ϣ
	UP_XIAMEN_VEHICLEMNG_MSG_DRIVER_REQ = 0x1F0C,//��ҵ��Ա��Ϣ�Ĳ�ѯ����

	//����·���������Ϣ
	DOWN_XIAMEN_VEHICLEMNG_MSG_ACK = 0x9FFF,//�����г��������Ϣ�ϴ���Ӧ��
	UP_XIAMEN_VEHICLEMNG_MSG_DRIVER_ACK = 0x9F0C,//��ҵ��Ա��Ϣ��ѯ����Ӧ��

	UP_PREVENTION_MSG_FILELIST_REQ = 0x1C02,	//�Զ��ϱ�������ȫ���ܷ��ر�������Ŀ¼��Ϣ 
	UP_PREVENTION_MSG_FILELIST_REQ_ACK = 0x1C01,	//������ȫ���ܷ��ر�������Ŀ¼����Ӧ�� 
	DOWN_PREVENTION_MSG_FILELIST_REQ = 0x9C01,	//������ȫ���ܷ��ر�������Ŀ¼���� 
};

enum ProjectVersion
{
	VERSION_2011,
	VERSION_2019,
};