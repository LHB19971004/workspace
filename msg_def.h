#pragma once

enum NotifyState {
	E_TCP_CONNECT,
	E_TCP_DISCONNECT,
};

enum MsgId
{
	UP_CONNECT_REQ = 0x1001,	//主链路登陆请求信息 0x1001
	UP_CONNECT_RSP,				//主链路登陆应答信息 0x1002
	UP_DISCONNECT_REQ,			//主链路注销请求消息 0x1003
	UP_DISCONNECT_RSP,			//主链路注销应答消息 0x1004
	UP_LINKTEST_REQ,			//主链路连接保持请求消息 0x1005
	UP_LINKTEST_RSP,			//主链路连接保持应答请求 0x1006
	UP_DISCONNECT_INFORM,		//主链路断开通知消息 0x1007 从链路
	UP_CLOSELINK_INFORM,		//下级平台主动关闭链路通知消息 从链路 0x1008

	DOWN_CONNECT_REQ = 0x9001,	//从链路连接请求消息
	DOWN_CONNECT_RSP,			//从链路连接应答消息
	DOWN_DISCONNECT_REQ,		//从链路注销请求消息
	DOWN_DISCONNECT_RSP,		//从链路注销应答消息
	DOWN_LINKTEST_REQ = 0x9005,			//从链路连接保持请求信息
	DOWN_LINKTEST_RSP,			//从链路连接保持应答消息
	DOWN_DISCONNECT_INFORM,		//从链路断开通知消息  主链路
	DOWN_CLOSELINK_INFORM,		//上级平台主动关闭链路通知消息 主链路 

	UP_EXG_MSG = 0x1200,					//主链路动态信息交换消息
	DOWN_EXG_MSG = 0x9200,	            //从链路动态信息交互
	UP_PLATFORM_MSG = 0x1300,				//主链路平台间消息交互消息
	DOWN_PLATFORM_MSG = 0x9300,			//从链路平台间消息交互消息
	UP_WARN_MSG = 0x1400,					//主链路报警消息交互消息
	DOWN_WARN_MSG = 0x9400,				//从链路报警交互消息
	UP_CTRL_MSG = 0x1500,
	DOWN_CTRL_MSG = 0x9500,				//从链路车辆监管消息
	UP_BASE_MSG = 0x1600,					//朱链路静态信息交互消息
	DOWN_BASE_MSG = 0x9600,				//从链路静态信息交互消息
	UP_BASE_MSG_VEHICLE_ADDED_ACK = 0x1601,			//补报车辆静态消息应答
	DOWN_BASE_MSG_VEHICLE_ADDED = 0x9601,				//补报车辆静态消息请求

	//////////SICHUAN809////
	DOWN_BASE_MSG_COMPANY_ACK = 0X9605,//主动上报道路运输企业静态信息应答
	DOWN_BASE_MSG_VEHICLE_ACK = 0X9606, //主动上报车辆静态信息应答
	DOWN_BASE_MSG_PRACTITIONERS_ACK = 0X9607, //主动上报从业人员即司机静态信息应答
	/////////////////////////////

	UP_XIAMEN_VEHICLEMNG_MSG = 0x1F00,    //主链路车辆监管消息
	DOWN_XIAMEN_VEHICLEMNG_RSP = 0x9F00,   //从链路车辆监管消息

	UP_PREVENTION_MSG = 0x1C00,		//主链路主动安全智能防控交互消息 
	DOWN_PREVENTION_MSG = 0x9C00,	//从链路主动安全智能防控交互消息 
};

enum SubCmdId
{
	UP_EXG_MSG_REGISTER = 0x1201,				//上传车辆注册消息
	UP_EXG_MSG_REAL_LOCATION,				//实时上传车辆定位信息
	UP_EXG_MSG_HISTORY_LOCATION,			//车辆定位信息自动补传
	UP_EXG_MSG_RETURN_STARTUP_ACK,			//启动车辆定位信息交换应答
	UP_EXG_MSG_RETURN_END_ACK,				//结束车辆定位信息交互应答
	UP_EXG_MSG_APPLY_FOR_MONITOR_STARTUP,	//申请交换指定车辆定位信息请求
	UP_EXG_MSG_APPLY_FOR_MONITOR_END,		//取消交换指定车辆定位信息请求
	UP_EXG_MSG_APPLY_HISGNSSDATA_REQ,		//补发车辆定位信息请求
	UP_EXG_MSG_REPORT_DRIVER_INFO_ACK,		//上报车辆驾驶员身份识别消息应答
	UP_EXG_MSG_TAKE_EWAYBILL_ACK,			//上报车辆电子运单应答
	UP_EXG_MSG_EXTEND_REAL_LOCATION = 0x120E, //实时上传车辆扩展定位信息消息
	UP_EXG_MSG_EXTEND_HISTORY_LOCATION = 0x120F, //车辆扩展定位信息补报消息


	DOWN_EXG_MSG_CAR_LOCATION = 0x9202,		//交换上传车辆定位信息
	DOWN_EXG_MSG_HISTORY_LOCATION_ARCOSSAREA,//车辆定位信息自动补传
	DOWN_EXG_MSG_CAR_INFO,//交换车辆静态信息
	DOWN_EXG_MSG_RETURN_STARTUP,//启动车辆定位信息交换请求
	DOWN_EXG_MSG_RETURN_END,//介素车辆定位信息交换请求
	//DOWN_EXG_MSG_RETURN_STARTDOWN_ACK,		//启动车辆定位信息交换应答
	//DOWN_EXG_MSG_RETURN_END_ACK,			//结束车辆定位信息交互应答
	DOWN_EXG_MSG_APPLY_FOR_MONITOR_STARTUP_ACK,//申请交换指定车辆定位信息应答
	DOWN_EXG_MSG_APPLY_FOR_MONITOR_END_ACK,		//取消交换指定车辆定位信息应答
	DOWN_EXG_MSG_APPLY_HISGNSSDATA_ACK,		//补发车辆定位信息应答
	DOWN_EXG_MSG_REPORT_DRIVER_INFO,	   //上报车辆驾驶员身份识别消息请求
	DOWN_EXG_MSG_TAKE_EWAYBILL_REQ,		  //上报车辆电子运单应答请求


