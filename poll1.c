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
#define SERVER_PORT "8088"
// 密码
#define PWD 1234
// 数据大小
#define BUFSIZE 65535
#define SSL_CONNECTION_RESPONSE "HTTP/1.0 200 Connection established"
#define PROXY_AGENT "Proxy-agent: NG001"
#define EINTR 4 /* Interrupted function call */
int sum = 1;
struct timeval timeout = {0, 10000};
struct Client { 
    int client_sock; //sock地址
    int istsl;      //是否https
    int ishand;   //已经握手
}; 
struct Client Clients[1000];

// 超时自动断开
void timeclose(int connfd)
{
    /* Set timeout for client-side I/O operations */
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    setsockopt(connfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));
    setsockopt(connfd, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(timeout));
}
void d(char *str)
{
    // db(__FILE__, __LINE__, __func__, "输出内容：%s", str);
    printf("输出信息:%s\n", str);
}
void dd(int *str)
{
    // db(__FILE__, __LINE__, __func__, "输出内容：%s", str);
    printf("输出信息:%d\n", str);
}
// 打印16进制
void d16(const char *buffer)
{
    int size = sizeof(buffer) / sizeof(char);
    //  dd(&size);
    size_t len = 500;
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

/*
 * Set the socket to non blocking -rjkaes
 */
int socket_nonblocking(int sock)
{
    int flags;

    // assert (sock >= 0);

    flags = fcntl(sock, F_GETFL, 0);
    return fcntl(sock, F_SETFL, flags | O_NONBLOCK);
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
    strcpy(str, str);
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
/*
 * Set the socket to blocking -rjkaes
 */
int socket_blocking(int sock)
{
    int flags;
    flags = fcntl(sock, F_GETFL, 0);
    return fcntl(sock, F_SETFL, flags & ~O_NONBLOCK);
}

/*
 * Matched pair for safe_write(). If an EINTR occurs, pick up and try
 * again.
 */

ssize_t nread(int fd, char *buffer, size_t count)
{
    ssize_t len;

    do
    {
        len = read(fd, buffer, count);
    } while (len < 0 && errno == EINTR);

    return len;
}
/*
 * Write the buffer to the socket. If an EINTR occurs, pick up and try
 * again. Keep sending until the buffer has been sent.
 */
ssize_t nwrite(int fd, const char *buffer, size_t count)
{
    ssize_t len;
    size_t bytestosend;
    bytestosend = count;
    while (1)
    {
        len = send(fd, buffer, bytestosend, MSG_NOSIGNAL);

        if (len < 0)
        {
            if (errno == EINTR)
                continue;
            else
                return -errno;
        }

        if ((size_t)len == bytestosend)
            break;

        buffer += len;
        bytestosend -= len;
    }

    return count;
}
// 数据转发
void forward_data(int source_sock, int destination_sock)
{
    char buffer[BUFSIZE];
    size_t n;

    while ((n = nread(source_sock, buffer, BUFSIZE)) > 0)
    {

        nwrite(destination_sock, buffer, n);
    }

    shutdown(destination_sock, SHUT_RDWR);

    shutdown(source_sock, SHUT_RDWR);
}
// ip 端口转结构
struct sockaddr_in getadd(char *ip, char *port)
{
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(atoi(port));
    if (inet_pton(AF_INET, ip, &dest_addr.sin_addr) <= 0)
    {
        d("inet_pton");
        // return NULL;
    }
    return dest_addr;
}
// 转发
char *forward(int connfd, char *ip, char *port, char *buffer)
{

    int dest_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (dest_sock == -1)
    {
        perror("socket");
        return NULL;
    }
    struct sockaddr_in dest_addr = getadd(ip, port);

    if (connect(dest_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) == -1)
    {
        perror("connect");
        close(dest_sock);
        close(connfd);
        return NULL;
    }
    d16(buffer);
    forward_data(connfd, dest_sock);
    close(dest_sock);
    close(connfd);
    return "";
}
/*
 * Send a "message" to the file descriptor provided. This handles the
 * differences between the various implementations of vsnprintf. This code
 * was basically stolen from the snprintf() man page of Debian Linux
 * (although I did fix a memory leak. :)
 */
int write_message(int fd, const char *fmt, ...)
{
    ssize_t n;
    size_t size = (1024 * 8); /* start with 8 KB and go from there */
    char *buf, *tmpbuf;
    va_list ap;

    if ((buf = (char *)malloc(size)) == NULL)
        return -1;

    while (1)
    {
        va_start(ap, fmt);
        n = vsnprintf(buf, size, fmt, ap);
        va_end(ap);

        /* If that worked, break out so we can send the buffer */
        if (n > -1 && (size_t)n < size)
            break;

        /* Else, try again with more space */
        if (n > -1)
            /* precisely what is needed (glibc2.1) */
            size = n + 1;
        else
            /* twice the old size (glibc2.0) */
            size *= 2;

        if ((tmpbuf = (char *)realloc(buf, size)) == NULL)
        {
            free(buf);
            return -1;
        }
        else
            buf = tmpbuf;
    }

    if (nwrite(fd, buf, n) < 0)
    {
        free(buf);
        return -1;
    }
    free(buf);
    return 0;
}
_Bool checkTsl(char *buf)
{
    char *conntmp = "CONNECT";
    if (!strstr(buf, conntmp))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
/* 响应隧道连接请求  */
int send_tunnel_ok(int client_sock)
{
    char *resp = "HTTP/1.1 200 Connection Established\r\n\r\n";
    int len = strlen(resp);
    char buffer[len + 1];
    strcpy(buffer, resp);
    if (nwrite(client_sock, buffer, len) < 0)
    {
        perror("Send http tunnel response  failed\n");
        return -1;
    }
    return 0;
}
// 开启监听
void start()
{
}

// 域名转ip
char *domain_to_ip(char *domain)
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

// 判断是否TSL连接
// 提取域名端口信息
char *extract_host(char *buffer, char **host, char **ip, char **port)
{
    regex_t regex;
    int ret;
    // char msgbuf[1024];
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
            *host = cutstr(buffer, start + 5, end);
            // strcpy(host,host);

            char **tmp;
            tmp = split_string(*host, ":");
            *host = tmp[0];
            *port = tmp[1];

            *ip = domain_to_ip(*host);

            return *host; // return the host line
        }
    }
    else if (ret == REG_NOMATCH)
    {
        printf("匹配失败\n");
    }
    else
    {
        // regerror(ret, &regex, msgbuf, sizeof(msgbuf));
        // fprintf(stderr, "Regex match failed: %s\n", msgbuf);
        d("Regex match failed");
        return "";
    }

    // 释放正则表达式资源
    regfree(&regex);

    return "";
}

int main(int argc, char *argv[])
{
    int listenfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    char sendbuf[BUFSIZE], recvbuf[BUFSIZE];

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
    servaddr = getadd("0.0.0.0", SERVER_PORT);
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

    printf("Listening on port %s...\n", SERVER_PORT);

    while (1)
    {
        // 注册套接字到epoll实例
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = listenfd;
        epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);
        // 等待事件发生
        // int num_events = epoll_wait(epfd, events, 10, -1);//阻塞
        int num_events = epoll_wait(epfd, events, 10000, 0);//非阻塞
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
                char *destdomain, *destip, *destport;
                // 读取客户端请求体
                nread(connfd, recvbuf, BUFSIZE);
                strcpy(sendbuf, recvbuf);
                // 在HTTPS代理过程中，数据包传输过程中会发生以下变化：
                timeclose(connfd);
                //  d16(recvbuf);
                // 判断connect
                if (checkTsl(recvbuf))
                {
                    char *host = extract_host(recvbuf, &destdomain, &destip, &destport);
                    if (host == NULL)
                    {
                        d("没匹配到目标");
                        close(connfd);
                        break;
                    }
                    // char destdomain[100], destip[20], destport[10];

                    d("https请求-------------------------------");
                    d(destdomain);
                    d(destport);
                    send_tunnel_ok(connfd);
                    // char *response = forwardSslNoDecode(connfd, destip, atoi(destport), sendbuf);
                    break;
                }
                else
                {
                    // HTTP请求处理
                    char *host = extract_host(recvbuf, &destdomain, &destip, &destport);
                    if (host == NULL)
                    {
                        d("没匹配到目标");
                        close(connfd);
                        break;
                    }
                    d("http请求-------------------------------");
                    d(destdomain);
                    d(destport);
                    destip = domain_to_ip(destdomain);
                  
                    char *response = forward(connfd, destip, (destport), sendbuf);
                   
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
