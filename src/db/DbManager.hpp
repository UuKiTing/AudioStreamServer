#ifndef DB_MANAGER_HPP
#define DB_MANAGER_HPP

#include "MysqlDB.hpp"

namespace db{

class DbManager{
public:
    DbManager(const DbManager&&) = delete;
    DbManager& operator=(const DbManager&&) = delete;

    static DbManager& getInstance();

    bool init(const std::string& ip, const std::string user, const std::string pwd, const std::string db, unsigned port = 3306);

    std::string querySongs();

    bool coverPathExist(const std::string& fileName);

    bool lyricsPathExist(const std::string& fileName);

    bool audioPathExist(const std::string& fileName);

private:
    DbManager();
    ~DbManager();

    MysqlDB mysqldb{};

    bool closed_ = false;
};

} // namespace db


#endif // DB_MANAGER_HPP