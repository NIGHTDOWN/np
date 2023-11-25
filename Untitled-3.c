

/**  
 * @brief SSL代理服务器  
 *  
 * 该代码实现了一个SSL代理服务器，可以接收来自客户端的SSL加密的HTTP请求，  
 * 然后将请求转发给后端的非SSL代理服务器或目标服务器，并将响应返回给客户端。  
 *  
 * @param argc 命令行参数的数量  
 * @param argv 命令行参数数组  
 *  
 * @return 程序执行成功返回0，否则返回非0值  
 */  
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <errno.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <sys/socket.h>  
#include <netdb.h>  
#include <openssl/bio.h>  
#include <openssl/ssl.h>  
#include <openssl/err.h>  
  
/**  
 * @brief 创建socket并绑定到本地地址  
 *  
 * @param sockfd 存储创建的socket文件描述符  
 * @param local_addr 本地地址信息  
 *  
 * @return 成功返回0，失败返回-1  
 */  
int create_socket(int *sockfd, struct sockaddr_in *local_addr) {  
    int sock = socket(AF_INET, SOCK_STREAM, 0);  
    if (sock == -1) {  
        perror("ERROR opening socket");  
        return -1;  
    }  
    *sockfd = sock;  
    memset(local_addr, 0, sizeof(struct sockaddr_in));  
    local_addr->sin_family = AF_INET;  
    local_addr->sin_addr.s_addr = INADDR_ANY;  
    local_addr->sin_port = htons(8080);  
    if (bind(*sockfd, (struct sockaddr *) local_addr, sizeof(struct sockaddr_in)) == -1) {  
        perror("bind");  
        return -1;  
    }  
    if (listen(*sockfd, 10) == -1) {  
        perror("listen");  
        return -1;  
    }  
    return 0;  
}  
  
/**  
 * @brief 接受客户端连接请求并建立SSL连接  
 *  
 * @param sockfd 已绑定的socket文件描述符  
 * @param client_fd 存储建立的客户端连接的文件描述符  
 * @param ssl_client 存储建立的SSL结构体指针  
 * @param ctx 存储SSL上下文指针  
 *  
 * @return 成功返回0，失败返回-1  
 */  
int accept_connection(int sockfd, int *client_fd, SSL **ssl_client, SSL_CTX *ctx) {  
    struct sockaddr_in client_addr;  
    socklen_t client_len = sizeof(struct sockaddr_in);  
    *client_fd = accept(sockfd, (struct sockaddr *) &client_addr, &client_len);  
    if (*client_fd == -1) {  
        perror("accept");  
        return -1;  
    }  
    *ssl_client = SSL_new(ctx);  
    SSL_set_fd(*ssl_client, *client_fd);  
    if (SSL_accept(*ssl_client) <= 0) {  
        ERR_print_errors_fp(stderr);  
        return -1;  
    }  
    return 0;  
}  
  
