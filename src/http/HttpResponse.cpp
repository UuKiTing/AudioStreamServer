#include "HttpResponse.hpp"
#include <iostream>

namespace http{

    void HttpResponse::setStatusCode(HttpStatusCode code) {
        statusCode_ = code;
    }

    void HttpResponse::addHeader(const std::string& key, const std::string& value) {
        headers_[key] = value;
    }

    void HttpResponse::setBody(const std::string& body) {
        body_ = body;
    }

    std::string HttpResponse::buildResponse() {
        std::string rep;

        char buf[32];
        snprintf(buf, sizeof(buf), "HTTP/1.1 %d \r\n", statusCode_);

        rep.append(buf);

        for (const auto& header : headers_) {
            rep.append(header.first + ": " + header.second + "\r\n");
        }

        rep.append("\r\n");
        rep.append(body_);

        return rep;
    }
} // namespace http