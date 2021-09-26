#include <WinSock2.h>
#pragma comment(lib,"ws2_32")        //链接到ws2_32动态链接库

class CInitSock
{
public:
    CInitSock(BYTE minorVer = 2,BYTE majorVer = 2)
    {
        WSADATA wsaData;
        WORD VersionRequset;
        VersionRequset = MAKEWORD(minorVer,majorVer);
        //装载winsock库
        if (WSAStartup(VersionRequset,&wsaData)!=0)
        {
            //装载winsock库失败，推出
            exit(0);
        }
    }
    ~CInitSock()
    {
        WSACleanup();
    }
};

#include <iostream>
#include "../Common/CInitSock.h"
using namespace std;
CInitSock Initsock;
bool GetIp();
int main()
{
    GetIp();
    return 0;
}
bool GetIp()
{
    char szText[256];
    //获取本机主机名称
    int iRet;
    iRet = gethostname(szText,256);
    int a = WSAGetLastError();
    if (iRet!=0)
    {
        printf("gethostname()  Failed!");
        return FALSE;
    }
    //通过主机名获取到地址信息
    HOSTENT *host = gethostbyname(szText);
    if (NULL==host)
    {
        printf("gethostbyname() Failed!");
        return false;
    }
    in_addr PcAddr;
    for (int i=0;;i++)
    {
        char *p = host->h_addr_list[i];
        if (NULL==p)
        {
            break;
        }
        memcpy(&(PcAddr.S_un.S_addr),p,host->h_length);
        char*szIP = ::inet_ntoa(PcAddr);
        printf("本机的ip地址是：%s\n",szIP);
    }
}