#include "metadata.h"

#define URL_MAX_LEN 50  
#define VALUE_LEN 1024

//etcd v3 json's key&value need to be base64
//#include "http://www.cnblogs.com/src/utility/base64.h"
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

vector<string> split2list(string info){
    stringstream sstr(info);
    string token;
    vector<string> res;
    while(getline(sstr,token,',')){
        res.push_back(token);
    }
    return res;
}

vector<string> getTables(){
    //返回创建的所有表格
    string key = "/gdd_table";
    string op = "GET";
    key = toJson(key);
    string res  = etcd_op(key,op);
    string v = get_value_from_json(res);
    vector<string> re = split2list(v);
    return re;
}
vector<string> getTableAttri(string tableName) {
    //返回表的attri
    string key = "/gdd_table/"+tableName;
    string op = "GET";
    key = toJson(key);
    string res  = etcd_op(key,op);
    string v = get_value_from_json(res);
    vector<string> re = split2list(v);
    return re;
}
string getTableAttriType(string tableName, string attriName){
    string key = "/gdd_table/"+tableName+"/"+attriName;
    string op = "GET";
    key = toJson(key);
    string res  = etcd_op(key,op);
    string v = get_value_from_json(res);
    return v;
}
string getTableAttriDesc(string tableName, string attriName){
    string key = "/gdd_table/"+tableName+"/desc/"+attriName;
    string op = "GET";
    key = toJson(key);
    string res  = etcd_op(key,op);
    string v = get_value_from_json(res);
    return v;
}
string getTableKey(string tableName){
    string key = "/gdd_key/"+tableName;
    string op = "GET";
    key = toJson(key);
    string res  = etcd_op(key,op);
    string v = get_value_from_json(res);
    return v;
}
string getTableFragType(string tableName){
    string key = "/part_schema/"+tableName;
    string op = "GET";
    key = toJson(key);
    string res  = etcd_op(key,op);
    string v = get_value_from_json(res);
    return v;
}
vector<string> getTableFragH(string tableName){
    string key = "/part_schema/"+tableName+"/H";
    string op = "GET";
    key = toJson(key);
    string res  = etcd_op(key,op);
    string v = get_value_from_json(res);
    vector<string> re = split2list(v);
    return re;
}
int getTableFragNum(string tableName){
    string key = "/part_info/"+tableName;
    string op = "GET";
    key = toJson(key);
    string res  = etcd_op(key,op);
    string v = get_value_from_json(res);
    char *p = (char*)v.c_str();
    return atoi(p);
}
string getTableFragCondition(string tableName, int index){
    string key = "/part_info/"+tableName+"/"+tableName+"."+to_string(index);
    string op = "GET";
    key = toJson(key);
    string res  = etcd_op(key,op);
    string v = get_value_from_json(res);
    return v;
}
string getTableFragCol(string tableName, int index){
    string key = "/part_column/"+tableName+"."+to_string(index);
    string op = "GET";
    key = toJson(key);
    string res  = etcd_op(key,op);
    cout << res << endl;
    string v = get_value_from_json(res);
    return v;
}

int getTableFragSize(string tableName,int index){
    string key = "/part_size/"+tableName+"."+to_string(index);
    string op = "GET";
    key = toJson(key);
    string res  = etcd_op(key,op);
    string v = get_value_from_json(res);
    char *p = (char*)v.c_str();
    return atoi(p);
}
int getTableFragSite(string tableName,int index){
    string key = "/part_site/"+tableName+"."+to_string(index);
    string op = "GET";
    key = toJson(key);
    string res  = etcd_op(key,op);
    string v = get_value_from_json(res);
    char *p = (char*)v.c_str();
    return atoi(p);
}
string toJson(string k){
    ZBase64 base64;
    string kk = "{\"key\":\""+base64.EncodeBase64(k)+"\"}";
    return kk;
}

bool saveTableToEtcd(GDD table){
    // add table
    vector<string> exist_tables = getTables();
    exist_tables.push_back(table.name);
    string k0 = "/gdd_table";
    string v0 = "";
    // cout << typeid(table).name() << endl;
    cout << exist_tables.size() << endl;
    for (int i=0; i<exist_tables.size(); i++){
        v0 += exist_tables[i];
        v0 += ",";
    }
    v0.pop_back();
    Insert_Attrvalue(k0,v0);

    // save table
    string k = "/gdd_table/"+table.name;
    string v = "";
    // cout << typeid(table).name() << endl;
    cout << table.cols.size() << endl;
    for (int i=0; i<table.cols.size(); i++){
        v += table.cols[i].name;
        v += ",";
    }
    v.pop_back();
    Insert_Attrvalue(k,v);

    for (int i=0; i<table.cols.size(); i++){
        ColumnDef col = table.cols[i];
        string k1 = k + "/" + col.name;
        string v1 = col.type;
        Insert_Attrvalue(k1,v1);

        string k2 = k + "/desc/" + col.name;
        string v2 = col.desc;
        Insert_Attrvalue(k2,v2);

        if(col.key == true){
            string k3 = "/gdd_key/"+table.name;
            string v3 = col.name;
            Insert_Attrvalue(k3,v3);
        }
    }
    return true;
}

