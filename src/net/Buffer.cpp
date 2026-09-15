#include "Buffer.hpp"
#include <unistd.h>
#include <sys/uio.h>
#include <cerrno>

namespace net{
    Buffer::Buffer(size_t initSize)
    :  buffer_(prependSize + initSize),
       readerIndex_(prependSize),
       writerIndex_(prependSize){}

    size_t Buffer::readableBytes() const{
        return writerIndex_ - readerIndex_;
    }

    size_t Buffer::writableBytes() const {
        return buffer_.size() - writerIndex_;
    }

    size_t Buffer::prependablebytes() const {
        return readerIndex_;
    }

    void Buffer::append(const char *data, size_t len) {
        ensureWritableBytes(len);
        std::copy(data, data + len, begin() + writerIndex_);
        writerIndex_ += len;
    }

    void Buffer::append(const std::string &str) {
        append(str.data(), str.size());
    }

    void Buffer::retrieve(size_t len) {
        if(len < readableBytes()){
            readerIndex_ += len;
        }
        else{
            retrieveAll();
        }
    }

    void Buffer::retrieveAll() {
        readerIndex_ = prependSize;
        writerIndex_ = prependSize;
    }

    std::string Buffer::retrieveAsString(size_t len) {
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    std::string Buffer::retrieveAllAsString() {
        return retrieveAsString(readableBytes());
    }

    void Buffer::ensureWritableBytes(size_t len) {
        if(writableBytes() < len){
            makeSpace(len);
        }
    }

    const char* Buffer::peek() const {
        return begin() + readerIndex_;
    }

    ssize_t Buffer::readDataInFd(int fd, int* saveErrno) {
        // 栈上准备 64KB 临时缓冲区
        char extrabuff[65536];
        struct iovec vec[2];
        const size_t writable = writableBytes();

        // 第一块缓冲区：指向 Buffer 内部剩余可写空间
        vec[0].iov_base = begin() + writerIndex_;
        vec[0].iov_len = writable;
        // 第二块缓冲区：指向栈上的 extrabuff
        vec[1].iov_base = extrabuff;
        vec[1].iov_len = sizeof(extrabuff);

        const int iovNums = (writable < sizeof(extrabuff)) ? 2 : 1;
        const ssize_t n = ::readv(fd, vec, iovNums);

        if(n < 0){
            *saveErrno = errno;
        }
        else if(static_cast<size_t>(n) <= writable){
            writerIndex_ += n;
        }
        else{
            writerIndex_ = buffer_.size();
            append(extrabuff, n - writable);
        }

        return n;
    }

    const char* Buffer::findCRLF() {
        for (int i = readerIndex_; i < writerIndex_ - 1; i++){
            if(buffer_[i] == '\r' && buffer_[i + 1] == '\n'){
                return &buffer_[i];
            }    
        }
    
        return nullptr;
    }

    const char* Buffer::begin() const{
        return &*buffer_.begin();
    }

    char* Buffer::begin() {
        return &*buffer_.begin();
    }

    void Buffer::makeSpace(size_t len) {
        if(writableBytes() + prependablebytes() < len - prependSize){
            buffer_.resize(writerIndex_ + len);
        }
        else{
            size_t readable = readableBytes();
            std::copy(begin() + readerIndex_, begin() + writerIndex_, begin() + prependSize);
            readerIndex_ = prependSize;
            writerIndex_ = readerIndex_ + readable;
        }
    }
} // namespace net