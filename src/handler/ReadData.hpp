#ifndef READ_DATA_HPP
#define READ_DATA_HPP

#include "HttpRequest.hpp"
#include <string>

namespace handler{

enum class ResourceType {Audio, Image, Lyrics};

constexpr const char* toBaseDir(ResourceType type) {
    switch (type) {
        case ResourceType::Audio: return "resource/songAudio";
        case ResourceType::Image: return "resource/songImage";
        case ResourceType::Lyrics: return "resource/songLyrics";
    }
    return "";
}

std::string readSongsMetadata(http::HttpRequest req);

std::string readSongImage(http::HttpRequest req);

std::string readSongLyrics(http::HttpRequest req);

std::string readSongAudio(http::HttpRequest req);

std::string readFileData(const std::string &fileName, ResourceType  type);

bool exist(const std::string &fileName, ResourceType type);

std::pair<int, int> parseRangeHeader(const std::string &rangeHeader, int totalSize);

std::string urlDecode(const std::string str);

} // namespace handle

#endif // READ_DATA_HPP