bool saveFragToEtcd(Fragment frag){
    // save frag info
    string k = "/part_schema/"+frag.name;
    string v = frag.fragtype;
    Insert_Attrvalue(k,v);

    k = "/part_info/"+frag.name;
    v = to_string(frag.fragnum);
    Insert_Attrvalue(k,v);

    for (int i=0; i < frag.fragnum; i++){
        FragDef fr = frag.frags[i];
        string k1 = k+"/"+frag.name+"."+to_string(fr.id);
        string v1 = fr.condition;
        Insert_Attrvalue(k1,v1);

        k1 = "/part_site/"+frag.name+"."+to_string(fr.id);
        v1 = to_string(fr.site);
        Insert_Attrvalue(k1,v1);

        k1 = "/part_size/"+frag.name+"."+to_string(fr.id);
        v1 = to_string(fr.size);
        Insert_Attrvalue(k1,v1);

        k1 = "/part_column/"+frag.name+"."+to_string(fr.id);
        v1 = fr.column;
        Insert_Attrvalue(k1,v1);
    }
    return true;
}
string get_value_from_json(string &res){
    Value root; 
    Value node;
    Reader reader;  
    FastWriter writer; 
    string json = res;
     if(!reader.parse(json, root)){  
        cout << "parse json error" << endl;
        return "";   
    }
    // cout << cout << typeid(root).name() << "       " << root["kvs"][0]["value"] << endl;

    string nodeString = writer.write(root["kvs"]); 
    nodeString.pop_back();
    string nodeString1 = nodeString.substr(1);
    if(!reader.parse(nodeString1, node)){
        cout << "parse json error" << endl;
        return "";
    }
    // cout << node["value"] << endl;

    string v = writer.write(node["value"]);
    v.pop_back();
    v.pop_back();
    string v1 = v.substr(1);
    // cout << v1<< endl;
    ZBase64 base64;
    return base64.DecodeBase64(v1);

}

GDD getTableFromEtcd(string tablename){
    GDD tbl;
    tbl.name = tablename;
    string key = getTableKey(tablename);
    vector<string> attri = getTableAttri(tablename);
    for(int i=0; i<attri.size();i++){
        string att = attri[i];
        ColumnDef col;
        col.name = att;
        col.type = getTableAttriType(tablename,att);
        col.desc = getTableAttriDesc(tablename,att);
        if(att == "key"){
            col.key=true;
        }
        tbl.cols.push_back(col);
    }
    

    return tbl;
}
Fragment getFragFromEtcd(string tablename){
    Fragment fmt;
    fmt.name = tablename;
    fmt.fragtype = getTableFragType(tablename);
    fmt.fragnum = getTableFragNum(tablename);
    cout <<"get Ftagment: "<<tablename << "  " <<" fmt.fragtype : "<<fmt.fragtype<<" fmt.fragnum : "<<fmt.fragnum;
    for(int i=1;i<=fmt.fragnum;i++){
        FragDef df;
        df.column = getTableFragCol(tablename,i);
        df.condition = getTableFragCondition(tablename,i);
        df.id = i;
        df.site = getTableFragSite(tablename,i);
        df.size = getTableFragSize(tablename,i);
        fmt.frags.push_back(df);
        cout <<" get frag : "<<i<<" "<<" df.condition : "<< df.condition <<" df.column : "<<df.column<<" df.site : "<<df.site<< endl;
    }
    return fmt;
}

// int main(){
//     ZBase64 base64;
    
//     string t2 = "publisher";
//     // vector<string> r = getTableAttri(t2);
//     // cout << r[0] << endl;

//     // string t2 = "publisher";
//     // GDD g = getTableFromEtcd(t2);
//     // cout << g.name <<endl;
//     // cout << g.cols[0].name <<"  "<< g.cols[0].type <<"  "<< g.cols[0].desc <<"  "<< g.cols[0].key <<"  "<< endl;
    
