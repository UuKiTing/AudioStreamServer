# AudioStreamServer

基于 **C++17 / epoll Reactor** 实现的轻量级音频流媒体 HTTP 服务器。

底层 socket / epoll 使用 C 封装为静态库，上层网络通信、HTTP 处理及业务逻辑使用 C++17 实现。使用 MySQL 存储歌曲信息，为客户端提供歌曲元数据、音频、封面和歌词等资源。

## 特性

- 基于 **epoll Reactor** 的单线程网络模型
- 自定义 socket / epoll 网络层，不依赖第三方网络库
- 支持 HTTP `Range` 请求，实现音频分段传输
- 支持 HTTP Keep-Alive 连接复用
- 使用 MySQL 存储歌曲元数据
- 根据数据库校验文件名，限制文件访问范围

## 项目结构

```text
├── resource/              # 音频、封面、歌词资源
└── src/
    ├── app/               # 程序入口、路由及初始化
    ├── c_core/            # C socket / epoll 静态库
    ├── net/               # Reactor 网络层
    ├── http/              # HTTP 请求解析及响应
    ├── db/                # MySQL 数据库操作
    └── handler/           # 音频、封面、歌词及元数据处理
```

## 环境

- Linux
- GCC / Clang
- C11 / C++17
- CMake ≥ 3.14
- MySQL
- nlohmann/json

## 编译运行

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

./build/AudioStreamServer
```

服务器默认监听：

```text
0.0.0.0:8080
```

## HTTP API

| 方法 | 路径                     | 说明                 |
| ---- | ------------------------ | -------------------- |
| GET  | `/songsJson`             | 获取歌曲元数据       |
| GET  | `/songAudio/{fileName}`  | 获取音频，支持 Range |
| GET  | `/songImage/{fileName}`  | 获取歌曲封面         |
| GET  | `/songLyrics/{fileName}` | 获取歌曲歌词         |

### 示例

```bash
# 获取歌曲列表
curl http://127.0.0.1:8080/songsJson

# 获取音频并请求指定范围
curl -i -H "Range: bytes=0-1023" \
    http://127.0.0.1:8080/songAudio/Starboy.mp3
```

## 注意事项

- 程序需要在项目根目录运行，以正确访问 `resource/`。
- 数据库连接参数配置在 `src/app/main.cpp` 中，请根据实际环境修改。
- 数据库中的文件名需要与 `resource/` 中的实际文件保持一致。
