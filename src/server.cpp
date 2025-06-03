#include "router.cpp"
#include "message.hpp"
#include "utility.hpp"
#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<cstring>

class Server {
    private:
        int _socketFD;
        sockaddr_in _addr;
        std::vector<std::pair<std::string, RouteNode>> _routes;

    public:
        Server(int port) {
            _socketFD = socket(AF_INET, SOCK_STREAM, 0);
            if (_socketFD == -1) {
                throw std::runtime_error("Failed to create socket");
            }

            _addr.sin_family = AF_INET;
            _addr.sin_port = htons(port);
            _addr.sin_addr.s_addr = INADDR_ANY;

            if (bind(_socketFD, (sockaddr *)&_addr, sizeof(_addr)) == -1) {
                throw std::runtime_error("Binding of Socket to Address failed");
            }

            if (listen(_socketFD, 2) == -1) {
                throw std::runtime_error("Socket couldn't be setup to listen");
            }
        }
        void addRoute(std::string method, std::string path, std::function<Response(Request,std::map<std::string,std::string>)> callback) {
            for (auto& route : _routes) {
                if (route.first == method && route.second.path() == path) {
                    route.second.addCallback(method, callback);
                    return;
                }
            }
            RouteNode newRoute(path);
            newRoute.addCallback(method, callback);
            _routes.push_back(std::make_pair(method, newRoute));
        }
        void start() {
            while (true) {
                int client = accept(_socketFD, nullptr, nullptr);
                if (client == -1) {
                    std::cerr << "Failed to accept connection" << std::endl;
                    continue;
                }

                char in_buffer[1024] = {0};
                recv(client, in_buffer, 1024, 0);
                std::cout << "Received the message: " << in_buffer << "\n";

                Request req(in_buffer);
                Response res;

                for (auto& route : _routes) {
                    if (route.second.path() == req.route()) {
                        res = route.second.call(req);
                        break;
                    }
                }

                send(client, res.toString().c_str(), res.toString().length(), 0);
                close(client);
            }
        }
        ~Server() {
            close(_socketFD);
        }
};
// int main() {
//     Server server(8080);

//     server.addRoute("GET", "/hello", [](Request req, std::map<std::string,std::string> params) {
//         return Response("HTTP/1.1", 200, "Hello World");
//     });

//     server.start();

//     return 0;
// } //debugging purpose