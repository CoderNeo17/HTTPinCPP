#include "utility.cpp"
#include "message.cpp"
#include <functional>
#include <vector>
#include <map>
#include <sstream>
#include <string>

class RouteNode{
    private:
        std::string _path;
        std::map<std::string,std::function<Response(Request,std::map<std::string,std::string>)>> _callback; // Ensure Response and Request are defined in included headers
        
    public:
        RouteNode(std::string path) : _path(path) {}
        void addCallback(std::string method, std::function<Response(Request,std::map<std::string,std::string>)> callback) {
            _callback[method] = callback;
        }
        Response call(Request req) {
            if (_callback.find(req.method()) != _callback.end()) {
                std::map<std::string,std::string> params;
                std::stringstream pathStream(_path);
                std::stringstream routeStream(req.route());
                std::string pathSegment, routeSegment;
                while(getline(pathStream, pathSegment, '/') && getline(routeStream, routeSegment, '/')) {
                    if (routeSegment == "*") {
                        continue;
                    }else if(pathSegment == routeSegment) {
                        continue;
                    }else if (routeSegment[0] == ':') {
                        params[pathSegment] = routeSegment;
                    } else {
                        break;
                    }
                }
                return _callback[req.method()](req,params);
            } else {
                return Response("HTTP/1.1", 405, "Method Not Allowed");
            }
        }
        std::string path() {
            return _path;
        }

        bool supportsMethod(const std::string& method) {
            return _callback.find(method) != _callback.end();
        }
};

class Router{
    private:
        std::vector<std::pair<std::string,RouteNode>> _routes;
        bool path_match(std::string path, std::string route) {
            std::stringstream pathStream(path);
            std::stringstream routeStream(route);
            std::string pathSegment, routeSegment;
            while(getline(pathStream, pathSegment, '/') && getline(routeStream, routeSegment, '/')) {
                if (routeSegment == "*") {
                    return true;
                }else if(pathSegment == routeSegment) {
                    continue;
                }else if (routeSegment[0] == ':') {
                    continue;
                } else {
                    return false;
                }
            }
            return false;
        }
    public:
        void addRoute(std::string path, std::string method, std::function<Response(Request,std::map<std::string,std::string>)> callback) {
            for (auto& route : _routes) {
                if (route.first == path) {
                    route.second.addCallback(method, callback);
                    return;
                }
            }
            RouteNode newNode(path);
            newNode.addCallback(method, callback);
            _routes.push_back(std::make_pair(path, newNode));
        }

        Response route(Request req) {
            for (auto& route : _routes) {
                if(path_match(route.first, req.route())) {
                    return route.second.call(req);
                }
            }
            return Response("HTTP/1.1", 404, "Not Found");
        }
        
};