/**  
 * @brief 通过TCP连接向后端服务器发送数据，并接收响应数据，同时进行SSL解密处理。  
 *  
 * @param client_fd 客户端连接的文件描述符（已建立SSL连接）  
 * @param server_fd 存储建立的服务器连接的文件描述符（未加密）  
 * @param ctx SSL上下文指针，用于SSL解密处理。  
 * @param buffer 存储接收到的响应数据的缓冲区（最大长度为1024字节）  
 * @param bytes 实际接收到的响应数据的字节数，需要减去SSL解密后的额外字节。  
 * @param server_addr 存储目标服务器地址信息的结构体指针（用于TCP连接）  
 * @param server_len 目标服务器地址信息的结构体长度（用于TCP连接）  
 * @param server_addrlen 目标服务器地址信息的结构体长度（用于TCP连接）的变量指针。  
 * @param err 处理过程中遇到的错误信息（如果有）。如果成功则为NULL。可以忽略。  
 * @return 成功返回0，失败返回-1。如果失败，需要释放已分配
 * 的资源，并关闭文件描述符。
*/
int send_and_receive_with_ssl_decrypt(int client_fd, int server_fd, SSL_CTX *ctx, char *buffer, int *bytes, struct sockaddr_in *server_addr, socklen_t server_addrlen, const char **err) {
SSL *ssl_server;
BIO *bio_server;
int tmp_bytes;
char *tmp_buffer;

/* Create SSL/TLS structure for server-side communication */  
ssl_server = SSL_new(ctx);  
SSL_set_fd(ssl_server, server_fd);  
if (SSL_accept(ssl_server) <= 0) {  
    ERR_print_errors_fp(stderr);  
    *err = "SSL_accept failed";  
    return -1;  
}  
 
/* Send data from client to server using SSL/TLS connection */  
tmp_buffer = buffer;  
while (*bytes > 0) {  
    tmp_bytes = SSL_write(ssl_server, tmp_buffer, *bytes);  
    if (tmp_bytes < 0) {  
        ERR_print_errors_fp(stderr);  
        *err = "SSL_write failed";  
        return -1;  
    }  
    tmp_buffer += tmp_bytes;  
    *bytes -= tmp_bytes;  
}  
 
/* Receive data from server using plain TCP connection */  
if (SSL_shutdown(ssl_server) <= 0) {  
    ERR_print_errors_fp(stderr);  
    *err = "SSL_shutdown failed";  
    return -1;  
}  
SSL_free(ssl_server);  
bio_server = BIO_new(BIO_f_ssl());  
BIO_set(bio_server, BIO_s_socket());  
BIO_set(bio_server, BIO_NOCLOSE);  
BIO_set(bio_server, server_fd);  
tmp_buffer = buffer;  
while ((tmp_bytes = BIO_read(bio_server, tmp_buffer, *bytes)) > 0) {  
    tmp_buffer += tmp_bytes;  
    *bytes -= tmp_bytes;  
}  
if (tmp_bytes < 0) {  
    ERR_print_errors_fp(stderr);  
    *err = "BIO_read failed";  
    return -1;  
}  
 
return 0;
}

int main(int argc, char **argv) {
int sockfd, client_fd, server_fd;
SSL *ssl_client, *ssl_server;
SSL_CTX *ctx;
char buffer[1024];
int bytes;
struct sockaddr_in server_addr = { 0 };
socklen_t server_addrlen = sizeof(struct sockaddr_in);
const char *err = NULL;

/* Create SSL/TLS context */  
ctx = SSL_CTX_new(TLSv1_2_server_method());  
if (ctx == NULL) {  
    ERR_print_errors_fp(stderr);  
    exit(EXIT_FAILURE);  
}  
SSL_CTX_set_ecdh(ctx, SSL_CTX_get_ecdh(ctx));
SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION);

/* Create socket and bind it to local address */  
if (create_socket(&sockfd, &server_addr) != 0) {  
    exit(EXIT_FAILURE);  
}  
 
/* Accept client connection and establish SSL/TLS connection */  
if (accept_connection(sockfd, &client_fd, &ssl_client, ctx) != 0) {  
    exit(EXIT_FAILURE);  
}  
 
/* Send data from client to server using SSL/TLS connection */  
bytes = SSL_read(ssl_client, buffer, sizeof(buffer) - 1);  
if (bytes < 0) {  
    ERR_print_errors_fp(stderr);  
    exit(EXIT_FAILURE);  
} else if (bytes == 0) {  
    printf("Server shutdown\n");  
    exit(EXIT_SUCCESS);  
} else {  
    buffer[bytes] = '\0';  
    printf("Received from client: %s\n", buffer);  
}  
 
/* Send data from server to client using SSL/TLS connection */  
SSL_write(ssl_client, "Hello from server!", strlen("Hello from server!"));  
 
/* Shutdown SSL/TLS connections */  
SSL_shutdown(ssl_client);  
SSL_shutdown(ssl_server);  
SSL_free(ssl_client);  
SSL_free(ssl_server);  
BIO_free(bio_client);  
BIO_free(bio_server);  
close(client_fd);  
close(server_fd);  
close(sockfd);  
 
/* Free SSL/TLS context */  
SSL_CTX_free(ctx);  
 
return 0;
}
/* Create socket and bind it to local address */  
if (create_socket(&sockfd, &server_addr) != 0) {  
    fprintf(stderr, "Failed to create socket or bind to local address\n");  
    return -1;  
}  
  
/* Accept client connection and establish SSL/TLS connection */  
if (accept_connection(sockfd, &client_fd, &ssl_client, ctx) != 0) {  
    fprintf(stderr, "Failed to accept client connection or establish SSL/TLS connection\n");  
    return -1;  
}  
  
