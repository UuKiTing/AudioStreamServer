#include "MysqlDB.hpp"
#include <iostream>

namespace db{

MysqlDB::MysqlDB() {
    mysql_ = nullptr;
    connected_ = false;
}

MysqlDB::~MysqlDB() {
    close();
}


MysqlDB& MysqlDB::getInstance() {
    static MysqlDB instance;
    return instance;
}


bool MysqlDB::init(const std::string& host, const std::string& user,
                       const std::string& passwd, const std::string& db,
                       unsigned int port) { 
    std::lock_guard<std::mutex> lock(mutex_);
    
    mysql_ = mysql_init(nullptr);

    if(mysql_ == nullptr){
        return false;
    }

    mysql_options(mysql_, MYSQL_SET_CHARSET_NAME, "utf8mb4");


    mysql_real_connect(mysql_, 
                       host.c_str(),  
                       user.c_str(),  
                       passwd.c_str(), 
                       db.c_str(), 
                       port, 
                       nullptr, 
                       0);

    if(mysql_ == nullptr){
        std::cerr << mysql_error(mysql_) << std::endl;

        mysql_close(mysql_);

        mysql_ = nullptr;

        return false;
    }


    connected_ = true;


    return true;
}

bool MysqlDB::execute(const std::string& sql) {
    std::lock_guard<std::mutex> lock(mutex_);

    if(!connected_) return false;


    if(mysql_query(mysql_, sql.c_str()) == 0){
        std::cerr << mysql_error(mysql_) << std::endl;

        return false;
    }   

    return true;
}

MYSQL_RES* MysqlDB::query(const std::string& sql) {
        std::lock_guard<std::mutex> lock(mutex_);

    if(!connected_) return nullptr;


    if(mysql_query(mysql_, sql.c_str()) == 0){
        std::cerr << mysql_error(mysql_) << std::endl;

        return nullptr;
    }   

    return mysql_store_result(mysql_);
}

std::string MysqlDB::error() {
    if(mysql_){
        return mysql_error(mysql_);
    }

    return "";
}

void MysqlDB::close() {
    std::lock_guard<std::mutex> lock(mutex_);

    if(mysql_){
        mysql_close(mysql_);

        mysql_ = nullptr;
    }

    connected_ = false;
}


}