	UP_PLATFORM_MSG_POST_QUERY_ACK = 0x1301,	//平台查岗信息
	UP_PLATFORM_MSG_INFO_ACK = 0x1302,		//下发平台间报文应答
	
	DOWN_PLATFORM_MSG_POST_QUERY_REQ = 0x9301,//平台查岗信息请求
	DOWN_PLATFORM_MSG_INFO_REQ = 0x9302,		//下发平台间报文应答请求

	UP_WARN_MSG_URGE_TODO_ACK = 0x1401,		//报警督办应答
	UP_WARN_MSG_INFO = 0x1402,				//主动上报报警信息消息
	UP_WARN_MSG_OPERATION_INFO = 0x1403,		//主动上报报警处理消息
	UP_WARN_MSG_INFO_GS = 0x1404,				//主动上报安全智能防控报警消息 
	UP_WARN_MSG_FILELIST_AUTO = 0x1407,		//报警附件主动上报



	DOWN_WARN_MSG_URGE_TODO_REQ = 0x9401,		//报警督办请求
	DOWN_WARN_MSG_ADPT_INFORM_TIPS = 0x9402,	//报警预警
	DOWN_WARN_MSG_EXG_INORM = 0x9403,			//实时交换报警信息

	UP_CTRL_MSG_MONITOR_VEHICLE_ACK = 0x1501,	//车辆单项监听应答
	UP_CTRL_MSG_TAKE_PHOTO_ACK,				//车辆拍照应答
	UP_CTRL_MSG_TEXT_INFO_ACK,				//下发车辆报文应答
	UP_CTRL_MSG_TACK_TRAVEL_ACK,			//上报车辆行驶记录应答
	UP_CTRL_MSG_EMERGENCY_MONITORING_ACK,	//车辆应急接入监管平台应答消息


	DOWN_CTRL_MSG_MONITOR_VEHICLE_REQ = 0x9501,		//车辆单项监听请求
	DOWN_CTRL_MSG_TAKE_PHOTO_REQ,					//车辆拍照请求
	DOWN_CTRL_MSG_TEXT_INFO,                            //车辆文本信息下发
	DOWN_CTRL_MSG_TACK_TRAVEL_REQ,					//上报车辆行驶记录请求
	DOWN_CTRL_MSG_EMERGENCY_MONITORING_REQ,			//车辆应急接入监管平台请求消息

	//UP_BASE_MSG_VEHICLE_ADDED_ACK=0x1601,			//补报车辆静态消息应答
	//DOWN_BASE_MSG_VEHICLE_ADDED=0x9601,				//补报车辆静态消息请求

	//主链路车辆监管消息
	UP_XIAMEN_VEHICLEMNG_MSG_VEH = 0x1F01,//上传车辆技术档案基础档案消息数据体
	UP_XIAMEN_VEHICLEMNG_MSG_MAINTENANCE = 0x1F02,//上传车辆维修记录消息
	UP_XIAMEN_VEHICLEMNG_MSG_COMPONENTCHG = 0x1F03,//上传车辆主要部件更换记录消息
	UP_XIAMEN_VEHICLEMNG_MSG_GRADE = 0x1F04,//上传车辆等级评定记录消息
	UP_XIAMEN_VEHICLEMNG_MSG_CHANGE = 0x1F05,//上传车辆变更记录消息
	UP_XIAMEN_VEHICLEMNG_MSG_TRANSACTION = 0x1F06,//上传车辆异动记录消息
	UP_XIAMEN_VEHICLEMNG_MSG_ACCIDENT = 0x1F07,//上传车辆交通事故记录消息
	UP_XIAMEN_VEHICLEMNG_MSG_DRIVERINFO = 0x1F08,//上传从业人员信息消息
	UP_XIAMEN_VEHICLEMNG_MSG_DRIVERVITAE = 0x1F09,//上传从业人员履历消息
	UP_XIAMEN_VEHICLEMNG_MSG_DRIVING = 0x1F0A,//上传车辆驾驶从业人员签到签退信息消息
	UP_XIAMEN_VEHICLEMNG_MSG_ILLEGAL = 0x1F0B,//上传车辆驾驶员违规违法驾驶记录消息
	UP_XIAMEN_VEHICLEMNG_MSG_DRIVER_REQ = 0x1F0C,//从业人员信息的查询请求

	//从链路车辆监管消息
	DOWN_XIAMEN_VEHICLEMNG_MSG_ACK = 0x9FFF,//厦门市车辆监管信息上传的应答
	UP_XIAMEN_VEHICLEMNG_MSG_DRIVER_ACK = 0x9F0C,//从业人员信息查询请求应答

	UP_PREVENTION_MSG_FILELIST_REQ = 0x1C02,	//自动上报主动安全智能防控报警附件目录消息 
	UP_PREVENTION_MSG_FILELIST_REQ_ACK = 0x1C01,	//主动安全智能防控报警附件目录请求应答 
	DOWN_PREVENTION_MSG_FILELIST_REQ = 0x9C01,	//主动安全智能防控报警附件目录请求 
};

enum ProjectVersion
{
	VERSION_2011,
	VERSION_2019,
};