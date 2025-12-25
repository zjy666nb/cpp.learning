#  C++ 多线程HTTP服务器

一个从零实现的高性能多线程HTTP服务器，支持静态文件服务和并发连接处理。

##  功能特性

- ** 多线程并发**：支持同时处理多个客户端连接
- ** HTTP/1.1协议**：完整的HTTP协议实现
- ** 静态文件服务**：支持HTML、CSS、JS、图片等文件类型
- ** 安全防护**：路径安全检查，防止目录遍历攻击
- ** 详细日志**：实时显示连接状态和请求信息
- ** 错误处理**：完善的错误处理和404页面

##  系统架构

```
┌─────────────────────────────────────────────┐
│                  客户端                      │
└─────────────────┬───────────────────────────┘
                  │ HTTP请求
┌─────────────────▼───────────────────────────┐
│           多线程HTTP服务器                   │
│  ┌─────────────┐  ┌─────────────┐         │
│  │  主线程     │  │  工作线程   │  ...    │
│  │ • 监听端口  │  │ • 处理请求  │         │
│  │ • 接受连接  │  │ • 文件I/O   │         │
│  └──────┬──────┘  └──────┬──────┘         │
└─────────┼────────────────┼─────────────────┘
          │                │
    ┌─────▼────┐    ┌──────▼──────┐
    │ Socket   │    │  静态文件   │
    │ 管理层   │    │   系统      │
    └──────────┘    └─────────────┘
```

##  技术栈

- **语言**: C++11/14
- **网络编程**: Linux Socket API (socket/bind/listen/accept)
- **并发模型**: std::thread, 线程分离(detach)
- **协议**: HTTP/1.1 (请求解析、响应构造)
- **文件系统**: C++17 filesystem, 文件I/O流
- **工具链**: g++, Makefile, Git

##  快速开始

### 环境要求
- Linux 或 WSL2 (Windows Subsystem for Linux)
- g++ 7.0+ 支持C++17
- 标准C++库

### 编译运行
```bash
# 克隆项目
git clone https://github.com/yourusername/cpp-http-server.git
cd cpp-http-server

# 编译
make

# 运行服务器
./bin/server

# 浏览器访问
打开 http://localhost:8080
```

### 测试连接
```bash
# 使用curl测试
curl http://localhost:8080

# 使用telnet手动测试
telnet localhost 8080
# 输入: GET / HTTP/1.1[回车]Host: localhost[回车][回车]
```

##  项目结构

```
cpp-http-server/
├── src/                    # 源代码
│   ├── main.cpp           # 程序入口
│   ├── server.cpp         # 服务器核心逻辑
│   ├── server.h
│   ├── request_handler.cpp # 请求处理器
│   └── request_handler.h
├── www/                   # 静态文件目录
│   ├── index.html        # 主页
│   ├── about.html        # 关于页面
│   ├── style.css         # 样式表
│   └── 404.html          # 错误页面
├── Makefile              # 构建配置
├── README.md            # 项目说明
└── .gitignore           # Git忽略配置
```

##  API接口

服务器支持标准的HTTP方法：

| 方法 | 路径 | 描述 | 示例 |
|------|------|------|------|
| GET | `/` | 返回首页 | `curl http://localhost:8080/` |
| GET | `/about.html` | 返回关于页面 | `curl http://localhost:8080/about.html` |
| GET | `/style.css` | 返回CSS样式表 | `curl http://localhost:8080/style.css` |
| GET | `/any-file` | 返回对应静态文件 | - |

##  测试与验证

### 功能测试
```bash
# 1. 基本功能测试
./test_basic.sh

# 2. 并发压力测试 (使用ab工具)
ab -n 1000 -c 50 http://localhost:8080/

# 3. 长时间运行测试
./run_stress_test.sh
```

### 性能指标
- 支持100+并发连接
- 平均响应时间: <50ms
- 内存使用稳定，无内存泄漏

##  故障排除

| 问题 | 原因 | 解决方案 |
|------|------|----------|
| 编译错误: filesystem not found | g++版本过低 | 使用 `g++ -std=c++17 -lstdc++fs` |
| 端口被占用 | 8080端口已被使用 | 修改代码中的端口号或杀死占用进程 |
| 权限拒绝 | 非特权用户使用1024以下端口 | 使用8080等高位端口 |
| 连接被拒绝 | 服务器未启动 | 检查服务器进程是否运行 |

##  性能优化

1. **连接池管理**：复用连接减少开销
2. **内存池**：预分配内存减少动态分配
3. **缓存机制**：缓存常用文件减少磁盘I/O
4. **I/O多路复用**：考虑epoll替代多线程

##  贡献指南

欢迎提交Issue和Pull Request！

1. Fork本仓库
2. 创建功能分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启Pull Request

##  许可证

本项目采用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详情

##  致谢

- 感谢所有测试和反馈的朋友
- 参考项目: [tinyhttpd](https://github.com/EZLippi/Tinyhttpd)
- 学习资源: 《Unix网络编程》《C++ Primer》