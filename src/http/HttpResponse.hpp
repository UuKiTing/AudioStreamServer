#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include "Buffer.hpp"
#include <string>
#include <unordered_map>

namespace http{

enum HttpStatusCode {
    Ok = 200,
    NotFound = 404,
    BadRequest = 400,
    Partial = 206
};

class HttpResponse {
public:
    void setStatusCode(HttpStatusCode code);

    void addHeader(const std::string& key, const std::string& value);

    void setBody(const std::string& body);

    std::string buildResponse();


private:
    std::unordered_map<std::string, std::string> headers_;
    HttpStatusCode statusCode_;
    std::string body_;
};

} // namespace http



#endif //HTTP_RESPONSE_HPP