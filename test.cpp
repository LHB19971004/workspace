
#include <iostream>
#include "stdafx.h"
#include <direct.h>
#include <fstream>
#include <windows.h>
#include <list>
#include <time.h>
#include <map>
#include <string>
#include <mutex>
#include "struct.h"

using namespace std;
const static string FILE_NAME = "config.json";
static mutex st_mutex;
//
//
//

struct StKks
{
	string wid;
	double lon;
	double lat;
	int32_t vehicle_id;
	string status_str;

};


static map<int32_t, StKks> st_kks_map;
struct RabbitMQMessageProperty
{
	string _exchange;
	string _route_key;
	bool _base64;
	bool _zip_compress;

	//重载小于运算符

	bool operator<(const RabbitMQMessageProperty& ty)const 
	{
		return make_tuple(_exchange, _route_key, _base64, _zip_compress)
			  < make_tuple(ty._exchange, ty._route_key, ty._base64, ty._zip_compress);
	}
	

	RabbitMQMessageProperty()
		:_exchange("exchange"),
		_route_key("#"),
		_base64(false),
		_zip_compress(true)
	{

	}
};



//////写json文件
void  WriteJsonFile()
{
	Json::Value js_ret;
	js_ret["eventID"] = "6F1A663A-6BDB-4ed1-A824-179602F4883C";
	js_ret["eventStatus"] = 1;
	js_ret["attachmentNum"] = 2;
	js_ret["vehicleStatus"] = "9999999";//车辆状态
	Json::Value js_data;
	js_data["altitude"] = 0;
	js_data["direction"] = 331;//航向
	js_data["latitude"] = 26422680;
	js_data["longitude"] = 117781058;
	js_data["speed"] = 5;
	js_ret["GPS"] = js_data;;
	js_ret["eventCategory"] = "ADAS";
	Json::Value js_info;
	js_info["grade"] = 2;
	js_info["type"] = 201;
	js_ret["ADAS"] = js_info;
	Json::FastWriter writer;
	string jsonFile = writer.write(js_ret);
	ofstream ofs;
	ofs.open("166.json");
	bool isExit = ofs.is_open();

	ofs << jsonFile;
	ofs.close();

	int data_len = jsonFile.size();
	int nSize  = 1 + 2 + 4 + data_len;

}

void write_json()
{
    Json::Value js_ret;
    Json::FastWriter write;
    Json::Value js_data;
    double _lon = 23.151990;
    double _lat = 113.353816;
    char str_lon[100] = { 0 };
    char str_lat[100] = { 0 };
    sprintf_s(str_lon, sizeof(str_lon), "%.6f", _lon);
    sprintf_s(str_lat, sizeof(str_lat), "%.6f", _lat);
    js_data["chepai"] = "陕A12345";
    js_data["x"] = str_lon;
    js_data["y"] = str_lat;
    js_ret.append(js_data);

    Json::Value js_Data;
    js_Data["token"] = "d55z4tgbuimas1roz852ig2e0ax180kua57ikqvy3oh0jiay0kdvi98xhgiuzv";
    js_Data["data"] = js_data;

    string _data_info = write.write(js_Data);
    ofstream ofs;
    ofs.open("rr.json", ios::out);
    bool is_ok = ofs.is_open();
    if (!is_ok)
    {
        return ;
    }
    ofs << _data_info;
    ofs.close();

}


int32_t read_json01()
{
    Json::Reader read;
    Json::Value js_value_data;
    ifstream ifs;
    ifs.open("kk.json");
    bool is_exit = ifs.is_open();
    if (!read.parse(ifs, js_value_data, false))
    {
        return 0;
    }
    

    string str_lon = js_value_data["data"]["x"].asString();
    string str_lat = js_value_data["data"]["y"].asString();
    string str_regname = js_value_data["data"]["chepai"].asString();
    string str_token = js_value_data["token"].asString();
 
 
    ifs.close();

}


void ReadJsonFile()
{

	ifstream ifs;
	ifs.open("123.json");
	bool isExit = ifs.is_open();
	Json::Reader read;
	Json::Value root;

	if (!read.parse(ifs, root, false))
	{
		assert(0);
		return;
	}
	string name;
	int age;
	string sex;
	string address;
    int nSize = root.size();
	for (int i = 0; i < nSize; i++)
	{
		name = root["name"][i].asString();
		age =  root[i]["age"].asInt();
		sex = root[i]["sex"].asString();
		address = root[i]["address"].asString();

	}
	read.getFormatedErrorMessages();
	cout << name << " " << age << " " << sex << " " << address;
	ifs.close();

	

   
}

