#include "rest/core/RestServer.hpp"

#include <iostream>

using namespace web;
using namespace web::http;
using namespace std;
using namespace rest::core;


bool compareString::less(const char* first, const char* second){
    if (*first == '\0' && *second == '\0')
        return false;

    if (*first == '*' && *(first+1) != '\0' && *second == '\0')
        return false;

    if (*second == '*' && *(second+1) != '\0' && *first == '\0')
        return true;
    
    if (*first == *second)
        return less(first+1, second+1);

    if (*first == '*')
        return !(!less(first+1, second) || !less(first, second+1));

    if (*second == '*')
        return !(!less(first, second+1) || !less(first+1, second));
    
    return *first < *second;
}
//조건문 너무 길어서 거슬린다

Request::Request(http_request req) : request(req){}

const uri& Request::uri() const {
    return request.request_uri();
}

Response::Response(http_response resp) :  response(resp){}

Response::Response(const string& message) : response(status_codes::OK){
    response.set_body(message);
}

Response::Response(const string& message, status_code statusCode){
    response.set_status_code(statusCode);
    response.set_body(message);
}



RestServer::RestServer(const string& url) : listener(url){
    
    listener.support(methods::POST, [=](http_request req){
        map<string, SynchronizedHandler>::iterator find = urlMap.find(string("post").append(req.request_uri().to_string()));
        if(find == urlMap.end() || find->second == nullptr)
            req.reply(Response("",status_codes::NotFound).resp());
        else
            req.reply(find -> second(req).resp());
    });

    listener.support(methods::GET, [=](http_request req){
        map<string, SynchronizedHandler>::iterator find = urlMap.find(string("get").append(req.request_uri().to_string()));
        if(find == urlMap.end() || find->second == nullptr)
            req.reply(Response("",status_codes::NotFound).resp());
        else
            req.reply(find -> second(req).resp());
    });
    
}

RestServer::~RestServer(){
    listener.close();
}

void RestServer::Run(){
    listener.open().wait();
    return;
}

void RestServer::OnGet(const string& url, SynchronizedHandler handler){
    map<string, SynchronizedHandler>::iterator find = urlMap.find(string("get").append(url));
    if(find == urlMap.end())
        urlMap.insert(pair<string, SynchronizedHandler>(string("get").append(url), handler));
    else
        find->second = handler;
    
    return;
}

void RestServer::OnPost(const string& url, SynchronizedHandler handler){
    map<string, SynchronizedHandler>::iterator find = urlMap.find(string("post").append(url));
    if(find == urlMap.end())
        urlMap.insert(pair<string, SynchronizedHandler>(string("post").append(url), handler));
    else
        find->second = handler;
    return;
}