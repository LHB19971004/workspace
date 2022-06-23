#include <iostream>
#include "stdafx.h"
#include <direct.h>
#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <windows.h>
#include "json_fun.h"

using namespace rapidjson;
using namespace std;






#ifdef _DEBUG
//解析错误的日志
int32_t ParseErrorLog(const rapidjson::Value& js_ret)
{
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	js_ret.Accept(writer);
	printf_s("错误的日志:%s", buffer.GetString());
	return 1;
}
#endif


int32_t writeRapidJsonData()
{
	/*rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	writer.StartObject();
	writer.Key("ProjectID");
	writer.Int(123);
	writer.Key("CarName");
	writer.String("陕A12345");
	writer.EndObject();
	std::string strJson = buffer.GetString();*/

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	writer.StartObject();

	//int
	writer.Key("ProjectID");
	writer.Int(123);
	//字符串
	writer.Key("RegName");
	writer.String("陕A12345");

	//结构体类型
	writer.Key("stPerson");
	writer.StartObject();
	writer.Key("name");
	writer.String("测试");
	writer.Key("age");
	writer.Int(23);
	writer.EndObject();

	/************ 数组类型 **************/
	
	//整型数组
	writer.Key("IntArray");
	writer.StartArray();
	writer.Int(10);
	writer.Int(20);
	writer.Int(30);
	writer.Int(40);
	writer.EndArray();

	//浮点型数组
	writer.Key("DoubleArray");
	writer.StartArray();
	for (int i = 1; i < 4; i++)
	{
		writer.Double(i*0.1);
	}
	writer.EndArray();

	//字符串数组
	writer.Key("stringArray");
	writer.StartArray();
	writer.String("aaa");
	writer.String("bbb");
	writer.String("ccc");
	writer.EndArray();


	writer.EndObject();
	std::string strRadjson = buffer.GetString();
	ofstream ofs("JSON_DATA\\testRadjson.txt", ios::out);
	bool isOk = ofs.is_open();
	ofs << strRadjson;
	ofs.close();
	return 1;
}

int32_t ParseAiTrack(const rapidjson::Value& js_data)
{
	double lon = 0.0;
	int32_t height = 0;
	if (!js_data.HasMember("RegName") || !js_data.HasMember("DateTerminalID") || !js_data.HasMember("Wid")
		/*|| !js_data.HasMember("LoStatus")*/ /*|| !js_data.HasMember("Direction")*/)
	{
		return 0;
	}
	if (js_data.HasMember("Lon") && js_data["Lon"].IsDouble())
	{
		lon = js_data["Lon"].GetDouble();
	}
	if (js_data.HasMember("Height") && js_data["Height"].IsInt())
	{
		height = js_data["Height"].GetInt();
	}
	string wid = js_data["Wid"].GetString();

	return 1;
}

int32_t ParseMQPos(const rapidjson::Value& js_ret)
{
	if (!js_ret.HasMember("AiTrack"))
	{
		ParseErrorLog(js_ret);
		return 0;
	}
	Value::ConstMemberIterator iter;
	iter = js_ret.FindMember("AiTrack");
	if (iter != js_ret.MemberEnd())
	{
		const Value& ai_track = iter->value;
		if (!ParseAiTrack(ai_track))
		{
			assert(0);
			return 0;
		}
	}
	return 1;
}

int32_t ProcessRapidJson(const string& data)
{
	/*if (!data.empty() && data.front() == '{')
	{
		Document sub;
		if (sub.Parse(UTF8ToGBK(data).c_str())!= NULL)
		{
			try
			{
				ParseMQPos(sub);
			}
			catch (std::bad_alloc &)
			{
				assert(0);
				return 0;
			}
			return 1;
		}
	}*/

	return 1;

}

int32_t writeAiTrackData()
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	writer.StartObject();

	writer.Key("AiTrack");
	writer.StartObject();
	writer.Key("RegName");
	writer.String("陕A12345");
	writer.Key("DateTerminalID");
	writer.Int(123);
	writer.Key("Status");
	writer.String("停运");
	writer.Key("Speed");
	writer.Int(100);
	writer.Key("VehicleID");
	writer.Int(10086);
	writer.Key("Lon");
	writer.Double(114.340488);
	writer.Key("Lat");
	writer.Double(23.01266);
	writer.Key("Wid");
	writer.String("13140489645");
	writer.Key("Height");
	writer.Int(100);
	writer.EndObject();
	writer.EndObject();
	string strJson = buffer.GetString();
	//读rapidJson数据
	ProcessRapidJson(strJson);
	return 1;



}


int32_t testJsonDataInfo()
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	writer.StartObject();

	writer.Key("DevInfo");
	writer.StartObject();
	writer.Key("name");
	writer.String("aaa");
	writer.Key("age");
	writer.Int(24);
	writer.Key("address");
	writer.String("sfsafsafsa");
	writer.EndObject();
	writer.EndObject();
	string str_json = buffer.GetString();

	return 1;
}

//int main()
//{
//	//writerapidjsondata();
//	//writeaitrackdata();
//	testJsonDataInfo();
//
//	system("pause");
//	return 1;
//}