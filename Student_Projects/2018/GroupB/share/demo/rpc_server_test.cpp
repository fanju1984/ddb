#include "rpc_server.h"
#include <iostream>
#include <thread>
using namespace std;

int main(){
    thread th1(startServer);
    th1.detach();
    while(true){
        string s;
        cin >> s ;
        cout << s << endl;
    }
    // cout << "aaaa" << endl;
    // startServer();
    return 0;
}




