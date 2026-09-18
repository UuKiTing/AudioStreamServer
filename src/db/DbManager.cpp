#include "DbManager.hpp"
#include <iostream>
#include <nlohmann/json.hpp> 
#include <string>

namespace db{

using json = nlohmann::json;

DbManager& DbManager::getInstance() {
    static DbManager instance;
    return instance;
}

bool DbManager::init(const std::string& ip, const std::string user, const std::string pwd, const std::string db, unsigned port) {
    if(!mysqldb.init(ip, user, pwd, db, port)){
        closed_ = true;
        return false;
    }     

    return true;
}

std::string  DbManager::querySongs() {
    if(closed_) return "[]";

    MYSQL_RES *res = mysqldb.query("SELECT * FROM songs");
    if(!res) return "[]";

    MYSQL_ROW row;

    json arr = json::array();

    while((row = mysql_fetch_row(res)) != nullptr){
        json song;

        song["id"] = row[0];
        song["title"]  = row[1];
        song["artist"]  = row[2];
        song["duration"] = row[3];
        song["filePath"]  = row[4];
        song["coverPath"]  = row[5];
        song["lyricsPath"]  = row[6];

        arr.push_back(std::move(song));
    }   

    mysql_free_result(res);

    return arr.dump();
}


bool DbManager::coverPathExist(const std::string& fileName) {
    if(closed_) return false;

    MYSQL_RES *res = mysqldb.query("SELECT coverPath FROM songs WHERE coverPath = '" + fileName + "'");
    if(!res) return false;

    MYSQL_ROW row = mysql_fetch_row(res);
    if(!row) return false;
    
    mysql_free_result(res);

    return true;
}

bool DbManager::lyricsPathExist(const std::string& fileName) {
    if(closed_) return false;

    MYSQL_RES *res = mysqldb.query("SELECT lyricsPath FROM songs WHERE lyricsPath = '" + fileName + "'");
    if(!res) return false;

    MYSQL_ROW row = mysql_fetch_row(res);
    if(!row) return false;
    
    mysql_free_result(res);

    return true;
}

bool DbManager::audioPathExist(const std::string& fileName) {
    if(closed_) return false;

    MYSQL_RES *res = mysqldb.query("SELECT filePath FROM songs WHERE filePath = '" + fileName + "'");
    if(!res) return false;

    MYSQL_ROW row = mysql_fetch_row(res);
    if(!row) return false;
    
    mysql_free_result(res);

    return true;
}

DbManager::DbManager() {
}

DbManager::~DbManager() {
}


} // namespace db
