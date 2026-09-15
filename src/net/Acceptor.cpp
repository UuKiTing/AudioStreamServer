#include "Acceptor.hpp"
#include "EventLoop.hpp"
#include <iostream>

namespace net{
    Acceptor::Acceptor(EventLoop* loop, std::string ip, int port) : loop_(loop){
        server_soc.setNonBlocking();
        server_soc.bindAndListen(ip, port);
    }

    Acceptor::~Acceptor() {
        if(listening_){
            if(!loop_->delFd(server_soc.fd())){

            }
        }
    }

    void Acceptor::setNewConnectionCallback(const NewConnectionCallback& callBack) {
        newConnectionCallback_ = callBack;
    }

    void Acceptor::listen() {
        listening_ = true;

        bool res = loop_->addFd(server_soc.fd(), EPOLLIN, [&](uint32_t events){
            if(events & EPOLLIN){   
                handle();
            }
        });


        if(!res){
            std::cerr << "[Acceptor::listen] EventLoop::addFd()函数调用失败！";
        }
    }

    void Acceptor::handle() {
        while(true){
            std::string client_ip;
            int client_port;

            int client_fd = server_soc.accept(client_ip, client_port);

            if(client_fd > 0){
                net::Socket client_soc{client_fd};

                if(!client_soc.isVaild()){
                    std::cerr << "[Acceptor::handle] client_soc创建失败！\n";
                    continue;
                }

                if(!client_soc.setNonBlocking()){
                    std::cerr << "[Acceptor::handle] 为fd=" << client_fd << "设置非阻塞模式失败！\n";
                    continue;
                }

                if(newConnectionCallback_){
                    newConnectionCallback_(std::move(client_soc), client_ip, client_port);
                }
                else{
                    std::cerr << "[Acceptor::handle] newConnectionCallback_没有被设置！\n";
                }
            }
            else{
                break;
            }

        }
    }


} // namespace net