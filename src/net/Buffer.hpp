#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <vector>
#include <string>
#include <algorithm>
#include <cstddef>

namespace net{

class Buffer{
public:
    static constexpr size_t initialSize = 1024; // 缓冲区初始大小
    static constexpr size_t prependSize = 8; // 缓冲区前置预留大小
    
    explicit Buffer(size_t initSize = initialSize);

    // 可读取数据大小
    size_t readableBytes() const;

    // 可写入数据大小
    size_t writableBytes() const;

    // 头部空闲数据大小
    size_t prependablebytes() const;

    // 添加数据
    void append(const char* data, size_t len);
    void append(const std::string& str);

    // 消费缓冲区 len 大小的数据
    void retrieve(size_t len);

    // 消费缓冲区所有数据
    void retrieveAll();

    // 消费缓冲区 len 大小的数据并返回消费数据string
    std::string retrieveAsString(size_t len);
    
    // 消费缓冲区所有数据并返回消费数据string
    std::string retrieveAllAsString();

    // 如果len大小数据不能写入缓冲区就腾空或者扩大缓冲区空间
    void ensureWritableBytes(size_t len);

    // 返回可读数据缓冲区起始位置
    const char* peek() const;

    // 读取 fd 的数据并写入缓冲区中
    ssize_t readDataInFd(int fd, int* saveErrno);

    // 在缓冲区中查找第一个 \r\n 的指针
    const char* findCRLF();

private:
    // 返回缓冲区的初始地址
    const char* begin() const;
    char* begin();

    // 腾出或者扩大缓冲区 len 大小的空间
    void makeSpace(size_t len);

    std::vector<char> buffer_;

    size_t readerIndex_; // 可读取起始位置偏移量
    size_t writerIndex_; // 可写入起始位置偏移量
};


}; // namespace net



#endif //BUFFER_HPP