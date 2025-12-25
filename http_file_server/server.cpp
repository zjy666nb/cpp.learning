#include<iostream>
#include<sys/socket.h>//socket编程
#include<netinet/in.h>//网络地址结构，IPv4，htons函数
#include<unistd.h>//提供close函数
#include<cstring>//提供memset
#include<thread>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <arpa/inet.h>
namespace fs = std::filesystem;

std::string get_content_type(const std::string&file_path){
    if(file_path.find("。heml")!=std::string::npos){
        return "text/html;charset=utf-8";
    }else if(file_path.find(".css")!=std::string::npos){
        return "text/css";
    }else if(file_path.find(".js")!=std::string::npos){
        return "application/javascript";
    }else if(file_path.find(".png")!=std::string::npos){
        return "image/png";
    }else if(file_path.find(".jpg")!=std::string::npos||
    file_path.find(".jpeg")!=std::string::npos){
        return "image/jpeg";
    }else if(file_path.find(".gif")!=std::string::npos){
        return "image/gif";
    }else if(file_path.find(".pdf")!=std::string::npos){
        return "application/pdf";
    }else if(file_path.find(".json")!=std::string::npos){
        return "text/plain";
    }else {
        return "www/index.html";
    }
}
std::string safe_file_path(const std::string& requested_path){
    if(requested_path=="/"||requested_path.empty()){
        return "www/index.html";
    }
    std::string path=requested_path;
    if(path[0]=='/'){
        path=path.substr(1);
    }

    std::string full_path="www/"+path;

    if(path.find("..")!=std::string::npos){
        std::cout<<"警告，检查到可疑路径"<<std::endl;
        return "www/404.html";
    }
    return full_path;
}

std::string read_file(const std::string& file_path){
    std::ifstream file(file_path,std::ios::binary);

    if(!file){
        std::cout<<"文件未找到"<<std::endl;
        return "";
    }
    file.seekg(0,std::ios::end);
    size_t file_size=file.tellg();
    file.seekg(0,std::ios::beg);

    std::string content(file_size,'\0');
    file.read(&content[0],file_size);
    file.close();
    return content;
}

std::string extract_request_path(const std::string& http_request){
    size_t start=http_request.find(' ');
    if(start==std::string::npos){
        return "/";
    }
    size_t end=http_request.find(' ',start+1);
    if(end==std::string::npos){
        return "/";
    }
    return http_request.substr(start+1,end-start-1);
}

