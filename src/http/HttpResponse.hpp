#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include "Buffer.hpp"
#include <string>
#include <unordered_map>

namespace http{

class HttpResponse {
public:
    enum HttpStatusCode {
        k200Ok = 200,
        k404NotFound = 404,
        k400BadRequest = 400
    };

    explicit HttpResponse(bool close) : statusCode_(k200Ok), closeConnection_(close) {}

    void setStatusCode(HttpStatusCode code);
    void setStatusMessage(const std::string& message);
    void setCloseConnection(bool on);
    bool closeConnection() const;

    void setContentType(const std::string& contentType);

    void addHeader(const std::string& key, const std::string& value) ;

    void setBody(const std::string& body);

    // 将 响应行 + 响应头 + 响应体 拼装写入 Buffer
    void appendToBuffer(net::Buffer* output) const;

private:
    std::unordered_map<std::string, std::string> headers_;
    HttpStatusCode statusCode_;
    std::string statusMessage_;
    bool closeConnection_;
    std::string body_;
};

} // namespace http



#endif //HTTP_RESPONSE_HPP