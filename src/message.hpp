#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <map>
#include <string>

// Forward declaration
std::string getDescription(int code);

class Request {
private:
    std::string _method;
    std::string _url;
    std::string _version;
    std::map<std::string, std::string> _headers;
    std::map<std::string, std::string> _query;
    std::string _route;
    std::string _body;

    void parseURL();

public:
    Request();
    Request(const char* req);

    std::string body();
    std::string url();
    std::string version();
    std::string route();
    std::string method();

    std::string operator[](std::string header);
    std::string query(std::string qry);
    std::string toString();
};

class Response {
private:
    std::string _version;
    int _status;
    std::map<std::string, std::string> _headers;
    std::string _body;

public:
    Response();
    Response(std::string version, int status, std::string body);

    // Getters
    std::string version() const;
    int status() const;
    std::string header(std::string key);
    std::string body() const;

    // Setters
    void setVersion(const std::string& version);
    void setStatus(int status);
    void setHeader(std::string key, std::string value);
    void setBody(const std::string& body);

    std::string toString() const;
};

#endif // MESSAGE_HPP
