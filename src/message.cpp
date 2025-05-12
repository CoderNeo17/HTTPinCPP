#include<iostream>
#include<map>
#include<sstream>
#include "utility.cpp"

class Request{
    private:
        std::string _method;
        std::string _url;
        std::string _version;
        std::map<std::string,std::string> _headers;
        std::map<std::string, std::string> _query;
        std::string _route;
        std::string _body;

        void parseURL();

    public:
        Request(){}
        Request(const char* req);
        std::string body();
        std::string url();
        std::string version();
        std::string route();
        std::string operator [](std::string header);
        std::string query(std::string qry);
        std::string toString();
        std::string method();
};

class Response {
    private:
        // Variables
        std::string _version;
        int _status;
        std::map<std::string, std::string> _headers;
        std::string _body;
    public:
        // Getters
        std::string version() const { return _version; }
        int status() const { return _status; }
        std::string header(std::string key){ return _headers[key]; }
        std::string body() const { return _body; }
        // Setters
        void setVersion(const std::string& version) { _version = version; }
        void setStatus(int status) { _status = status; }
        void setHeader(std::string key, std::string value) { _headers[key] = value; }
        void setBody(const std::string& body) { _body = body; }
        // Constructors
        Response() : _version("HTTP/1.1"), _status(200), _body("") {}
        Response(std::string version, int status, std::string body);
        // Convert the response to a string
        std::string toString() const;
};

std::string Request::method(){
    return _method;
}
std::string Request::body(){
    return _body;
}
std::string Request::url(){
    return _url;
}
std::string Request::version(){
    return _version;
}
std::string Request::operator [](std::string header){
    return (_headers.count(header) == 0) ? "NULL" : _headers[header];
}
std::string Request::query(std::string qry){
    return (_query.count(qry) == 0) ? "NULL" : _query[qry];
}
std::string Request::toString(){
    std::string req = _method + " " + _url + " " + _version + "\r\n";
    for (const auto& header : _headers) {
        req += header.first + ": " + header.second + "\r\n";
    }
    req += "\r\n" + _body;
    return req;
}
std::string Request::route(){
    return _route;
}
void Request::parseURL(){
    size_t pos = this->_url.find("?");
    if (pos != std::string::npos) {
        _route = this->_url.substr(0, pos);
        std::string queryString = this->_url.substr(pos + 1);
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
                for (size_t i = 0; i < value.length(); i++) {
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
                if (decodedKey != "" && decodedValue != "") {
                    _query[decodedKey] = decodedValue;
                }
            }
        }
    } else {
        _route = this->_url;
        _query.clear();
    }
}
Request::Request(const char* req){
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
            _headers[line.substr(0, pos)].pop_back(); // Remove the trailing '\r'
        }
    }
    std::getline(request, _body);
}

Response::Response(std::string version, int status, std::string body){
    _version = version;
    _status = status;
    _body = body;
}
std::string Response::toString() const {
    std::string response = _version + " " + std::to_string(_status) + " " + getDescription(_status) + "\r\n";
    for (const auto& header : _headers) {
        response += header.first + ": " + header.second + "\r\n";
    }
    response += "\r\n" + _body;
    return response;
}

int main(){
    // Example usage
    const char* req = "GET https://www.google.com/search?q=hex+encode+of+space&rlz=1C1GCEA_enIN1067IN1068&oq=hex+encode+of+space&gs_lcrp=EgZjaHJvbWUyBggAEEUYOTIHCAEQIRigATIHCAIQIRigATIHCAMQIRifBTIHCAQQIRifBTIHCAUQIRifBTIHCAYQIRifBTIHCAcQIRiPAtIBCDUyNTJqMGo3qAIAsAIA&sourceid=chrome&ie=UTF-8 HTTP/1.1\r\nHost: example.com\r\nUser-Agent: curl/7.64.1\r\nAccept: */*\r\n\r\n";
    Request request(req);
    std::cout << "Method: " << request.method() << std::endl;
    std::cout << "URL: " << request.url() << std::endl;
    std::cout << "Version: " << request.version() << std::endl;
    std::cout << "Route: " << request.route() << std::endl;
    std::cout << "Query (q): " << request.query("q") << std::endl;
    std::cout << "Query (rlz): " << request.query("rlz") << std::endl;
    std::cout<<getDescription(200)<<std::endl;

    return 0;
}