/* Send data from client to server using SSL/TLS connection */  
if (SSL_read(ssl_client, buffer, sizeof(buffer) - 1) < 0) {  
    fprintf(stderr, "Failed to read data from client using SSL/TLS connection\n");  
    ERR_print_errors_fp(stderr);  
    return -1;  
} else {  
    buffer[bytes] = '\0';  
    printf("Received from client: %s\n", buffer);  
}  
  
/* Send data from server to client using SSL/TLS connection */  
if (SSL_write(ssl_client, "Hello from server!", strlen("Hello from server!")) <= 0) {  
    fprintf(stderr, "Failed to write data to client using SSL/TLS connection\n");  
    ERR_print_errors_fp(stderr);  
    return -1;  
}
/* Shutdown SSL/TLS connections */  
if (SSL_shutdown(ssl_client) <= 0) {  
    fprintf(stderr, "Failed to shutdown SSL/TLS connection for client\n");  
    ERR_print_errors_fp(stderr);  
    return -1;  
}  
if (SSL_shutdown(ssl_server) <= 0) {  
    fprintf(stderr, "Failed to shutdown SSL/TLS connection for server\n");  
    ERR_print_errors_fp(stderr);  
    return -1;  
}  
  
/* Free SSL/TLS resources */  
SSL_free(ssl_client);  
SSL_free(ssl_server);  
  
/* Free BIO resources */  
BIO_free(bio_client);  
BIO_free(bio_server);  
  
/* Close file descriptors */  
close(client_fd);  
close(server_fd);  
close(sockfd);  
  
/* Free SSL/TLS context */  
SSL_CTX_free(ctx);
// /* Set timeout for client-side I/O operations */  
// struct timeval timeout;  
// timeout.tv_sec = 5;  
// timeout.tv_usec = 0;  
// setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));  
// setsockopt(client_fd, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(timeout));


/* Create non-blocking socket and set it as non-blocking mode */  
int flags = fcntl(sockfd, F_GETFL, 0);  
if (flags == -1) {  
    perror("Failed to get socket flags");  
    return -1;  
}  
fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
这段代码创建了一个非阻塞的socket，并将其设置为非阻塞模式。这样可以让socket操作不阻塞程序的执行，提高程序的响应性和吞吐量。

注意，在非阻塞模式下，read和write操作可能会返回部分数据，需要循环读取或写入直到完成。同时，也需要处理由于数据不完整而导致的读写错误。


/* Create SSL/TLS structure for server-side communication */  
ssl_server = SSL_new(ctx);  
if (ssl_server == NULL) {  
    fprintf(stderr, "Failed to create SSL/TLS structure for server-side communication\n");  
    ERR_print_errors_fp(stderr);  
    return -1;  
}  
SSL_set_fd(ssl_server, server_fd);  
if (SSL_accept(ssl_server) <= 0) {  
    fprintf(stderr, "Failed to accept client connection or establish SSL/TLS connection\n");  
    ERR_print_errors_fp(stderr);  
    return -1;  
}  
  
/* Send data from client to server using SSL/TLS connection */  
bytes = SSL_read(ssl_client, buffer, sizeof(buffer) - 1);  
if (bytes < 0) {  
    fprintf(stderr, "Failed to read data from client using SSL/TLS connection\n");  
    ERR_print_errors_fp(stderr);  
    return -1;  
} else {  
    buffer[bytes] = '\0';  
    printf("Received from client: %s\n", buffer);  
}
这段代码创建了一个SSL/TLS结构体，用于服务器端的通信。通过调用SSL_set_fd将socket描述符与SSL结构体关联起来，然后调用SSL_accept接受客户端的连接并建立SSL/TLS连接。接着，使用SSL_read从客户端读取数据，并输出到控制台。

/* Free BIO resources */  
BIO_free(bio_client);  
BIO_free(bio_server);
这段代码释放了SSL/TLS和BIO的资源，避免了内存泄漏。

注意，在实际应用中，还需要考虑处理各种异常情况和安全问题，例如SSL/TLS版本过低、证书验证失败、密码套件不匹配等。同时，为了提高安全性，建议使用TLS 1.2或更高版本，关闭不必要的功能，如SSLv2和SSLv3等。