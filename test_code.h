#include <string>
using namespace std;

class ZBase64
{
public:
    string Encode(const unsigned char* Data,int DataByte);
    string Decode(const char* Data,int DataByte,int& OutByte);
};

string EncodeBase64(const string& str);
string DecodeBase64(const string& str);