#include "Router.hpp"
#include "DbManager.hpp"
#include "HttpResponse.hpp"
#include <fstream>

namespace http
{
    Router::Router() {}

    void Router::addRoute(const std::string& prefix, bool isHasSuffix, Method method, Handler handler) {
        methods_[prefix] = method;
        routes_[prefix] = handler;
        isHasSuffixs_[prefix] = isHasSuffix;
    } 

    std::string Router::dispatch(HttpRequest req) {
        HttpResponse response;

        if(allowedMethods_.find(req.getMethod()) == allowedMethods_.end()){
            response.methodNotAllowed();
            return response.buildResponse();
        }

        std::string path = req.getPath();

        response.notFound();

        for (const auto &it : routes_){
            if(!isHasSuffixs_[it.first]){ // 如果没有后缀
                if(it.first == path)
                    return it.second(req);
                else 
                    return response.buildResponse();
            }

            if(path.find(it.first, 0) == 0){ // 如果有后缀
                std::string suffix = matchPath(path, it.first); // 提取后缀字符串

                if(!suffix.empty()){
                    req.setSuffix(suffix);
                    return it.second(req);
                }
                else{
                    return response.buildResponse();
                }
            }
        }

        return response.buildResponse();
    }


    std::string Router::matchPath(const std::string& path, const std::string& prefix) {
        std::string suffix = "";

        if((path.size() > prefix.size()) && (path.compare(0, prefix.size(), prefix) == 0) && (path[prefix.size()] == '/')){
            size_t start = prefix.size() + 1;
            size_t end = path.find('/', start);

            if(end != std::string::npos){
                suffix = path.substr(start);
            }        
            else{
                suffix = path.substr(start, end - start);
            }
        }

        return suffix;
    }
} // namespace http
