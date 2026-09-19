#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include "Buffer.hpp"
#include <string>
#include <unordered_map>

namespace http{

enum HttpStatusCode {
    Ok = 200,
    Partial = 206,
    BadRequest = 400,
    NotFound = 404,
    MethodNotAllowed = 406,	
};


class HttpResponse {
public:
    void setStatusCode(HttpStatusCode code);

    void addHeader(const std::string& key, const std::string& value);

    void setDescribe(const std::string& describe);

    void setBody(const std::string& body);

    std::string buildResponse();

    void reset();

    void methodNotAllowed();

    void notFound();

private:
    std::unordered_map<std::string, std::string> headers_;
    HttpStatusCode statusCode_;
    std::string body_;
    std::string describe_;
};

} // namespace http



#endif //HTTP_RESPONSE_HPP