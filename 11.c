
要改进以上代码以支持 SSL，您需要进行以下步骤：

引入必要的头文件：在代码中引入 <openssl/ssl.h> 和 <openssl/err.h> 头文件，以便使用 OpenSSL 库的功能。
初始化 OpenSSL：在代码中调用 SSL_library_init() 函数来初始化 OpenSSL 库。
创建 SSL 上下文：使用 SSL_CTX_new() 函数创建一个 SSL 上下文对象，这将用于配置 SSL 连接。
配置 SSL 上下文：根据您的需求，使用 SSL_CTX_set_options() 函数设置 SSL 上下文的各种选项。
加载证书和私钥：使用 SSL_CTX_use_certificate_file() 和 SSL_CTX_use_PrivateKey_file() 函数加载您的证书和私钥文件。
创建 SSL 连接：使用 SSL_new() 函数基于先前创建的 SSL 上下文创建一个 SSL 连接对象。
设置回调函数：根据您的需求，可以设置回调函数来处理 SSL 连接的各种事件，例如握手完成、数据传输等。
进行 SSL 连接：使用 SSL_connect() 函数与目标服务器建立 SSL 连接。
进行数据传输：使用 SSL_write() 和 SSL_read() 函数进行数据的发送和接收。
关闭 SSL 连接：使用 SSL_shutdown() 函数关闭 SSL 连接，并释放相关资源。
清理 OpenSSL：使用 SSL_CTX_free() 和 EVP_cleanup() 函数清理 OpenSSL 资源。
下面是一个示例代码，演示了如何将以上步骤应用于改进的代码以支持 SSL：

c
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <openssl/ssl.h>  
#include <openssl/err.h>  
  
#define MAX_BUFFER 1024  
  
int main(int argc, char *argv[]) {  
    int sockfd, client_sockfd;  
    struct sockaddr_in server_addr, client_addr;  
    char buffer[MAX_BUFFER];  
    int buffer_len = MAX_BUFFER;  
    int total_bytes = 0;  
  
    // 初始化 OpenSSL  
    SSL_library_init();  
    SSL_CTX *ctx = SSL_CTX_new(SSLv23_server_method());  
  
    // 设置证书和私钥文件路径（示例）  
    const char *cert_file = "cert.pem";  
    const char *key_file = "key.pem";  
    if (SSL_CTX_use_certificate_file(ctx, cert_file, SSL_FILETYPE_PEM) <= 0) {  
        fprintf(stderr, "Error loading certificate file\n");  
        exit(EXIT_FAILURE);  
    }  
    if (SSL_CTX_use_PrivateKey_file(ctx, key_file, SSL_FILETYPE_PEM) <= 0) {  
        fprintf(stderr, "Error loading private key file\n");  
        exit(EXIT_FAILURE);  
    }  
    SSL_CTX_set_options(ctx, SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1); // 设置 SSL 选项（示例）  
  
    // 创建 socket  
    sockfd = socket(AF_INET, SOCK_STREAM, 0);  
    if (sockfd == -1) {  
        perror("socket");  
        exit(EXIT_FAILURE);  
    }  
  
    // 配置 socket 地址结构  
    memset(&server_addr, 0, sizeof(server_addr));  
    server_addr.sin_family = AF_INET;  
    server_addr.sin_addr.s_addr = INADDR_ANY;  
    server_addr.sin_port = htons(8080);  // 配置代理端口为8080  
  
    // 绑定 socket  
    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {  
        perror("bind");  
        exit(EXIT_FAILURE);  
    }  
  
    // 监听 socket  
    if (listen(sockfd, 5) == -1) {  
        perror("listen");  
        exit(EXIT_FAILURE);
SSL *ssl;  
int ssl_sockfd;  
  
// Accept client connection  
socklen_t client_addr_len = sizeof(client_addr);  
ssl_sockfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_addr_len);  
if (ssl_sockfd == -1) {  
    perror("accept");  
    exit(EXIT_FAILURE);  
}  
  
// Create SSL object for the new socket  
ssl = SSL_new(ctx);  
SSL_set_fd(ssl, ssl_sockfd);  
  
// Establish SSL connection  
if (SSL_accept(ssl) == -1) {  
    fprintf(stderr, "SSL_accept failed\n");  
    ERR_print_errors_fp(stderr);  
    exit(EXIT_FAILURE);  
}
// Read data from client  
total_bytes = SSL_read(ssl, buffer, buffer_len);  
if (total_bytes < 0) {  
    fprintf(stderr, "SSL_read failed\n");  
    ERR_print_errors_fp(stderr);  
} else if (total_bytes == 0) {  
    printf("Client disconnected\n");  
} else {  
    // Process the received data  
    // ...  
}  
  
// Write data to client  
const char *response = "Hello from SSL proxy!";  
total_bytes = SSL_write(ssl, response, strlen(response));  
if (total_bytes < 0) {  
    fprintf(stderr, "SSL_write failed\n");  
    ERR_print_errors_fp(stderr);  
} else if (total_bytes == 0) {  
    printf("SSL connection closed\n");  
}
// Close SSL connection and release resources  
SSL_shutdown(ssl);  
SSL_free(ssl);  
close(ssl_sockfd);
// Cleanup SSL context  
SSL_CTX_free(ctx);