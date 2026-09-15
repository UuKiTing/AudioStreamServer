#include "TcpServer.hpp"

namespace net{
    TcpServer::TcpServer(EventLoop* loop, const std::string& ip, uint16_t port, const std::string& name) 
        : loop_(loop),
          acceptor_(new Acceptor(loop, ip, port)){

        acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, 
            this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    }

    TcpServer::~TcpServer() {
        for (auto& pair : connections_) {
            TcpConnectionPtr conn(pair.second);
            pair.second.reset();
        }
    }

    void TcpServer::start() {
        acceptor_->listen();
    }

    void TcpServer::setConnectionCallback(ConnectionCallback callBack) {
        connectionCallback_ = callBack;
    }

    void TcpServer::setMessageCallback(MessageCallback callBack) {
        messageCallback_ = callBack;
    }

    void TcpServer::newConnection(Socket client_soc, const std::string& ip, int port) {
        int fd = client_soc.fd();

        auto conn = std::make_shared<net::TcpConnection>(loop_, std::move(client_soc), ip, port);

        connections_[fd] = conn;

        conn->setMessageCallback(messageCallback_);

        conn->setCloseCallback(std::bind(&TcpServer::closeConnection, this, std::placeholders::_1));

        conn->connectEstablished();

        if(connectionCallback_){
            connectionCallback_(conn);
        }
    }

    void TcpServer::closeConnection(const TcpConnectionPtr& c) {
        std::cout << "Client disconnected! IP: " << c->getIp()
        << ", Port: " << c->getPort()
        << " [fd = " << c->getFd() << "]" << std::endl;
        
        connections_.erase(c->getFd());
    }

} // namespace net
