#include "rest/core/RestServer.hpp"

#include <iostream>


using namespace web;
using namespace web::http;
using namespace std;
using namespace rest::core;

Response::Response(http_response resp) :  response(resp){
}

Response::Response(const string& message) : response(status_codes::OK){
    response.set_body(message);
}

Response::Response(const string& message, status_code statusCode){
    response.set_status_code(statusCode);
    response.set_body(message);
}



RestServer::RestServer(const string& url) : listener(url){
    
    listener.support(methods::POST, [=](http_request req){
        map<string, SynchronizedHandler>::iterator find = urlMap.find(req.request_uri().to_string());
        if(find == urlMap.end())
            req.reply(Response("",status_codes::NotFound).resp());
        else
            req.reply(find -> second(req).resp());
    });

    listener.support(methods::GET, [=](http_request req){
        map<string, SynchronizedHandler>::iterator find = urlMap.find(req.request_uri().to_string());
        if(find == urlMap.end())
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
    urlMap.insert(pair<string, SynchronizedHandler>(url, handler));
    return;
}

void RestServer::OnPost(const string& url, SynchronizedHandler handler){
    urlMap.insert(pair<string, SynchronizedHandler>(url, handler));
    return;
}