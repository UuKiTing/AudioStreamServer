#include "HttpRequest.hpp"
#include <sstream>

namespace http{
    HttpRequest::HttpRequest(): method_(Method::Invaild), version_(Version::UnKnown) {
    }

    const Method HttpRequest::getMethod() const{
        return method_;
    }

    void HttpRequest::setMethod(Method method) {
        method_ = method;
    }

    const std::string HttpRequest::getPath() const{
        return path_;
    }

    void HttpRequest::setPath(std::string path) {
        path_ = path;
    }

    Version HttpRequest::getVersion() {
        return version_;
    }

    void HttpRequest::setVersion(Version v) {
        version_ = v;
    }

    const std::string HttpRequest::getQuery() const{
        return query_;
    }

    void HttpRequest::setQuery(std::string query) {
            query_ = query;
    }

    const std::string HttpRequest::getBody() const{
        return body_;
    }

    void HttpRequest::setBody(std::string body) {
        body_ = body;
    }

    const std::string HttpRequest::getSuffix() const {
        return suffix_;
    }

    void HttpRequest::setSuffix(std::string suffix) {
        suffix_ = suffix;
    }

    const std::string HttpRequest::getHeader(const std::string& key) const{
        auto it = headers_.find(key);
        if(it != headers_.end()){
            return it->second;
        }
        else{
            return "";
        }
    }

    void HttpRequest::addHeader(const std::string& key,
                                const std::string& value) {

        headers_[key] = value;
    }

    void HttpRequest::reset() {
        method_ = Method::Invaild;
        version_ = Version::UnKnown;
        path_.clear();
        headers_.clear();
        query_.clear();
        body_.clear();
        suffix_.clear();
    }


} // namespace http