void writeFile()
{
    Json::FastWriter write;
    Json::Value js_data;
    js_data["name"] = "lihongbing";
    js_data["age"] = 23;
    js_data["tephone"] = "15511326097";
    string str_data = write.write(js_data);
    ofstream ofs;
    ofs.open("lihongbing.json", ios::out);
    bool ok = ofs.is_open();
    ofs << str_data;
    ofs.close();

}

#include <string>

void TestRapidjson()
{
    Json::Reader read;
    Json::Value js_data;
    ifstream ifs;
    ifs.open("lihongbing.json", ios::in);
    bool isok = ifs.is_open();
    if (read.parse(ifs, js_data))
    {
        string name = js_data["name"].asString();
        string tel = js_data["tephone"].asString();
        int age = js_data["age"].asInt();

    }
    ifs.close();



}

#if 0
int main(void )
{
    TestRapidjson();
    writeFile();
    ReadJsonFile();//非数组
    ReadJsonFile();//数组
    write_json();
    read_json01();

	system("pause");
	return 0;
}
#endif


void write_file1()
{
    Json::Value sendpos;
    Json::Value  js_array;
    Json::FastWriter write;

    sendpos["name"] = "lhb";
    sendpos["age"] = 23;
    sendpos["address"] = "CHINA";
    js_array.append(sendpos);

    Json::Value _json;
    _json["data"] = js_array;
    _json["id"] = "FAFDSAFISAFNSAFSAF";

    fstream ofs;
    ofs.open("file1.json", ios::out);
    bool isOK = ofs.is_open();
    string dataInfo = write.write(_json);
    ofs << dataInfo << endl;
    ofs.close();
}




void write_file2()
{
    Json::Value js_ret;
    Json::FastWriter write;
    Json::Value js_data;
    double _lon = 23.151990;
    double _lat = 113.353816;
    char str_lon[100] = { 0 };
    char str_lat[100] = { 0 };
    sprintf_s(str_lon, sizeof(str_lon), "%.6f", _lon);
    sprintf_s(str_lat, sizeof(str_lat), "%.6f", _lat);
    js_data["chepai"] = "陕A12345";
    js_data["x"] = str_lon;
    js_data["y"] = str_lat;
    js_ret.append(js_data);

    Json::Value js_net;
    js_net["data"] = js_ret;
    js_net["token"] = "d55z4tgbuimas1roz852ig2e0ax180kua57ikqvy3oh0jiay0kdvi98xhgiuzv";
   

    string _data_info = write.write(js_net);
    ofstream ofs;
    ofs.open("file1001.json", ios::out);
    bool is_ok = ofs.is_open();
    if (!is_ok)
    {
        return;
    }
    ofs << _data_info;
    ofs.close();

}

int32_t read_json()
{
    Json::Reader read;
    Json::Value js_value_data;
    ifstream ifs;
    ifs.open("file1001.json");
    bool is_exit = ifs.is_open();
    if (!read.parse(ifs, js_value_data, false))
    {
        return 0;
    }
    for (int i = 0; i < js_value_data.size(); i++)
    {
        string str_lon = js_value_data["data"][i]["x"].asString();
        string str_lat = js_value_data["data"][i]["y"].asString();
        string str_regname = js_value_data["data"][i]["chepai"].asString();
    }
    string str_token = js_value_data["token"].asString();
 
 
    ifs.close();

}
#include <map>



//Json问题的解释
void READ()
{
	//map<int32_t, std::string> g_map_data;
	Json::Reader read;
	Json::Value js_data;
	ifstream ifs;
	ifs.open("http.json", ios::in);
	bool isok = ifs.is_open();
	if (!read.parse(ifs, js_data))
	{
		return ;
	}


	bool is = js_data.isMember("aiTrack");//判断json中是否有aiTrack这个成员



	auto& ai_track = js_data["aiTrack"];
	auto tt = ai_track["gpsTime"].asDouble()/1000;







}


