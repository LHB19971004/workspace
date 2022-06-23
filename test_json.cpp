
#include <iostream>
#include "stdafx.h"
#include <direct.h>
#include <fstream>
#include <json/json.h>
#include <json/writer.h>


using namespace std;


int32_t writeJsonData()
{
	Json::Value js_ret;
	Json::FastWriter writer;
	js_ret["regName"] = "ÉÂA12345";
	js_ret["color"] = 2;
	js_ret["lon"] = 119.000000;
	js_ret["lat"] = 23.067200;
	string strJson = writer.write(js_ret);
	ofstream ofs("JSON_DATA\\test.txt", ios::out);
	bool isOk = ofs.is_open();
	ofs << strJson;
	ofs.close();
	return 1;
}

int32_t readJsonData()
{
	Json::Value js_data;
	Json::Reader read;
	ifstream ifs("JSON_DATA\\test.txt", ios::in);
	bool isOk = ifs.is_open();
	if (!ifs.is_open())
	{
		return 0;
	}
	if (read.parse(ifs, js_data))
	{
		if (!js_data["regName"].isString()
			|| !js_data["color"].isNumeric()
			|| !js_data["lon"].isNumeric()
			|| !js_data["lat"].isNumeric())
		{
			return 0;
		}
		string regname = js_data["regName"].asString();
		int8_t color = js_data["color"].asInt();
		int32_t Lon = js_data["lon"].asDouble();
		int32_t Lat = js_data["lat"].asDouble();
	}
	ifs.close();
	return 1;


}





//int main()
//{
//	//writeJsonData();
//	readJsonData();
//
//	system("pause");
//	return 0;
//
//}
