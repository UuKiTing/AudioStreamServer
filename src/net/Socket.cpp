#include "Socket.hpp"

namespace net{

    Socket::Socket(){
        fd_ = c_socket_create_tcp();
        if(fd_ < 0){
            throw std::runtime_error("Failed to create TCP socket!");
        }
    }

    Socket::Socket(int fd): fd_(fd){}

    Socket::~Socket() {
        if(fd_ >= 0){
            ::close(fd_);
        }
    }

    Socket::Socket(Socket &&other) noexcept: fd_(other.fd_) {
        other.fd_ = -1;
    }

    Socket& Socket::operator=(Socket &&other) noexcept {
        if(this != &other){
            if(fd_ >= 0){
                ::close(fd_);
            }
            fd_ = other.fd_;
            other.fd_ = -1;
        }
        return *this;
    }

    int Socket::fd() const { 
        return fd_; 
    }

    bool Socket::setNonBlocking() {
        return c_socket_set_nonblocking(fd_) == 0;
    }

    bool Socket::bindAndListen(std::string& client_ip, int port) {
        return c_socket_bind_and_listen(fd_, client_ip.data(), port) == 0;
    }

    int Socket::accept(std::string &client_ip, int& client_port) {
        char ip_buff[INET_ADDRSTRLEN] = {0};
        int port = 0;

        int client_fd = c_socket_accept(fd_, ip_buff, &port);

        if(client_fd >= 0){
            client_ip = ip_buff;
            client_port = port;
        }

        return client_fd;
    }

    bool Socket::isVaild() {
        return fd_ > 0;
    }

} // namespace net