#include "ReadData.hpp"
#include "HttpResponse.hpp"
#include "DbManager.hpp"

#include <fstream>
#include <sstream>

namespace handler{
    
std::string readSongsMetadata(http::HttpRequest req) {
    http::HttpResponse response;

    std::string data = db::DbManager::getInstance().querySongs();

    response.setStatusCode(http::HttpStatusCode::Ok);
    response.setDescribe("Song MetaData");
    response.addHeader("Content-Type", "application/json; charset=utf-8");
    response.addHeader("Content-Length", std::to_string(data.size()));
    response.addHeader("Connection", "keep-alive");
    response.setBody(data);

    return response.buildResponse();
}

std::string readSongImage(http::HttpRequest req) {
    http::HttpResponse response;

    std::string coverFileName = urlDecode(req.getSuffix());
    if(!coverFileName.empty()){
        std::string data = readFileData(coverFileName, ResourceType::Image);

        if (data.empty()) {
            response.notFound();
            return response.buildResponse();
        }

        response.setStatusCode(http::HttpStatusCode::Ok);
        response.setDescribe("Song Image");
        response.addHeader("Content-Type", "mage/png");
        response.addHeader("Content-Length", std::to_string(data.size()));
        response.addHeader("Cache-Control", "max-age=86400, public");
        response.addHeader("Connection", "keep-alive");
        response.addHeader("Access-Control-Allow-Origin", "*");
        response.setBody(data);
        
        return response.buildResponse();
    }

    response.notFound();
    return response.buildResponse();
}

std::string readSongLyrics(http::HttpRequest req) {
    http::HttpResponse response;

    std::string lyricsFileName = urlDecode(req.getSuffix());

    if(!lyricsFileName.empty()){
        std::string data = readFileData(lyricsFileName, ResourceType ::Lyrics);

        if (data.empty()) {
            response.notFound();
            return response.buildResponse();
        }

        response.setStatusCode(http::HttpStatusCode::Ok);
        response.setDescribe("Song Lyrics");
        response.addHeader("Content-Type", "text/plain; charset=utf-8");
        response.addHeader("Content-Length", std::to_string(data.size()));
        response.addHeader("Cache-Control", "max-age=86400, public");
        response.addHeader("Connection", "keep-alive");
        response.addHeader("Access-Control-Allow-Origin", "*");
        response.setBody(data);

        return response.buildResponse();
    }

    response.notFound();
    return response.buildResponse();
}

std::string readSongAudio(http::HttpRequest req) {
    http::HttpResponse response;
    
    std::string audioFileName = urlDecode(req.getSuffix());

    if(audioFileName.empty()){
        response.notFound();
        return response.buildResponse();
    }

    std::string path = std::string(toBaseDir(ResourceType::Audio)) + '/' + audioFileName;

    if(path.empty()){
        response.notFound();
        return response.buildResponse();
    }

    std::ifstream ifs(path, std::ios::binary | std::ios::ate);

    if(!ifs.is_open()){
        std::cerr << "无法打开文件(" << path << ")进行读取！" << std::endl;
        response.notFound();
        return response.buildResponse();
    }        

    size_t fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    response.setDescribe("Song Audio");
    response.addHeader("Accept-Ranges", "bytes");
    response.addHeader("Content-Type", "audio/mpeg");

    std::string rangeHeader = req.getHeader("Range");

    if(rangeHeader.empty()){
        response.setStatusCode(http::HttpStatusCode::Ok);
        response.addHeader("Content-Length", std::to_string(fileSize));
        
        std::string data(fileSize, '\0');
        ifs.read(data.data(), fileSize);
        
        response.setBody(data);

        return response.buildResponse();
    }
    else{
        auto[start, end] = parseRangeHeader(rangeHeader, fileSize);

        if(start >= fileSize || end >= fileSize || start > end){
            response.setStatusCode(http::HttpStatusCode::RangeNotSatisfiable);
            response.addHeader("Content-Range", "bytes */" + std::to_string(fileSize));
            return response.buildResponse();
        }

        size_t contentLength = end - start + 1;

        ifs.seekg(start);
        std::string subData(contentLength, '\0');
        ifs.read(subData.data(), contentLength);

        size_t readSizes = ifs.gcount();
        if(readSizes != contentLength){
            subData.resize(readSizes);  
            contentLength = readSizes;
        }

        response.setStatusCode(http::HttpStatusCode::Partial);
        response.addHeader("Content-Range", "bytes " + std::to_string(start) + "-" + 
                                    std::to_string(end) + "/" + 
                                    std::to_string(fileSize));
        response.addHeader("Content-Length", std::to_string(contentLength));
        response.setBody(subData);
    }

    ifs.close();

    return response.buildResponse();
}

std::string readFileData(const std::string& fileName, ResourceType type) {
    if(!exist(fileName, type)) return "";

    std::string path = std::string(toBaseDir(type)) + '/' + fileName;

    std::ifstream ifs(path, std::ios::binary | std::ios::ate);

    if(!ifs.is_open()){
        std::cerr << "无法打开文件(" << path << ")进行读取！" << std::endl;
        return "";
    }        

    size_t fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::string data(fileSize, '\0');
    ifs.read(data.data(), fileSize);
    
    ifs.close();

    return data;
}

bool exist(const std::string& fileName, ResourceType type) {
    bool isExist = false;
    if(type == ResourceType ::Image){
        isExist = db::DbManager::getInstance().coverPathExist(fileName);
    }
    else if(type == ResourceType::Lyrics){
        isExist = db::DbManager::getInstance().lyricsPathExist(fileName);
    }
    else if(type == ResourceType::Audio){
        isExist = db::DbManager::getInstance().audioPathExist(fileName);
    }

    return isExist;
}

std::pair<int, int> parseRangeHeader(const std::string& rangeHeader, int totalSize) {
    size_t start = 0;
    size_t end = totalSize - 1;

    if(rangeHeader.find("bytes=") == 0){
        std::string rangeStr = rangeHeader.substr(6);
        size_t pos = rangeStr.find("-");
        if(pos != std::string::npos){
            std::string startStr = rangeStr.substr(0, pos);
            if(!startStr.empty()){
                start = std::stoll(startStr);
            }

            std::string endStr = rangeStr.substr(pos + 1);
            if(!endStr.empty()){
                end = std::stoll(endStr);
            }
        }
    }

    return std::make_pair(start, end);
}

std::string urlDecode(const std::string str) {
    std::string result;
    result.reserve(str.length());

    for (std::size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '%' && i + 2 < str.length()) {
            char h1 = str[i + 1];
            char h2 = str[i + 2];
            
            if (std::isxdigit(h1) && std::isxdigit(h2)) {
                int v1 = std::isdigit(h1) ? h1 - '0' : std::toupper(h1) - 'A' + 10;
                int v2 = std::isdigit(h2) ? h2 - '0' : std::toupper(h2) - 'A' + 10;
                result += static_cast<char>((v1 << 4) + v2);
                i += 2;
                continue;
            }
        }
        result += str[i];
    }
    return result;
}

} // namespace handle
