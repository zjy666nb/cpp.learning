#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include <arpa/inet.h>
#include<cstring>
#include<thread>
// 这个函数会在一个新线程中运行，用于处理一个客户端的全部通信
void handle_client(int client_socket) {
    std::cout << "New client connected! Handling in thread." << std::endl;
    char buffer[1024];

    while (true) {
        // 清空缓冲区
        memset(buffer, 0, sizeof(buffer));
        
        // 接收客户端发来的数据
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            std::cout << "Client disconnected." << std::endl;
            break; // 连接断开，退出循环
        }

        // 打印收到的消息
        std::cout << "Received: " << buffer;

        // 回声：将收到的数据原样发回去
        int bytes_sent = send(client_socket, buffer, bytes_received, 0);
        if (bytes_sent <= 0) {
            std::cerr << "Send failed." << std::endl;
            break;
        }
        std::cout << "Echoed back." << std::endl;
    }

    // 关闭这个客户端的专用Socket
    close(client_socket);
}
int main(){
   int server_socket = socket(AF_INET,SOCK_STREAM,0);
   if(server_socket==-1){
        std::cerr<<"Failed to create socket"<<std::endl;
        return -1;
    }
    std::cout<<"Socket created successfully"<<std::endl;
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(8888);
    server_addr.sin_addr.s_addr=INADDR_ANY;
    if((bind(server_socket,(sockaddr*)&server_addr,sizeof(server_addr)))==-1
    ){
        std::cerr<<"Bind failed"<<strerror(errno)<<std::endl;
        close(server_socket);
        return -1;
    }
    std::cout<<"Bind successful on port 8888"<<std::endl;
    if(listen(server_socket,5)==-1){
        std::cerr<<"listen failed"<<std::endl;
        close(server_socket);
        return -1;
    }
    std::cout<< "Echo server is listening on port 8888..."<<std::endl;
        // 4. 主循环：不断接受新的客户端连接
    while (true) {
        sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        // 接受一个 incoming connection (会阻塞直到有客户端连接)
        int client_socket = accept(server_socket, (sockaddr*)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            std::cerr << "Accept failed!" << std::endl;
            continue; // 接受失败，继续尝试接受下一个
        }

        std::cout << "A new client is connecting..." << std::endl;

        // 创建一个新线程来处理这个客户端，并立即分离它 (detach)
        std::thread client_thread(handle_client, client_socket);
        client_thread.detach(); // 让线程独立运行，主线程不再等待它
    }
    std::cout << "Server is running. Press Ctrl+C to exit." << std::endl;
    while (true) {
        // 一个空循环，保持服务器运行
        sleep(1);
    }
    close(server_socket);
    return 0;
}