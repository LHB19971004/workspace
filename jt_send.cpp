#include "jt_send.h"
#include "format_helper.h"
#include "utils.h"


//login
vector<char> JtLoginReq::toBinary(ProjectVersion version) const
{
	if (version == VERSION_2011)
	{
		FormatHelper packet;
		packet.putUInt32(user_id);
		packet.putString(password, 8);
		packet.putString(slave_ip, 32);
		packet.putUInt16(slave_port);
		return packet.getPacket();
	}
	if (version == VERSION_2019)
	{
		FormatHelper packet;
		packet.putUInt32(user_id);
		packet.putString(password, 8);
		packet.putUInt32(access_code);//下级平台接入码
		packet.putString(slave_ip, 32);
		packet.putUInt16(slave_port);
		return packet.getPacket();
	}
	return vector<char>();
}

//register
vector<char> JtRegister::toBinary(ProjectVersion version) const
{
	if (version == VERSION_2011)
	{
		FormatHelper packet;
		packet.putString(vehicle_no, 21);
		packet.putUInt8(vehicle_color);
		packet.putUInt16(UP_EXG_MSG_REGISTER);
		packet.putUInt32(61);//固定长度
		packet.putString(platform_id, 11);
		packet.putString(producer_id, 11);
		packet.putString(terminal_model_type, 20);
		packet.putString(terminal_id, 7);
		packet.putString(terminal_sim_code, 12);
		return packet.getPacket();
	}
	if (version == VERSION_2019)
	{
		FormatHelper packet;
		packet.putString(vehicle_no, 21);
		packet.putUInt8(vehicle_color);
		packet.putUInt16(UP_EXG_MSG_REGISTER);
		packet.putUInt32(110);//固定长度
		packet.putString(platform_id, 11);
		packet.putString(producer_id, 11);
		packet.putString(terminal_model_type, 30);//车载终端型号
		packet.putString(imel_id, 15);//车载终端通讯模块IMEI码
		packet.putString(terminal_id, 30);//车载终端编号
		packet.putString(terminal_sim_code, 13);
		return packet.getPacket();
	}
	return vector<char>();
}

//Send vehicle static data to the monitoring platform
vector<char> JtStatic::toBinary(ProjectVersion version) const
{
	FormatHelper packet;
	packet.putString(vehicle_no, 21);
	packet.putUInt8(vehicle_color);
	packet.putUInt16(UP_BASE_MSG_VEHICLE_ADDED_ACK);
	//车辆类型
	int32_t car_type = carUseTypeID;
	if (car_type == 64 || car_type > 89)
	{
		car_type = 90;
	}
	//运输行业编号
	string trans_type = transType;
	//车籍地
	string vehicle_org_code = vehicle_nationality;
	if (trans_type.size() == 0)
	{
		trans_type = "021";//货运车
		if (car_type > 9 && car_type < 17)
		{
			//客车
			trans_type = "011";
		}
		else if (car_type == 40)
		{
			//危运车
			trans_type = "031";
		}
		else if (car_type > 50)
		{
			trans_type = "091";
		}
	}
	//经营范围代码		
	string businessscopecode = bussiness_code;
	char bas_inf[256] = { 0 };
	int32_t len = sprintf(bas_inf, "VIN:=%s; VEHICLE_COLOR:=%d; VEHICLE_TYPE:=%d; TRANS_TYPE:=%s; VEHICLE_NATIONALITY:=%s;BUSINESSSCOPECODE:=%s;OWERS_ID:=%d;OWERS_NAME:=%s;OWERS_TEL:=%s",
		vehicle_no.c_str(), vehicle_color, car_type, trans_type.c_str(), vehicle_org_code.c_str(), businessscopecode.c_str(),owers_id,owers_name.c_str(), owers_tel.c_str());
	int32_t data_len = 2 + 4 + len;
	packet.putUInt32(data_len);//后续数据长度
	packet.putUInt16(DOWN_BASE_MSG_VEHICLE_ADDED);
	packet.putString(bas_inf, len);
	return packet.getPacket();
}




