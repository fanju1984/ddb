#include <curl/curl.h> 
// #include <json/json.h> 
#include <iostream>  
#include <string>
#include <map> 
#include <regex>
#include <vector>
#include <sstream>
#include <cstdlib>

using namespace std;

class ZBase64
{
private:
    //Base64编码解码表
    char* m_Base64_Table;

public:
    //构造
    ZBase64();
    //编码
    string EncodeBase64(const string strSource);
    //解码
    string DecodeBase64(const string strSource);
};
// #include "http://www.cnblogs.com/src/utility/base64.h"
ZBase64::ZBase64(){
    //Base64编码表
    this->m_Base64_Table="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
}
string ZBase64::EncodeBase64(const string strSource){
    /*
    * 以下是操作二进制数时用到的
    * 11111100   0xFC
    * 11000000   0x3
    * 11110000   0xF0
    * 00001111   0xF
    * 11000000   0xC0
    * 00111111   0x3F
    */

    string strEncode;
    char cTemp[4];

    //行长,MIME规定Base64编码行长为76字节
    int LineLength=0;

    for(int i=0;i<strSource.size();i+=3)
    {
    memset(cTemp,0,4);

    cTemp[0]=strSource[i];
    cTemp[1]=strSource[i+1];
    cTemp[2]=strSource[i+2];

    int len=strlen(cTemp);
    if(len==3)
    {
        strEncode+=this->m_Base64_Table[((int)cTemp[0] & 0xFC)>>2];
        strEncode+=this->m_Base64_Table[((int)cTemp[0] & 0x3)<<4 | ((int)cTemp[1] & 0xF0)>>4];
        strEncode+=this->m_Base64_Table[((int)cTemp[1] & 0xF)<<2 | ((int)cTemp[2] & 0xC0)>>6];
        strEncode+=this->m_Base64_Table[(int)cTemp[2] & 0x3F];
        if(LineLength+=4>=76) strEncode+="\r\n";
    }
    else if(len==2)
    {
        strEncode+=this->m_Base64_Table[((int)cTemp[0] & 0xFC)>>2];
        strEncode+=this->m_Base64_Table[((int)cTemp[0] & 0x3 )<<4 | ((int)cTemp[1] & 0xF0 )>>4];
        strEncode+=this->m_Base64_Table[((int)cTemp[1] & 0x0F)<<2];
        if(LineLength+=4>=76) strEncode+="\r\n";
        strEncode+="=";
    }
    else if(len==1)
    {
        strEncode+=this->m_Base64_Table[((int)cTemp[0] & 0xFC)>>2];
        strEncode+=this->m_Base64_Table[((int)cTemp[0] & 0x3 )<<4];
        if(LineLength+=4>=76) strEncode+="\r\n";
        strEncode+="==";
    }
    memset(cTemp,0,4);
    }
    return strEncode;
}

string ZBase64::DecodeBase64(const string strSource){
    //返回值
    string strDecode;
    char cTemp[5];

    for(int i=0;i<strSource.size();i+=4)
    {
    memset(cTemp,0,5);

    cTemp[0]=strSource[i];
    cTemp[1]=strSource[i+1];
    cTemp[2]=strSource[i+2];
    cTemp[3]=strSource[i+3];

    int asc[4];
    for(int j=0;j<4;j++)
    {
        for(int k=0;k<(int)strlen(this->m_Base64_Table);k++)
        {
        if(cTemp[j]==this->m_Base64_Table[k]) asc[j]=k;
        }
    }
    if('='==cTemp[2] && '='==cTemp[3])
    {
        strDecode+=(char)(int)(asc[0] << 2 | asc[1] << 2 >> 6);
    }
    else if('='==cTemp[3])
    {
        strDecode+=(char)(int)(asc[0] << 2 | asc[1] << 2 >> 6);
        strDecode+=(char)(int)(asc[1] << 4 | asc[2] << 2 >> 4);
    }
    else
    {
        strDecode+=(char)(int)(asc[0] << 2 | asc[1] << 2 >> 6);
        strDecode+=(char)(int)(asc[1] << 4 | asc[2] << 2 >> 4);
        strDecode+=(char)(int)(asc[2] << 6 | asc[3] << 2 >> 2);
    }  
    }
    return strDecode;
}


size_t write_data(void *buffer, size_t size, size_t nmemb, void *stream) 
//从etcd里面取得部分数据
{ 
    strncat((char*)stream,(char*)buffer,size*nmemb);
    cout << nmemb*size << endl;
    cout << buffer << endl;
    // cout << stream << endl;
    return nmemb*size; 
} 

string Search_Value(string &dir) //将全部数据合并返回
{
    string etcd_url = "http://localhost:2379/v3/kv/range";
    // string etcd_url = "http://localhost:2379/v3/kv/put";
    // string etcd_url = "http://localhost:2379/v2/keys/"+dir+"?recursive=true";
        char *ss=(char*)etcd_url.c_str();
     CURLcode return_code;  
     cout << ss << endl;
     cout << dir << endl;
    return_code = curl_global_init(CURL_GLOBAL_SSL);  
    cout << "here1" << endl;
    if (CURLE_OK != return_code)  
    {  
        cout << "here2" << endl;
        cerr << "init libcurl failed." << endl;  
        return "";  
    }  
    // 获取easy handle  
    CURL *easy_handle = curl_easy_init();  
    cout << "here3" << endl;
    if (NULL == easy_handle)  
    {  
        cerr << "get a easy handle failed." << endl;  
        curl_global_cleanup();   
  
        return "";  
    }  
    char szJsonData[1024]; 
    strcpy(szJsonData, dir.c_str()); 
    char * buff_p = NULL;  
    char result[5000] = "";
    // 设置easy handle属性  
    struct curl_slist *headers = NULL;
    // headers = curl_slist_append(headers, "Accept: */*");
    // curl_easy_setopt(easy_handle, CURLOPT_HTTPHEADER, headers);//设置头
    curl_slist *plist = curl_slist_append(NULL,   
                "Content-Type:application/json;charset=UTF-8");  
    curl_easy_setopt(easy_handle, CURLOPT_HTTPHEADER, plist);
    curl_easy_setopt(easy_handle, CURLOPT_URL,ss);  
    curl_easy_setopt(easy_handle, CURLOPT_PORT, 2379); 
    curl_easy_setopt(easy_handle, CURLOPT_POSTFIELDS, szJsonData);
    // curl_easy_setopt(easy_handle, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(easy_handle, CURLOPT_VERBOSE, 1L);

    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, &write_data);  
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, &result);  
  
    // 执行数据请求  
    curl_easy_perform(easy_handle);   
  
    // 释放资源  
    curl_easy_cleanup(easy_handle);  
    curl_global_cleanup();  
    string info = result;
    cout << result << endl;
    return info;
}

int main(){
    ZBase64 base64;
    string eccode=base64.EncodeBase64("tjh");
    //输出结果为:aGVsbG8=
    cout << eccode << endl;
    string decode=base64.DecodeBase64("aGVsbG8=");
    // printf("%s",eccode.c_str());
    cout << decode << endl;
    //输出结果为:hello

    cout << "test res" << endl;
    string t = "{\"key\":\""+base64.EncodeBase64("foo")+"\"}";
    // string t = "{\"key\": \""+base64.EncodeBase64("tjh")+"\", \"value\": \"YmFy\"}";
    // string t = "foo";
    string res = Search_Value(t);
    // cout << res << endl;
    // cout << base64.DecodeBase64(res) << endl;
    return 0;
    
}