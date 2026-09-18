#ifndef MYSQLDB_HPP
#define MYSQLDB_HPP

#include <mysql/mysql.h>
#include <string>
#include <mutex>


namespace db{

class MysqlDB{
public:
    MysqlDB();
    ~MysqlDB();

    MysqlDB(const MysqlDB&&) = delete;
    MysqlDB& operator=(const MysqlDB&&) = delete;

    // 初始化
    bool init(const std::string& host,
              const std::string& user,
              const std::string& passwd,
              const std::string& db,
              unsigned int port = 3306);


    // 增删改查
    bool execute(const std::string& sql);

    MYSQL_RES* query(const std::string& sql);
    // 查询

    // 获取错误信息
    std::string error();

    // 关闭连接
    void close();

private:
    MYSQL *mysql_;

    bool connected_;

    std::mutex mutex_;
};

} // namespace Db


#endif // MYSQLDB_HPP