//Send the vehicle pos information message
vector<char> JtPos::toBinary(ProjectVersion version) const
{
	if (version == VERSION_2011)//表示2011版本
	{
		FormatHelper packet;
		packet.putString(vehicle_no, 21);
		packet.putUInt8(vehicle_color);
		packet.putUInt16(UP_EXG_MSG_REAL_LOCATION);
		packet.putUInt32(36);//固定长度
		packet.putUInt8(0);//默认不加密
		packet.put809Time(gps_time);
		packet.putUInt32(lon * 1000000);
		packet.putUInt32(lat * 1000000);
		packet.putUInt16(speed);
		packet.putUInt16(speed);
		packet.putUInt32(odometer);
		packet.putUInt16(direction);
		packet.putUInt16(height);
		packet.putUInt32(status);
		packet.putUInt32(alarm);
		return packet.getPacket();
	}
	if (version == VERSION_2019)//表示2019版本
	{
		FormatHelper packet_pos;
		packet_pos.putUInt32(alarm);//报警标志
		packet_pos.putUInt32(status);//状态
		packet_pos.putUInt32(lat * 1000000);
		packet_pos.putUInt32(lon * 1000000);
		packet_pos.putUInt16(height);//高程
		packet_pos.putUInt16(speed*10);
		packet_pos.putUInt16(direction);//方向
		packet_pos.putBcdTime(gps_time);//时间  
		packet_pos.putUInt8(0x01);
		packet_pos.putUInt8(0x04);
		packet_pos.putUInt32(odometer*10);//里程
		auto pos_packet = packet_pos.getPacket();

		FormatHelper packet;
		packet.putString(vehicle_no, 21);
		packet.putUInt8(vehicle_color);
		packet.putUInt16(UP_EXG_MSG_REAL_LOCATION);
		packet.putUInt32(pos_packet.size() + 50);//后续数据长度
		packet.putUInt8(0);//默认不加密
		packet.putUInt32(pos_packet.size());//车辆定位数据长度
		packet.putBuffer(pos_packet);
		packet.putString(platform_id1, 11);//监管平台唯一编码 
		packet.putUInt32(alarm1);//报警状态1
		packet.putString(platform_id2, 11);//市级监管平台唯一编码 
		packet.putUInt32(alarm2);//报警状态2
		packet.putString(platform_id3, 11);//省机监管平台唯一编码 
		packet.putUInt32(alarm3);//报警状态3
		return packet.getPacket();
	}
	return vector<char>();
}
//send vehicle  history data
vector<char> JtHistoryPos::toBinary(ProjectVersion version) const
{
	if (version == VERSION_2011)
	{
		FormatHelper packet;
		packet.putString(vehicle_no, 21);
		packet.putUInt8(vehicle_color);
		packet.putUInt16(UP_EXG_MSG_HISTORY_LOCATION);
		packet.putUInt32(37);//固定长度
		packet.putUInt8(1);//默认一条
		packet.putUInt8(0);//默认不加密
		packet.put809Time(gps_time);
		packet.putUInt32(lon * 1000000);
		packet.putUInt32(lat * 1000000);
		packet.putUInt16(speed);
		packet.putUInt16(speed);
		packet.putUInt32(odometer);
		packet.putUInt16(direction);
		packet.putUInt16(height);
		packet.putUInt32(status);
		packet.putUInt32(alarm);
		return packet.getPacket();
	}
	if (version == VERSION_2019)
	{
		FormatHelper packet_pos;
		packet_pos.putUInt32(alarm);//报警标志
		packet_pos.putUInt32(status);//状态
		packet_pos.putUInt32(lat * 1000000);
		packet_pos.putUInt32(lon * 1000000);
		packet_pos.putUInt16(height);//高程
		packet_pos.putUInt16(speed * 10);
		packet_pos.putUInt16(direction);//方向
		packet_pos.putBcdTime(gps_time);//时间  
		packet_pos.putUInt8(0x01);
		packet_pos.putUInt8(0x04);
		packet_pos.putUInt32(odometer * 10);//里程
		auto pos_packet = packet_pos.getPacket();

		FormatHelper packet;
		packet.putString(vehicle_no, 21);
		packet.putUInt8(vehicle_color);
		packet.putUInt16(UP_EXG_MSG_HISTORY_LOCATION);
		packet.putUInt32(pos_packet.size() + 51);//后续数据长度
		packet.putUInt8(1);//默认1条
		packet.putUInt8(0);//默认不加密
		packet.putUInt32(pos_packet.size());//车辆定位数据长度
		packet.putBuffer(pos_packet);
		packet.putString(platform_id1, 11);//监管平台唯一编码 
		packet.putUInt32(alarm1);//报警状态1
		packet.putString(platform_id2, 11);//市级监管平台唯一编码 
		packet.putUInt32(alarm2);//报警状态2
		packet.putString(platform_id3, 11);//省机监管平台唯一编码 
		packet.putUInt32(alarm3);//报警状态3
		return packet.getPacket();
	}
	return vector<char>();
}

