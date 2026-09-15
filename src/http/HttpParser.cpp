#include "HttpParser.hpp"
#include <sstream>

namespace http {

    bool HttpParser::parseRequest(net::Buffer* buf) {
        bool hasMore = true;
        bool ok = true;

        while(hasMore){
            if(state_ == State::RequestLine){
                const char* crlf = buf->findCRLF();

                if(crlf){
                    ok = parseRequestLine(buf->peek(), crlf);
                    if(ok){
                        size_t len = crlf - buf->peek();
                        buf->retrieve(len + 2);
                        state_ = State::Headers;
                    }
                    else{
                        hasMore = false;
                    }
                }
                else{
                    hasMore = false;
                }

            }
            else if(state_ == State::Headers){
                const char* crlf = buf->findCRLF();
                
                if(crlf){
                    const char* colon = std::find(buf->peek(), crlf, ':');

                    int len = crlf - buf->peek();

                    if(colon && colon < crlf){
                        std::string key(buf->peek(), colon);
                        std::string value(colon + 1, crlf);
                        
                        int start = 0;
                        while(start < value.size() && value[start] == ' ') start++;

                        req_.addHeader(key, value.substr(start));

                        buf->retrieve(len + 2);
                    }
                    else{
                        buf->retrieve(len + 2);

                        std::string contentLength = req_.getHeader("Content-Length");
                        if(!contentLength.empty()){
                            state_ = State::Body;
                        }
                        else{
                            state_ = State::Complete;
                            hasMore = false;
                        }
                    }

                }
                else{
                    hasMore = false;
                }
            }
            else if(state_ == State::Body){
                size_t contentLength = std::stoul(req_.getHeader("Content-Length"));
                
                if(buf->readableBytes() >= contentLength){
                    req_.setBody(std::string(buf->peek(), contentLength));
                    buf->retrieve(contentLength);
                    state_ = State::Complete;
                    hasMore = false;
                }
                else{
                    hasMore = false;
                }
            }
        }

        return ok;
    }

    bool HttpParser::isCompeted() {
        return state_ == State::Complete;
    }

    void HttpParser::reset() {
        state_ = State::RequestLine;
        req_.reset();
    }

    const HttpRequest& HttpParser::getRequest() const {
        return req_;
    }

    bool HttpParser::parseRequestLine(const char* start, const char* end) {
        std::string line(start, end);

        std::stringstream ss(line);

        std::string method, url, version;

        ss >> method >> url >> version;

        if(method == "GET") req_.setMethod(Method::Get);
        else if(method == "POST") req_.setMethod(Method::Post);
        else if(method == "PUT") req_.setMethod(Method::Put);
        else if(method == "DELETE") req_.setMethod(Method::Delete);
        else return false;

        size_t pos = url.find("?");
        if(pos != std::string::npos){
            req_.setPath(url.substr(0, pos));
            req_.setQuery(url.substr(pos + 1));
        }
        else{
            req_.setPath(url);
        }

        if(version == "HTTP/1.1") req_.setVersion(Version::Http11);
        else if(version == "HTTP/1.0") req_.setVersion(Version::Http10);
        else return false;

        return true;
    }
}