void handle_client(int client_socket,int client_id){
    char buffer[8912];

    int bytes_received=recv(client_socket,buffer,sizeof(buffer)-1,0);
    if(bytes_received<=0){
        std::cout<<"客户端"<<client_id<<"断开连接"<<std::endl;
        close(client_socket);
        return;
    }
    buffer[bytes_received]='\0';
    std::string request(buffer);
    
    std::string request_path=extract_request_path(request);
    std::cout<<"客户端"<<client_id<<"请求路径"<<request_path<<std::endl;

    std::string file_path=safe_file_path(request_path);
    std::cout<<"映射到文件"<<file_path<<std::endl;

    std::string file_content=read_file(file_path);

    std::string http_response;

    if(!file_content.empty()){
          std::string content_type = get_content_type(file_path);
        
        http_response = "HTTP/1.1 200 OK\r\n";
        http_response += "Content-Type: " + content_type + "\r\n";
        http_response += "Content-Length: " + std::to_string(file_content.length()) + "\r\n";
        http_response += "Connection: close\r\n";
        http_response += "\r\n";  // 空行分隔头部和主体
        http_response += file_content;
        
        std::cout << "返回文件: " << file_path 
                  << " (类型: " << content_type 
                  << ", 大小: " << file_content.length() << " 字节)" << std::endl;
    } else {
        // 文件不存在，返回404
        std::string not_found_html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>404 - 页面未找到</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; padding: 50px; }
        h1 { color: #e74c3c; font-size: 48px; }
        p { font-size: 18px; color: #7f8c8d; }
        a { color: #3498db; text-decoration: none; }
    </style>
</head>
<body>
    <h1>404</h1>
    <p>抱歉，您请求的页面 <strong>)" + request_path + R"(</strong> 不存在。</p>
    <p><a href="/">返回首页</a></p>
</body>
</html>)";
        
        http_response = "HTTP/1.1 404 Not Found\r\n";
        http_response += "Content-Type: text/html; charset=utf-8\r\n";
        http_response += "Content-Length: " + std::to_string(not_found_html.length()) + "\r\n";
        http_response += "Connection: close\r\n";
        http_response += "\r\n";
        http_response += not_found_html;
        
        std::cout << "返回404: " << file_path << " 不存在" << std::endl;
    }
    
    // 发送响应
    int bytes_sent = send(client_socket, http_response.c_str(), http_response.length(), 0);
    if (bytes_sent <= 0) {
        std::cerr << "向客户端 " << client_id << " 发送响应失败" << std::endl;
    } else {
        std::cout << "向客户端 " << client_id << " 发送 " << bytes_sent << " 字节" << std::endl;
    }
    
    // 关闭连接
    close(client_socket);
    std::cout << "客户端 " << client_id << " 处理完成" << std::endl << std::endl;
}
    


int main() {
    std::cout << "=== 启动静态文件服务器 ===" << std::endl;
    std::cout << "服务器根目录: www/" << std::endl;
    
    // 检查www目录是否存在
    if (!fs::exists("www")) {
        std::cout << "创建 www 目录..." << std::endl;
        fs::create_directory("www");
    }
    
    // 创建示例文件
    std::cout << "创建示例文件..." << std::endl;
    
    // 首页
    std::ofstream index_file("www/index.html");
    index_file << R"(<!DOCTYPE html>
<html>
<head>
    <title>我的静态网站</title>
    <link rel="stylesheet" href="/style.css">
    <style>
        body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; }
        .container { max-width: 1000px; margin: 0 auto; padding: 20px; }
        header { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); 
                color: white; padding: 40px; border-radius: 10px; margin-bottom: 30px; }
        .features { display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); 
                   gap: 20px; margin: 30px 0; }
        .card { background: white; border-radius: 8px; padding: 20px; 
                box-shadow: 0 4px 6px rgba(0,0,0,0.1); }
        img { max-width: 100%; border-radius: 8px; }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>   欢迎来到我的C++服务器</h1>
            <p>这是一个完全用C++编写的多线程静态文件服务器</p>
        </header>
        
        <div class="features">
            <div class="card">
                <h2> 功能特性</h2>
                <ul>
                    <li>多线程并发处理</li>
                    <li>静态文件服务</li>
                    <li>支持HTML/CSS/JS</li>
                    <li>图片文件支持</li>
                    <li>404错误页面</li>
                </ul>
            </div>
            
            <div class="card">
                <h2> 文件列表</h2>
                <ul>
                    <li><a href="/">首页</a></li>
                    <li><a href="/about.html">关于我们</a></li>
                    <li><a href="/contact.html">联系我们</a></li>
                    <li><a href="/style.css">样式表</a></li>
                    <li><a href="/image.jpg">示例图片</a></li>
                </ul>
            </div>
            
            <div class="card">
                <h2> 技术栈</h2>
                <ul>
                    <li>C++11/14</li>
                    <li>Linux Socket</li>
                    <li>HTTP/1.1协议</li>
                    <li>多线程编程</li>
                    <li>文件I/O操作</li>
                </ul>
            </div>
        </div>
        
        <div class="card">
            <h2> 示例图片</h2>
            <p>这张图片由服务器提供：</p>
            <img src="/image.jpg" alt="示例图片" width="400">
            <p>图片文件直接从磁盘读取并传输到浏览器。</p>
        </div>
    </div>
    
    <script>
        console.log("页面加载完成！");
        document.querySelectorAll('a').forEach(link => {
            link.addEventListener('click', function(e) {
                console.log("导航到: " + this.href);
            });
        });
    </script>
</body>
</html>)";
    index_file.close();
    
    // 关于页面
    std::ofstream about_file("www/about.html");
    about_file << R"(<!DOCTYPE html>
<html>
<head>
    <title>关于我们</title>
    <link rel="stylesheet" href="/style.css">
