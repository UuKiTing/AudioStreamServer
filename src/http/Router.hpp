#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"


namespace http{

enum class ResourceType {Audio, Image, Lyrics};

constexpr const char* toBaseDir(ResourceType type) {
    switch (type) {
        case ResourceType::Audio: return "resource/songAudio";
        case ResourceType::Image: return "resource/songImage";
        case ResourceType::Lyrics: return "resource/songLyrics";
    }
    return "";
}


class Router{
public:
    Router(const HttpRequest &request);

    std::string handle();

private:
    std::string handleGet();

    std::string matchPath(const std::string& path, const std::string& prefix);

    std::string readFileData(const std::string& fileName, ResourceType  filePath);

    std::pair<int, int> parseRangeHeader(const std::string &rangeHeader, int totalSize);

    Method method_ = Method::Get;
    const std::string path_;

    HttpRequest request_;
};

} // namespace http



#endif // ROUTER_HPP