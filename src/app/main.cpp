#include "TcpServer.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HttpParser.hpp"
#include "Router.hpp"
#include "DbManager.hpp"
#include "ReadData.hpp"


http::Router router;


void onConnection(const net::TcpConnectionPtr& conn){
    std::cout << "New client connected! IP: " << conn->getIp()
        << ", Port: " << conn->getPort()
        << " [fd = " << conn->getFd() << "]" << std::endl;
}

void onRequest(const net::TcpConnectionPtr& conn, net::Buffer* buf){
    http::HttpParser *parser = conn->getHttpParser();

    if(!parser->parseRequest(buf)){
        conn->send("HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
        conn->shutdown();
        return;
    }

    if(parser->isCompeted()){
        http::HttpRequest request = parser->getRequest();

        conn->send(router.dispatch(request));

        parser->reset();
    }
}

int main(){
    router.addRoute("/songsJson", false, http::Method::Get, handler::readSongsMetadata);
    router.addRoute("/songAudio", true, http::Method::Get, handler::readSongAudio);
    router.addRoute("/songImage", true, http::Method::Get, handler::readSongImage);
    router.addRoute("/songLyrics", true, http::Method::Get , handler::readSongLyrics);

    db::DbManager &dbManager = db::DbManager::getInstance();
    if(!dbManager.init("127.0.0.1", "luo", "123456", "db")){
        std::cout << "msyql connect failed!\n";
        return -1;
    }

    net::EventLoop loop;
    net::TcpServer server(&loop, "0.0.0.0", 8080);

    server.setConnectionCallback(onConnection);
    server.setRequestCallback(onRequest);

    server.start();
    loop.loop();
    
    return 0;
}
                                                                        