//     // Fragment f = getFragFromEtcd(t2);
//     // cout << f.name <<"  "<< f.fragnum <<"  "<< f.fragtype << endl;
//     // cout << f.frags[0].column <<"  "<< f.frags[0].condition <<"  "<< f.frags[0].id <<"  "<< f.frags[0].site <<"  "<< f.frags[0].size;
    
//     // return 0;
//     // cout << base64.DecodeBase64("aWQ=")<< endl;
//     int t1 = 2;
//     if (t1==1){
//         cout << "save table *********************************" << endl;
//         GDD tbl;
//         tbl.name = "publisher";
//         ColumnDef clm;
//         clm.name = "id";
//         clm.key = true;
//         clm.null = false;
//         clm.type = "int";
//         clm.desc = "desc of id";
//         tbl.cols.push_back(clm);
//         saveTableToEtcd(tbl);
//     }
//     if (t1==2){
//         cout << "save frag *********************************" << endl;
//         Fragment fmt;
//         fmt.name = "publisher";
//         fmt.fragtype = "V";
//         fmt.fragnum = 1;
        
//         FragDef fd;
//         fd.id = 1;
//         fd.site = 1;
//         fd.size = 0;
//         fd.column = "id,nation";
//         fd.condition = "id < 104000 AND nation=’PRC’";
//         fmt.frags.push_back(fd);
//         saveFragToEtcd(fmt);
//     }
//     cout <<"hh"<< getFragFromEtcd(t2).fragtype << endl;

//     // string t = "{\"key\":\""+base64.EncodeBase64("/part_column/publisher.1")+"\"}";
//     // string op = "GET";
//     // string res = etcd_op(t,op);  //res is json string
//     // string v = get_value_from_json(res);
//     // cout <<"show example:   key: /part_column/publisher.1   value: "<< v << endl;
//     return 0;
    
// }
//操作etcd 插入数据，或查询数据
string etcd_op(string &dt,string &op) 
{
    string etcd_url = "http://127.0.0.1:2379/v3/kv/";
    if(op == "PUT"){
        etcd_url += "put";
    }
    else if(op == "GET"){
        etcd_url+= "range";
    }
    else{
        cout << "illegal opration !" << endl;
        return "";
    }
    char *ss=(char*)etcd_url.c_str();
    CURLcode return_code;  
    // cout << "data   " << dt << endl;
    return_code = curl_global_init(CURL_GLOBAL_SSL);  
    if (CURLE_OK != return_code)  
    {  
        cerr << "init libcurl failed." << endl;  
        return "";  
    }  
    // 获取easy handle  
    CURL *easy_handle = curl_easy_init();  
    if (NULL == easy_handle)  
    {  
        cerr << "get a easy handle failed." << endl;  
        curl_global_cleanup();   
        return "";  
    }  
    char szJsonData[1024]; 
    strcpy(szJsonData, dt.c_str()); 
    char * buff_p = NULL;  
    char result[5000] = "";
    // 设置easy handle属性  
    // struct curl_slist *headers = NULL;
    // headers = curl_slist_append(headers, "Accept: */*");
    // curl_easy_setopt(easy_handle, CURLOPT_HTTPHEADER, headers);//设置头
    
    // transfer type == json
    curl_slist *plist = curl_slist_append(NULL,   
                "Content-Type:application/json;charset=UTF-8");  
    curl_easy_setopt(easy_handle, CURLOPT_HTTPHEADER, plist);
    curl_easy_setopt(easy_handle, CURLOPT_URL,ss);  
    curl_easy_setopt(easy_handle, CURLOPT_PORT, 2379); 
    curl_easy_setopt(easy_handle, CURLOPT_POSTFIELDS, szJsonData);
    // curl_easy_setopt(easy_handle, CURLOPT_CUSTOMREQUEST, "POST");
    // curl_easy_setopt(easy_handle, CURLOPT_VERBOSE, 1L);

    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, &write_data);  
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, &result);  
  
    // 执行数据请求  
    curl_easy_perform(easy_handle);   
  
    // 释放资源  
    curl_easy_cleanup(easy_handle);  
    curl_global_cleanup();  
    string info = result;
    // cout << info << endl;
    return info;
}

bool Insert_Attrvalue(string &key,string &value)
{  //调用存储数据的etcd接口 
    ZBase64 base64; 
    string data = "{\"key\": \""+base64.EncodeBase64(key)+"\", \"value\": \""+base64.EncodeBase64(value)+"\"}";
    string op = "PUT";
    etcd_op(data,op);
    cout << "Saved!" << endl;
    return true; 
}
size_t write_data(void *buffer, size_t size, size_t nmemb, void *stream) 
//curl 返回的数据流
{ 
    strncat((char*)stream,(char*)buffer,size*nmemb);
    return nmemb*size; 
} 
