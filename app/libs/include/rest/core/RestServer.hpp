#pragma once

#include <string>
#include <functional>
#include <cstring>

#include <cpprest/http_msg.h>
#include <cpprest/http_listener.h>

namespace rest
{
namespace core
{



struct compareString{
    bool less(const char* first,const char* second);
    bool operator()(const std::string& lhs, const std::string& rhs) {
        std::cout << lhs << " / " << rhs << " = " << less(lhs.c_str(), rhs.c_str()) <<"\n"; // 오류 없으면 cout 안나옴
        return less(lhs.c_str(), rhs.c_str());
    }
};


struct Request {
    Request(web::http::http_request req);

    const web::http::uri& uri() const;

    private:
        web::http::http_request request;
};

struct Response {
    Response(web::http::http_response resp);
    Response(const std::string& message);
    Response(const std::string& message, web::http::status_code statusCode);

    web::http::status_code code() const;

    const web::http::http_response& resp() const { return response; }

    private:
        web::http::http_response response;
};

inline bool operator == (const Response& lhs, const Response& rhs)
{
    if(lhs.code() == rhs.code()){
        return true;
    }

    return false;
}

using SynchronizedHandler = std::function<Response(const Request& req)>;

class RestServer
{
private:
    web::http::experimental::listener::http_listener listener;
    std::map<std::string, SynchronizedHandler, compareString> urlMap;
public:
    RestServer(const std::string& url);
    ~RestServer();

    void Run();

    void OnGet(const std::string& url, SynchronizedHandler handler);
    void OnPost(const std::string& url, SynchronizedHandler handler);
};


}
}