void test()
{
    Json::Value js_data;
    Json::FastWriter write;
	Json::Value js_info2;
	Json::Value js_info3;
	js_info2.append(123);
	js_info2.append(233);
	js_info2.append(343);
	js_info2.append(235);
    js_data["name"] = js_info2;
	string str = "21";
	string str1 = "22";
	js_info3.append(atoi(str.c_str()));
	js_info3.append(atoi(str1.c_str()));
	js_data["age"] = js_info3;

    string data = write.write(js_data);
	fstream ofs;
	ofs.open("FFFF.json", ios::out);
	bool isOK = ofs.is_open();
	ofs << data << endl;
	ofs.close();


  
}

int32_t data_test()
{
	Json::Value js_ret;
	Json::Value js_data;
	Json::FastWriter writer;
	js_ret["name"] = "yiliu";
	js_data.append(1);
	js_data.append(2);
	js_ret["id"] = js_data;

	Json::Value js_info;
	js_info["AiTrack"] = js_ret;

	string str = writer.write(js_info);
	return 1;
}


int32_t readLocalFileData()
{
	ifstream ifs;
	ifs.open("local.json", ios::in);
	bool isOk = ifs.is_open();
	Json::Value js_data;
	Json::Reader read;
	if (!read.parse(ifs, js_data))
	{
		assert(0);
		return 0;
	}
	if (!js_data.isMember("data"))
	{
		return 0;
	}
	auto t1 = js_data["RoutingKey"].asString();
	auto& data = js_data["data"];
	auto& aiTrack = data["AiTrack"];
	string status = aiTrack["Status"].asString();
	auto corpId = aiTrack["CorpID"].asInt();
	auto gpsTime = aiTrack["GpsTime"].asString();
	return 1;

}

static string UTF8ToGBK(const std::string & strUTF8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
	wchar_t * wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char *szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	//strUTF8 = szGBK;
	std::string strTemp(szGBK);
	delete[]szGBK;
	delete[]wszGBK;
	return strTemp;
}

struct stDevPos
{
	string _rename;
	int _temp_no;
	int _temp_type;
	int _temp_value;

};
static list<stDevPos> list_pos;

int32_t readNewJsonData()
{
	stDevPos dev_pos;
	ifstream ifs;
	ifs.open("new.json", ios::in);
	bool isOk = ifs.is_open();
	Json::Value js_ret;
	Json::Reader read;

	if (!read.parse(ifs, js_ret))
	{
		assert(0);
		return 0;
	}
	if (!js_ret.isMember("TempArr") && !js_ret["TempArr"].isArray())
	{
		return 0;

	}
	dev_pos._rename = js_ret["CustomName"].asString();
	
	auto& result = js_ret["TempArr"];
	for (auto iter = result.begin(); iter != result.end(); iter++)
	{
		auto& dev = *iter;
		dev_pos._temp_no= dev["TempRoute"].asInt();
		dev_pos._temp_type = dev["TempStatus"].asInt();
		dev_pos._temp_value = dev["TempValue"].asDouble();
		list_pos.push_back(dev_pos);
	}



	return 1;
}

string  writeNewJson()
{
	Json::Value js_ret;
	Json::FastWriter write;
	js_ret["customName"] = "沪BBH289";
	js_ret["equipCode"] = "E807BF2341BD";
	js_ret["oprType"] = 1;
	js_ret["lat"] = 30.202222;
	js_ret["lon"] = 120.77920;
	js_ret["lastOpenPlaceName"] = "陕西省西安市软件新城";

	Json::Value js_data;
	//js_data["info"].append(js_ret);
	js_data["info"] = js_ret;

	auto names = js_data.getMemberNames();
	string str;
	for (auto iter = names.begin(); iter != names.end(); iter++)
	{
		str.append("&");
		auto& js_value = js_data[*iter];
		js_value["name"] = "test";
		string test_str = write.write(js_value);
		str.append(test_str);
		str.append("=");
		
	}

	return str;
	/*js_data.append(js_ret);*/
	//auto string = write.write(js_data);


	
}





