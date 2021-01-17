#include <iostream>
#include <string>
#include <list>
#include <pthread.h>

#include "butil/logging.h"

#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <stdexcept>
#include <exception>

const std::string DEFAULT_HOST = "127.0.0.1";
const std::string DEFAULT_USERNAME = "root";
const std::string DEFAULT_PASSWORD = "123456";
const int MAXSIZE = 100;


class MysqlConnectionPool{
    private:
        int m_curPoolSize;
        int m_maxPoolSize;
        std::string m_username;
        std::string m_password;
        std::string m_url;

        std::list<sql::Connection*> m_connlist;
        pthread_mutex_t lock;
        sql::mysql::MySQL_Driver* m_driver;
        friend class std::auto_ptr<MysqlConnectionPool>;
        static std::auto_ptr<MysqlConnectionPool> auto_ptr_instance;

    private:
        MysqlConnectionPool(std::string url, std::string username, std::string password, int maxSize);
        ~MysqlConnectionPool(void);
        sql::Connection* CreateConnection(void);
        void InitConnection(int initialSize);
        void TerminateConnection(sql::Connection* conn);
        void TerminateConnectionPool(void);
        
    public:
        sql::Connection* GetConnection(void);
        void ReleaseConnection(sql::Connection* conn);
        static MysqlConnectionPool& GetInstance(void);
        int GetCurrentPoolSize(void);
};


#define MysqlPool MysqlConnectionPool::GetInstance()

std::auto_ptr<MysqlConnectionPool> MysqlConnectionPool::auto_ptr_instance;


MysqlConnectionPool::MysqlConnectionPool(std::string url, std::string username, std::string password, int maxSize){
    this->m_url = url;
    this->m_username = username;
    this->m_password = password;
    this->m_maxPoolSize = MAXSIZE;
    this->m_curPoolSize = 0;

    try
    {
        this->m_driver = sql::mysql::get_driver_instance();
    }
    catch(sql::SQLException& e){
        LOG(ERROR) << "cannot get mysql driver";
    }
    catch(const std::exception& e)
    {
        LOG(ERROR) << e.what();
    }
    this->InitConnection(m_maxPoolSize / 2);
}

MysqlConnectionPool::~MysqlConnectionPool(void){
    this->TerminateConnectionPool();
}

MysqlConnectionPool& MysqlConnectionPool::GetInstance(void){
    if (auto_ptr_instance.get() == 0)
    {
        auto_ptr_instance.reset(new MysqlConnectionPool(DEFAULT_HOST, DEFAULT_USERNAME, DEFAULT_PASSWORD, MAXSIZE));
    }
    return *(auto_ptr_instance.get());
    
}

void MysqlConnectionPool::InitConnection(int initialSize){
    sql::Connection* conn;
    pthread_mutex_lock(&this->lock);
    for (int i = 0; i < initialSize; i++)
    {
        conn = this->CreateConnection();
        if (conn)
        {
            m_connlist.push_back(conn);
            ++(this->m_curPoolSize);
        }
        else{
           LOG(ERROR) << "failed to create mysql connection";
        }
    }
    pthread_mutex_unlock(&this->lock);
}

sql::Connection* MysqlConnectionPool::CreateConnection(void){
    sql::Connection* conn;
    try
    {
        conn = this->m_driver->connect(this->m_url, this->m_username, this->m_password);
        return conn;
    }
    catch(sql::SQLException& e){
        LOG(ERROR) << "fail to create mysql connection";
        return NULL;
    }
    catch(const std::exception& e)
    {
        LOG(ERROR) << e.what();
        return NULL;
    }   
}

sql::Connection* MysqlConnectionPool::GetConnection(void){
    sql::Connection* conn;
    pthread_mutex_lock(&lock);
    if (this->m_connlist.size() > 0)
    {
        conn = this->m_connlist.front();
        m_connlist.pop_front();
        // if connection is closed, create new one after deleting old one
        if (conn->isClosed())
        {
            delete conn;
            conn = this->CreateConnection();
        }
        if (conn == NULL)
        {
            --this->m_curPoolSize;
        }
        pthread_mutex_unlock(&lock);
        return conn;
    }
    else
    {
        if (this->m_curPoolSize < this->m_maxPoolSize)
        {
            conn = this->CreateConnection();
            if (conn)
            {
                ++this->m_curPoolSize;
                pthread_mutex_unlock(&this->lock);
                return conn;
            }
            else
            {
                pthread_mutex_unlock(&this->lock);
                return NULL;
            }
        }
        else
        {
            pthread_mutex_unlock(&this->lock);
            return NULL;
        }
    }
}

void MysqlConnectionPool::ReleaseConnection(sql::Connection* conn){
    if (conn)
    {
        pthread_mutex_lock(&this->lock);
        m_connlist.push_back(conn);
        pthread_mutex_unlock(&this->lock);
    }
    
}


void MysqlConnectionPool::TerminateConnectionPool(void){
    pthread_mutex_lock(&this->lock);
    for(auto iter = this->m_connlist.begin(); iter != this->m_connlist.end(); iter++){
        this->TerminateConnection(*iter);
    }
    this->m_curPoolSize = 0;
    this->m_connlist.clear();
    pthread_mutex_unlock(&this->lock);
}

void MysqlConnectionPool::TerminateConnection(sql::Connection* conn){
    if (conn)
    {
        try
        {
            conn->close();
        }
        catch(sql::SQLException &e) {
			LOG(ERROR) << e.what();
		}
        catch(const std::exception& e)
        {
            LOG(ERROR) << e.what();
        }
        delete conn;
    }
    
}

int MysqlConnectionPool::GetCurrentPoolSize(){
    return this->m_curPoolSize;
}