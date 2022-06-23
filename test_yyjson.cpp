#include <iostream>
#include "stdafx.h"
#include <direct.h>
#include <fstream>
#include "yyjson.h"

using namespace std;

//读取json字符串
int32_t readYYJsonData(const char* json)
{

	yyjson_doc* doc = yyjson_read(json, strlen(json), 0);
	yyjson_val* root = yyjson_doc_get_root(doc);

	yyjson_val* name = yyjson_obj_get(root, "name");
	const char* strName = yyjson_get_str(name);
	//int32_t name_len = (int)yyjson_get_len(name);

	yyjson_val* age = yyjson_obj_get(root, "age");
	int32_t _age = (int)yyjson_get_int(age);

	yyjson_val* flag = yyjson_obj_get(root, "flag");
	bool isOk = (int)yyjson_get_int(flag);

	////数组的解析
	//yyjson_val*  hits = yyjson_obj_get(root, "hits");
	//yyjson_arr_foreach(this, idx, max, hit)
	//{
	//	printf_s("hit%d:%d \n", (int)idx, (int)yyjson_get_int(hit));
	//}

	return 1;

}


//写入json字符串
int32_t  writeYYJsonData()
{
	yyjson_mut_doc* doc = yyjson_mut_doc_new(NULL);
	yyjson_mut_val* root = yyjson_mut_obj(doc);
	yyjson_mut_doc_set_root(doc, root);

	yyjson_mut_obj_add_str(doc, root, "name", "易流科技");
	yyjson_mut_obj_add_int(doc, root, "age", 4);
	yyjson_mut_obj_add_bool(doc, root, "flag", 1);

	int hits_arr[] = { 2,2,1,3 };
	yyjson_mut_val* hits = yyjson_mut_arr_with_sint32(doc, hits_arr, 4);
	yyjson_mut_obj_add_val(doc, root, "hits", hits);

	const char* json = yyjson_mut_write(doc, 0, NULL);
	readYYJsonData(json);

	return 1;
}


//int main()
//{
//	writeYYJsonData();
//	
//	system("pause");
//	return 1;
//
//
//}