#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <iostream>
#include <string>
#include <unordered_map>

namespace http{

enum Method{Invaild, Get, Post, Put, Delete};
enum Version{UnKnown, Http10, Http11};

class HttpRequest{
public:

    HttpRequest();

    const Method getMethod() const; // 获取请求方法
    void setMethod(Method method); // 设置请求方法

    const std::string getPath() const;
    void setPath(std::string path);

    Version getVersion();
    void setVersion(Version v);

    const std::string getQuery() const;
    void setQuery(std::string query);

    const std::string getBody() const;
    void setBody(std::string body);

    const std::string getHeader(const std::string& key) const;
    void addHeader(const std::string& key, const std::string& value);

    void reset();


private:
    Method method_; // 请求方式
    std::string path_; // 请求路径
    Version version_; // HTTP版本
    std::string query_; // URL参数

    std::unordered_map<std::string, std::string> headers_;

    std::string body_;
};


} // namespace http

#endif // HTTP_REQUEST_HPP