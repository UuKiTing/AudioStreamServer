#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "HttpRequest.hpp"
#include <functional>
#include <set>


namespace http{

class Router{
public:
    using Handler = std::function<std::string(HttpRequest&)>;

    Router();

    void addRoute(const std::string& prefix, bool isHasSuffix, Method method, Handler handler);

    std::string dispatch(HttpRequest req);

private:
    std::string matchPath(const std::string& path, const std::string& prefix);

    std::unordered_map<std::string, Handler> routes_;
    std::unordered_map<std::string, Method> methods_;
    std::unordered_map<std::string, bool> isHasSuffixs_;

    std::set<Method> allowedMethods_{Method::Get};
};

} // namespace http



#endif // ROUTER_HPP