int32_t makeResult()
{
	Json::Value result;
	Json::FastWriter writer;
	result["RegName"] = "陕A12345";
	auto& gps = result["GPSData"];
	auto& track = gps["AiTrack"];
	track["Wid"] = "15511326097";
	track["VehicleID"] = 10086;
	track["Lon"] = 115.312560;
	track["lat"] = 23.014500;
	track["ACCStatus"] = 2;
	auto& AiTemp = gps["Temp"];
	AiTemp["T1"] = -12.1;
	AiTemp["T2"] = 23.4;
	AiTemp["T3"] = -999;
	AiTemp["T4"] = -999;
	auto& AiHump = gps["Hump"];
	AiHump["H1"] = -12.3;
	AiHump["H2"] = -55;
	AiHump["H3"] = -1;
	AiHump["H4"] = -1;
	result["Message"] = "成功";

	time_t gps_time = time(NULL);
	uint64_t time_stamp = static_cast<uint64_t>(gps_time);
	result["GpsTime"] = to_string((uint64_t)time_stamp);

	vector<string> vec_info{ "aaa","bbb","ccc" };
	auto& track_data = track["testString"];
	for (auto iter : vec_info)
	{
		track_data.append(iter);
	}

	auto stringJson = writer.write(result);
	if (!stringJson.empty() && stringJson.back() == '\n')
	{
		stringJson.pop_back();
	}
	return 1;

}





map<int32_t, StKks> TestKksJson()
{
	StKks kk{ "15511326099",114.4921020, 22.7539740 ,10088 };
	Json::Value js_ret;
	Json::FastWriter writer;
	js_ret["Wid"] = kk.wid;
	js_ret["Lon"] = kk.lon;
	js_ret["Lat"] = kk.lat;
	js_ret["VehicleId"] = kk.vehicle_id;
	kk.status_str = writer.write(js_ret);
	{
		std::lock_guard<std::mutex> _(st_mutex);
		st_kks_map[kk.vehicle_id] = kk;
	}
	return st_kks_map;
}




int32_t TestAitrackJson()
{
	RabbitMQAiTrack ai_track;
	ai_track._base._vehicle_id = 1008;
	ai_track._base._wid = 15511326097;
	ai_track._base._lon = 114.4921020;
	ai_track._base._lat = 22.7539740;
	ai_track._base._height = 400;
	ai_track._base._odometer = 25533;
	ai_track._base._speed = 124;
	ai_track._base._direction = 100;
	ai_track._base._line_status = 0;
	ai_track._base._acc_status = 2;
	ai_track._base._gps_time = time(0);
	ai_track._base._lo_status = 3;
	// system time
	time_t time_sys = time(NULL);
	ai_track._base._update_date = time_sys;
	string status_str;
	status_str.append("测试");
	ai_track._base._status = status_str;


	RabbitMQAiTemp ai_temp;
	ai_temp._wid = 15511326097;
	ai_temp._vehicle_id = 1008;
	ai_temp._lon = 114.4921020;
	ai_temp._lat = 22.7539740;
	ai_temp._cold_status = 0;
	ai_temp._t1 = 12.0;
	ai_temp._t2 = 23.1;
	ai_temp._t3 = -999;
	ai_temp._t4 = -999;
	ai_temp._t1_time = ai_track._base._gps_time;
	ai_temp._t2_time = ai_track._base._gps_time;
	ai_temp._t3_time = ai_track._base._gps_time;
	ai_temp._t4_time = ai_track._base._gps_time;
	ai_temp._gps_time = ai_track._base._gps_time;
	ai_temp._update_date = ai_track._base._update_date;
	ai_track.Push(ai_temp);
	auto str_json = ai_track.ToJsonStr();



	return 1;
}






int main()
{
	TestAitrackJson();



	auto map = TestKksJson();
	for (auto& iter : map)
	{
		cout << iter.second.status_str;
	}


	//makeResult();
   	auto str_data = writeNewJson();


	readNewJsonData();

	readLocalFileData();
	//data_test();

	//test();
	READ();
	//TestRapidjson();

	system("pause");
	return 0;
}












void Test()
{
    _mkdir("app.data");
    Json::Value js_data;
    Json::FastWriter write;
    Json::Value js_data_info;
    js_data["regname"] = "陕A12344";
    js_data["color"] = 1;
    js_data["wid"] = "123232334";
    js_data_info.append(js_data);

    Json::Value js_Data_msg;
    js_Data_msg["DataInfo"] = js_data_info;
    js_Data_msg["token"] = "A23233202lKASFDAF";
    string str_data_msg = write.write(js_Data_msg);
    ofstream ofs;
    ofs.open("app.data\\test.json");
    bool isOk = ofs.is_open();
    ofs << str_data_msg;
    ofs.close();

         
}

