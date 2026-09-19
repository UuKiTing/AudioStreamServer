#include "HttpResponse.hpp"
#include <iostream>

namespace http{

    void HttpResponse::setStatusCode(HttpStatusCode code) {
        statusCode_ = code;
    }

    void HttpResponse::addHeader(const std::string& key, const std::string& value) {
        headers_[key] = value;
    }

    void HttpResponse::setDescribe(const std::string& describe) {
        describe_ = describe;
    }

    void HttpResponse::setBody(const std::string& body) {
        body_ = body;
    }

    std::string HttpResponse::buildResponse() {
        std::string rep;

        char buf[32];
        snprintf(buf, sizeof(buf), "HTTP/1.1 %d %s\r\n", statusCode_, describe_.data());

        rep.append(buf);

        for (const auto& header : headers_) {
            rep.append(header.first + ": " + header.second + "\r\n");
        }

        rep.append("\r\n");
        rep.append(body_);

        return rep;
    }

    void HttpResponse::reset() {
        statusCode_ = HttpStatusCode::Ok;
        describe_.clear();
        headers_.clear();
        body_.clear();
    }

    void HttpResponse::methodNotAllowed() {
        this->reset();
        this->setStatusCode(HttpStatusCode::MethodNotAllowed);
        this->setDescribe("Method Not Allowed");
        this->addHeader("Allow", "GET");
        this->addHeader("Content-Type", "application/json; charset=utf-8");
    }

    void HttpResponse::notFound() {
        this->reset();
        this->setStatusCode(HttpStatusCode::NotFound);
        this->setDescribe("Not Fount");
        this->addHeader("Content-Type", "application/json; charset=utf-8");
    }
} // namespace http