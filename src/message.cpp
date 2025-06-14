#include "message.hpp"
#include "utility.hpp"
#include <sstream>

Request::Request() {}

Request::Request(const char* req) {
    std::stringstream request(req);
    std::string line;
    std::getline(request, line);
    std::stringstream requestLine(line);
    requestLine >> _method >> _url >> _version;
    parseURL();

    while (std::getline(request, line) && line != "\r") {
        size_t pos = line.find(": ");
        if (pos != std::string::npos) {
            _headers[line.substr(0, pos)] = line.substr(pos + 2);
            _headers[line.substr(0, pos)].pop_back(); // Remove trailing "\r"
        }
    }

    std::getline(request, _body);
}

std::string Request::method() { return _method; }
std::string Request::body() { return _body; }
std::string Request::url() { return _url; }
std::string Request::version() { return _version; }
std::string Request::route() { return _route; }

std::string Request::operator[](std::string header) {
    return (_headers.count(header) == 0) ? "NULL" : _headers[header];
}

std::string Request::query(std::string qry) {
    return (_query.count(qry) == 0) ? "NULL" : _query[qry];
}

std::string Request::toString() {
    std::string req = _method + " " + _url + " " + _version + "\r\n";
    for (const auto& header : _headers) {
        req += header.first + ": " + header.second + "\r\n";
    }
    req += "\r\n" + _body;
    return req;
}

void Request::parseURL() {
    size_t pos = _url.find("?");
    if (pos != std::string::npos) {
        _route = _url.substr(0, pos);
        std::string queryString = _url.substr(pos + 1);
        std::stringstream queryStream(queryString);
        std::string pair;
        while (std::getline(queryStream, pair, '&')) {
            size_t equalPos = pair.find('=');
            if (equalPos != std::string::npos) {
                std::string key = pair.substr(0, equalPos);
                std::string value = pair.substr(equalPos + 1);
                std::string decodedKey = "";
                std::string decodedValue = "";

                for (size_t i = 0; i < key.length(); ++i) {
                    if (key[i] == '%') {
                        int hexValue = 0;
                        hexValue += (key[i + 1] >= '0' && key[i + 1] <= '9') ? (key[i + 1] - '0') * 16 : (key[i + 1] - 'A' + 10) * 16;
                        hexValue += (key[i + 2] >= '0' && key[i + 2] <= '9') ? (key[i + 2] - '0') : (key[i + 2] - 'A' + 10);
                        decodedKey += static_cast<char>(hexValue);
                        i += 2;
                    } else if (key[i] == '+') {
                        decodedKey += ' ';
                    } else {
                        decodedKey += key[i];
                    }
                }

                for (size_t i = 0; i < value.length(); ++i) {
                    if (value[i] == '%') {
                        int hexValue = 0;
                        hexValue += (value[i + 1] >= '0' && value[i + 1] <= '9') ? (value[i + 1] - '0') * 16 : (value[i + 1] - 'A' + 10) * 16;
                        hexValue += (value[i + 2] >= '0' && value[i + 2] <= '9') ? (value[i + 2] - '0') : (value[i + 2] - 'A' + 10);
                        decodedValue += static_cast<char>(hexValue);
                        i += 2;
                    } else if (value[i] == '+') {
                        decodedValue += ' ';
                    } else {
                        decodedValue += value[i];
                    }
                }

                if (!decodedKey.empty() && !decodedValue.empty()) {
                    _query[decodedKey] = decodedValue;
                }
            }
        }
    } else {
        _route = _url;
        _query.clear();
    }
}

// Response definitions

Response::Response() : _version("HTTP/1.1"), _status(200), _body("") {}

Response::Response(std::string version, int status, std::string body) : _version(version), _status(status), _body(body) {}

std::string Response::version() const { return _version; }
int Response::status() const { return _status; }
std::string Response::header(std::string key) { return _headers[key]; }
std::string Response::body() const { return _body; }

void Response::setVersion(const std::string& version) { _version = version; }
void Response::setStatus(int status) { _status = status; }
void Response::setHeader(std::string key, std::string value) { _headers[key] = value; }
void Response::setBody(const std::string& body) { _body = body; }

std::string Response::toString() const {
    std::string response = _version + " " + std::to_string(_status) + " " + getDescription(_status) + "\r\n";
    for (const auto& header : _headers) {
        response += header.first + ": " + header.second + "\r\n";
    }
    response += "\r\n" + _body;
    return response;
}