void writeJson()
{
    Json::Value js_data;
    Json::FastWriter write;
    Json::Value js_ret;
    js_data["name"] = "lihongbing";
    js_data["age"] = 23;
    js_data["address"] = "gansusheng";
    js_ret.append(js_data);
    Json::Value js_person_info;
    js_person_info["personInfo"] = js_ret;
    js_person_info["token"] = "ASFDAIEUJIJLDSIFJDIJK13ISDA";
    string strjson = write.write(js_person_info);
    std::ofstream ofs;
    ofs.open("test.json");
    if (!ofs.fail())
    {
        ofs << strjson;
    }
    ofs.close();

}


void testJson()
{
    Json::Value js_read_data;
    Json::Reader read;
    ifstream ifs;
    ifs.open("test.json");
    if (!ifs.fail())
    {
        read.parse(ifs, js_read_data);
        int nSize = js_read_data.size();
        for (int i = 0; i < nSize; i++)
        {
            string str_name = js_read_data["personInfo"][i]["name"].asString();
            int age = js_read_data["personInfo"][i]["age"].asInt();
            string str_address = js_read_data["personInfo"][i]["address"].asString();
        }
        string str_token = js_read_data["token"].asString();
    }
    ifs.close();

}

void Test_json()
{
    Json::Value js_ret;
    Json::FastWriter write;


    Json::Value js_data;
    js_data["car_no"] = "测A123456";
    js_data["car_color"] = 1;
    js_data["vin"] = "123456789";
    js_data["install_time"] = "2019-01-01 13:25:00";
    Json::Value js_value;
    js_value.append(js_data);

    Json::Value js_info;
    js_info["data"] = js_value;
    js_ret["app_id"] = "e386879c-a3f7-4499-9b92-ef414d17486f";
    js_ret["sign"] = "e386879c-a3f7-4499-9b92-ef414d17486f";
    js_ret["timestam"] = "123456789";
    string str_json = write.write(js_info);

    ofstream ofs;
    ofs.open("qq.json");
    bool isOk = ofs.is_open();
    ofs << js_ret;
    ofs.close();


}








void readJson()
{
	///*Json::Value js_data;
	//Json::Reader read;
	//ifstream ifs;
	//ifs.open("TEST.json");
	//bool isOk = ifs.is_open();
	//if (read.parse(ifs, js_data, true))
	//{
	//	uint64_t devid  = _strtoi64(js_data["vehicleStatus"].asCString(), 0, 10);
	//	float lon = js_data["GPS"]["longitude"].asDouble();
	//	int type = js_data["BSD"]["type"].asInt();
	//}*/

	//ifs.close();
}


void readJsonData()
{
	//string str = "155113260973242432409204290892323523532543245325";
	//string strWid = str.substr(str.length()-7, str.length());
	//Json::Value js_ret;
	////WriteJsonFile();
	//js_ret["eventID"] =1; //报警id
	//js_ret["eventStatus"] = 2;
	//js_ret["attachmentNum"] = 96068;
	//js_ret["vehicleStatus"] = "202110290956324535";//车辆状态
	//Json::Value js_data;
	//js_data["altitude"] = 55;
	//js_data["direction"] = "设备编码";//航向
	//js_data["latitude"] = 10;
	//js_data["longitude"] = 56;
	//js_data["speed"] = 234;
	//js_ret["GPS"] = js_data;
	//js_ret["eventCategory"] = 3;//报警类别  高级驾驶辅助系统(ADAS),驾驶行为状态监测系统(DSM),盲区监测系统(BSD)
	//Json::Value js_info;
	//js_info["grade"] = 67;
	//js_info["type"] = 1;
	//js_ret["BSD"] = js_info;
	//Json::FastWriter writer;
	//auto strJon = writer.write(js_ret);
	//ofstream ofs;
	//ofs.open("TEST.json");
	//bool isOk = ofs.is_open();
	//ofs << strJon;
	//ofs.close();

}


//int main()
//{
//	
//
//	readJson();
//    system("pause");
//    return 0;
//}