vector<char> JtMsgPostQueryAck::toBinary(ProjectVersion version) const
{
	FormatHelper packet;
	packet.putUInt16(UP_PLATFORM_MSG_POST_QUERY_ACK);
	packet.putUInt32(21 + info_content.size());
	packet.putUInt8(object_type);
	packet.putString(object_id, 12);
	packet.putUInt32(info_id);
	packet.putUInt32(info_content.size());
	packet.putString(info_content);
	return packet.getPacket();
}

vector<char> JtMsgInfoAck::toBinary(ProjectVersion version) const
{
	FormatHelper packet;
	packet.putUInt16(UP_PLATFORM_MSG_INFO_ACK);
	packet.putUInt32(4);
	packet.putUInt32(info_id);
	return packet.getPacket();
}

//超速报警和疲劳驾驶报警
vector<char> WarnInfoGDBY::toBinary(ProjectVersion version) const
{
		FormatHelper body;
		body.putString(platformId1, 11);
		body.putUInt16(warn_type);
		body.putUInt64(warn_time);
		body.putUInt64(event_start_time);
		body.putUInt64(event_end_time);
		body.putString(vehicle_no, 21);
		body.putUInt8(vehicle_color);
		body.putUInt32(0);//线路id
		body.putUInt32(info_content.size());
		body.putString(info_content);
		vector<char> body_buffer = body.getPacket();

		FormatHelper  packet;
		packet.putUInt16(0x1402);
		packet.putUInt32(body_buffer.size());//后续数据长度
		packet.putBuffer(body_buffer);
		return packet.getPacket();
	    return vector<char>();
}

vector<char> ReportICInfo::toBinary(ProjectVersion version)const
{
	if (version == VERSION_2011)
	{
		FormatHelper body;
		body.putString(driver_name, 16);
		body.putString(id_number, 20);
		body.putString(cert, 40);
		body.putString(department, 200);
		vector<char> body_buffer = body.getPacket();

		FormatHelper packet;
		packet.putString(vehicle_no, 21);
		packet.putUInt8(vehicle_color);
		packet.putUInt16(0x120A);
		packet.putUInt32(body_buffer.size());
		packet.putBuffer(body_buffer);
		return packet.getPacket();
	}
	if (version == VERSION_2019)
	{
		FormatHelper body;
		body.putString(driver_name, 16);
		body.putString(id_number, 20);
		body.putString(cert, 20);
		body.putString(department, 200);
		body.putUInt64(expire_time);
		vector<char> body_buffer = body.getPacket();

		FormatHelper packet;
		packet.putString(vehicle_no, 21);
		packet.putUInt8(vehicle_color);
		packet.putUInt16(0x120C);
		packet.putUInt32(body_buffer.size());
		packet.putBuffer(body_buffer);
		return packet.getPacket();
	}
	
	return vector<char>();
}