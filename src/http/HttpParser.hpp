#ifndef HTTP_PARSER_HPP
#define HTTP_PARSER_HPP

#include "Buffer.hpp"
#include "HttpRequest.hpp"

namespace http{


class HttpParser{
public:
    enum class State{
        RequestLine,
        Headers,
        Body,
        Complete,
        Error
    };

    bool parseRequest(net::Buffer *buf);

    bool isCompeted();

    void reset();

    const HttpRequest& getRequest() const;

private:
    bool parseRequestLine(const char* start, const char* end);

    State state_ = State::RequestLine;
    HttpRequest req_; 
};

} // namemspace http

#endif // HTTP_PARSER_HPP