</head>
<body>
    <div style="max-width: 800px; margin: 0 auto; padding: 40px;">
        <h1>关于这个项目</h1>
        <p>这是一个学习C++网络编程的实践项目。</p>
        
        <h2>项目目标</h2>
        <p>通过实现一个完整的HTTP服务器，掌握：</p>
        <ul>
            <li>Linux Socket编程</li>
            <li>HTTP协议解析</li>
            <li>多线程并发处理</li>
            <li>文件I/O操作</li>
            <li>错误处理和安全防护</li>
        </ul>
        
        
        <p><a href="/">返回首页</a></p>
    </div>
</body>
</html>)";
    about_file.close();
    
    // 样式表
    std::ofstream css_file("www/style.css");
    css_file << R"(* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
}

body {
    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, sans-serif;
    line-height: 1.6;
    color: #333;
    background-color: #f5f5f5;
}

h1, h2, h3 {
    color: #2c3e50;
    margin-bottom: 1rem;
}

a {
    color: #3498db;
    text-decoration: none;
    transition: color 0.3s;
}

a:hover {
    color: #2980b9;
}

.card {
    background: white;
    border-radius: 8px;
    padding: 1.5rem;
    box-shadow: 0 2px 10px rgba(0,0,0,0.1);
    transition: transform 0.3s;
}

.card:hover {
    transform: translateY(-5px);
}

ul {
    padding-left: 1.5rem;
}

li {
    margin-bottom: 0.5rem;
}

.container {
    max-width: 1200px;
    margin: 0 auto;
    padding: 2rem;
})";
    css_file.close();
    
    // 404页面
    std::ofstream notfound_file("www/404.html");
    notfound_file << R"(<!DOCTYPE html>
<html>
<head>
    <title>404 - 页面未找到</title>
    <style>
        body { 
            font-family: Arial, sans-serif; 
            text-align: center; 
            padding: 100px 20px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            min-height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
        }
        .error-box {
            background: rgba(255,255,255,0.1);
            backdrop-filter: blur(10px);
            padding: 40px;
            border-radius: 15px;
            max-width: 500px;
        }
        h1 { font-size: 72px; margin-bottom: 20px; }
        p { font-size: 18px; margin-bottom: 30px; opacity: 0.9; }
        .home-btn {
            display: inline-block;
            background: white;
            color: #667eea;
            padding: 12px 30px;
            border-radius: 30px;
            text-decoration: none;
            font-weight: bold;
            transition: transform 0.3s;
        }
        .home-btn:hover {
            transform: scale(1.05);
        }
    </style>
</head>
<body>
    <div class="error-box">
        <h1>404</h1>
        <p>抱歉，您访问的页面不存在或已被移除。</p>
        <a href="/" class="home-btn">返回首页</a>
    </div>
</body>
</html>)";
    notfound_file.close();
    
    std::cout << " 示例文件创建完成" << std::endl;
    
    // 创建服务器Socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "创建Socket失败" << std::endl;
        return -1;
    }
    
    // 设置Socket选项，避免地址占用错误
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "绑定端口失败" << std::endl;
        close(server_socket);
        return -1;
    }
    
    if (listen(server_socket, 10) == -1) {
        std::cerr << "监听失败" << std::endl;
        close(server_socket);
        return -1;
    }
    
    std::cout << "==============================================" << std::endl;
    std::cout << "静态文件服务器已启动" << std::endl;
    std::cout << "访问地址: http://localhost:8080" << std::endl;
    std::cout << "文件目录: " << fs::current_path().string() << "/www/" << std::endl;
    std::cout << "==============================================" << std::endl;
    
    int client_count = 0;
    
    while (true) {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_socket = accept(server_socket, (sockaddr*)&client_addr, &client_len);
        
        if (client_socket == -1) {
            std::cerr << "接受连接失败，继续等待..." << std::endl;
            continue;
        }
        
        client_count++;
        
        // 输出客户端信息
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        std::cout << "\n 客户端 #" << client_count << " 连接来自: " << client_ip << std::endl;
        
        // 创建线程处理客户端
        std::thread client_thread(handle_client, client_socket, client_count);
        client_thread.detach();
    }
    
    close(server_socket);
    return 0;
}