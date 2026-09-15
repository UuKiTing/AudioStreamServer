#include "TcpServer.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HttpParser.hpp"
#include "MysqlDB.hpp"

void onConnection(const net::TcpConnectionPtr& conn){
    conn->setHttpParser(http::HttpParser());

    std::cout << "New client connected! IP: " << conn->getIp()
        << ", Port: " << conn->getPort()
        << " [fd = " << conn->getFd() << "]" << std::endl;
}

void onMessage(const net::TcpConnectionPtr& conn, net::Buffer* buf){
    http::HttpParser *parser = std::any_cast<http::HttpParser>(conn->getHttpParser());

    if(!parser->parseRequest(buf)){
        conn->send("HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
        conn->shutdown();
        return;
    }

    if(parser->isCompeted()){
        const http::HttpRequest request = parser->getRequest();

        std::cout << "[HTTP Request] Method: " << "GET"
                  << " Path: " << request.getPath() << std::endl;

        std::string responseBody = "<html><body><h1>Hello World from Reactor!</h1></body></html>";
        
        std::string response = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: text/html\r\n"
                               "Content-Length: " + std::to_string(responseBody.size()) + "\r\n"
                               "Connection: keep-alive\r\n\r\n" + responseBody;

        conn->send(response);

        parser->reset();
    }
}

int main(){ 
    auto& mysqlDb = db::MysqlDB::getInstance();
    if(!mysqlDb.init(
        "127.0.0.1",
        "luo",
        "123456",
        "db"
    )){
        std::cout << "mysql connect failed\n";
        return -1;
    }

    net::EventLoop loop;
    net::TcpServer server(&loop, "0.0.0.0", 8080);

    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);

    server.start();
    loop.loop();
    
    return 0;
}
                                                                        