#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>
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
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
// 缓存管道；创建共享内存空间
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#define Malloc(type, n) (type *)malloc((n) * sizeof(type))
#define d(msg)                                                       \
    {                                                                \
        printf(" %s                           %d\n", msg, __LINE__); \
                                                                     \
        printf("\n");                                                \
    }

#define SERVER_PORT "8088"
#define _BACKLOG_ 5
#define _BUF_SIZE_ 2099
#define CACHE_SIZE 840000 // 缓存大小
#define _MAX_ 64
#define MAX_KEYS 100
#define MAX_KEY_LENGTH 150
#define SHMID_KEY 0x169
// 守护进程标识；0非后台进程，1后台进程
int ndaemon = 0;
static int m_pid;
int exptime = 3600;
clock_t start_time, end_time;
pid_t pid, pid2, pid3, pid4;
void sigchld_handler(int signal)
{
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
}
void regfork()
{
    signal(SIGCHLD, sigchld_handler);
}
struct timeval timeout = {0, 1000000};

void inittime()
{
    start_time = clock(); // 记录程序开始执行时的时间
}
// 显示运行毫秒
void runtime()
{
    end_time = clock();                                                     // 记录程序结束执行时的时间
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC; // 计算程序运行耗时，单位为秒
    printf("程序运行耗时：%f秒\n", elapsed_time);
    return;
}

// 客户端连接对象包含用户端以及真实服务器
typedef struct client
{
    int fd;     // 客户端套子接
    int rd;     // 远程端套子接
    int istsl;  // 是否tsl协议
    int isinit; // 是否初始化
    int issock; // 是否sock5
    int ishand; // 是否建立握手
    char *host; // 是否建立握手
    char *ip;   // 是否建立握手
    char *port; // 是否建立握手

    char buf[_BUF_SIZE_];
} client_t, *client_p;
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

void dptr(void *ptr, const char *type)
{
    printf("Address of %s: %p\n", type, ptr);
}

void client_free(client_p clientobj)
{
    close(clientobj->fd);
    close(clientobj->rd);
    free(clientobj);
}
void checkpwd(client_p clientobj)
{
    // Proxy-Authorization:Basic dXNlcjpwd2Q= 　　 //身份验证信息
    // 数据头这块有这个参数；curl -x proxy -U user:pwd
    // 转发到代理就是  base64(user:pwd)   ==  dXNlcjpwd2Q=
}
// 字符串复制
char *strcp(char *dest, char *src)
{
    if (dest == NULL || src == NULL)
    {
        return NULL;
    }
    strcpy(dest, src);
    return dest;
}
void dd(int str)
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
/***************缓存**********/

struct ArrayNode
{
    time_t access_time; // 访问时间
    time_t expire_time; // 过期时间
    char values[MAX_KEY_LENGTH];
};
struct Array
{
    int size;
    char keys[MAX_KEYS][MAX_KEY_LENGTH];
    struct ArrayNode anode[MAX_KEYS];
};

char *array_find(const struct Array *my_array, const char *key)
{
    char *ret = NULL;
    int index = my_array->size - 1;
    if (index <= 0)
    {
        return ret;
    }
    char search_key[MAX_KEY_LENGTH];
    char *search_value = NULL;
    int i;
    strcpy(search_key, key);
    for (i = 0; i < index; i++)
    {
        if (strcmp(search_key, my_array->keys[i]) == 0)
        {
            search_value = strdup(my_array->anode[i].values);
            break;
        }
    }
    return search_value;
}
int array_clear(struct Array *my_array)
{
    // free(my_array->keys);
    // free(my_array->anode);
    free(my_array);
    my_array->size = 0;
}
int array_push(struct Array *my_array, char *key, char *val, time_t expire_in)
{
    if (my_array->size + 1 >= MAX_KEYS)
    {
        // 当数组已经满了；清空重新保存缓存/************这里可以优化lru算法来删除查询少的缓存给新数据提供空间
        array_clear(my_array);
    }
    if (!my_array->size)
        my_array->size = 1;
    int index = my_array->size - 1;
    strcpy(my_array->keys[index], key);
    struct ArrayNode vnode;
    //  vnode.values=val;
    vnode.access_time = time(NULL);

    if (expire_in == 0)
    {                        // 如果没有指定过期时长，使用默认时长
        expire_in = exptime; // 默认时长为 60 秒
    }

    vnode.expire_time = time(NULL) + expire_in;

    my_array->anode[index].access_time = vnode.access_time;

    my_array->anode[index].expire_time = vnode.expire_time;

    strcpy(my_array->anode[index].values, val);
    // my_array->anode[index].values =tmp;
    // strcpy(my_array->anode[index].values,tmp);
    // memcpy(my_array->anode[index].values,tmp,sizeof(tmp)+1);
    // strncpy(my_array->anode[index].values, vnode.values, sizeof(my_array->anode[index].values) - 1);
    // my_array->anode[index].values[sizeof(my_array->anode[index].values) - 1] = '\0';
    int size = my_array->size++;
    return size;
}

struct CacheNode
{
    char key[128];
    char value[128];
    struct CacheNode *prev;
    struct CacheNode *next;
    time_t access_time; // 访问时间
    time_t expire_time; // 过期时间
};

struct Cache
{
    pthread_mutex_t mutex; // 互斥量，用于同步访问缓存
    struct CacheNode *head;
    struct CacheNode *tail;
    int size;
};

// 初始化LRU缓存的共享内存
int initLRUCacheSharedMemory(key_t key)
{
    int shmidtmp;
    // 创建共享内存
    if ((shmidtmp = shmget(key, CACHE_SIZE, IPC_CREAT | 0666)) < 0)
    {
        //         SHMMAX 的默认值是 32MB 。一般使用下列方法之一种将 SHMMAX 参数设为 2GB ：
        // 通过直接更改 /proc 文件系统，你不需重新启动机器就可以改变 SHMMAX 的默认设置。我使用的方法是将以下命令放入 />etc/rc.local 启动文件中：
        // echo "2147483648" > /proc/sys/kernel/shmmax
        // 您还可以使用 sysctl 命令来更改 SHMMAX 的值：sysctl -w kernel.shmmax=2147483648
        // 最后，通过将该内核参数插入到 /etc/sysctl.conf 启动文件中，您可以使这种更改永久有效：echo "kernel.shmmax=2147483648" >> /etc/sysctl.conf
        perror("shmget");
        return -1;
    }

    return shmidtmp;
}
int shmid;           // 共享内存ID
char *shmaddr;       // 共享内存地址
struct Array *cache; // 缓存指针，指向共享内存地址处的内容
// 将LRU缓存映射到进程的地址空间
struct Array *mapLRUCacheToMemory(int shmid)
{

    // 将共享内存映射到进程的地址空间
    if ((cache = (struct Array *)shmat(shmid, NULL, 0)) == (void *)-1)
    {
        perror("shmat");
        return NULL;
    }

    return cache;
}

// 从进程的地址空间中解除对LRU缓存的映射
void unmapLRUCacheFromMemory(struct Array *cache)
{
    // 从进程的地址空间中解除对共享内存的映射
    if (shmdt(cache) == -1)
    {
        perror("shmdt");
    }
}
// 删除共享内存段
void delLRUCacheFromMemory(int shmid)
{
    // 从进程的地址空间中解除对共享内存的映射
    if (shmctl(shmid, IPC_RMID, NULL) == -1)
    {
        perror("shmctl");
    }
}
int is_expired(struct CacheNode *entry)
{
    return time(NULL) > entry->expire_time;
}

void remove_entry(struct Cache *cache, struct CacheNode *entry)
{
    entry->prev->next = entry->next;
    entry->next->prev = entry->prev;
    cache->size--;
}

void add_entry(struct Cache *cache, struct CacheNode *entry)
{
    entry->next = cache->head->next;
    entry->prev = cache->head;
    cache->head->next->prev = entry;
    cache->head->next = entry;
}

struct CacheNode *lookup_entry(struct Cache *cache, const char *key)
{
    struct CacheNode *curr = cache->head->next;
    while (curr != cache->tail)
    {
        if (strcmp(curr->key, key) == 0)
        {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

// 开辟二维空间

struct Array *getcacheprt()
{
    key_t key = ftok("./epoll", SHMID_KEY);
    shmid = initLRUCacheSharedMemory(key);
    cache = mapLRUCacheToMemory(shmid);
    return cache;
}
void OpeningUp(char **S, int row, int col)
{
    int i, j;
    /*利用malloc申请内存*/
    S = (char **)malloc(sizeof(char *) * row); // 分配行
    for (i = 0; i < row; i++)
        S[i] = (char *)malloc(sizeof(char) * col); // 分配列
    for (i = 0; i < row; i++)
    {
        for (j = 0; j < col; j++)
            ;
    }
}
void OpeningUp2(struct ArrayNode **S, int row)
{
    int i, j;
    /*利用malloc申请内存*/
    S = (struct ArrayNode **)malloc(sizeof(struct ArrayNode *) * row); // 分配行
    struct ArrayNode tmp;
    tmp.access_time = time(NULL);
    tmp.expire_time = time(NULL);
    char ttmp[MAX_KEY_LENGTH];
    for (i = 0; i < row; i++)
    {
        S[i] = (struct ArrayNode *)malloc(sizeof(tmp)); // 分配列
        S[i]->access_time = tmp.access_time;
        S[i]->expire_time = tmp.expire_time;
        strcpy(tmp.values, ttmp);
    }

    /*利用malloc()函数分配的内存在程序退出时，不要忘了利用free()函数释放
    要不容易造成内存泄漏*/
    /*释放申请的内存*/
    // for (i = 0; i < row; i++) // 先释放分配的列
    // 	free(S[i]);
    // free(S);				  // 再释放分配的行
    // return 0;
}

struct Array *initcache()
{
    cache = getcacheprt();
    // 缓存空间开辟
    // cache->size=malloc(sizeof(int));
    cache->size = 1;
    OpeningUp(cache->keys, 1000, 150);
    if (cache->keys == NULL)
    {
        free(cache);
        exit(1);
    }
    OpeningUp2(cache->anode, MAX_KEYS);

    if (cache->anode == NULL)
    {
        free(cache);
        exit(1);
    }

    return cache;
}
char *nget(const char *key)
{
    // if(!cache)initcache();
    // struct Array  *cache=getcacheprt();
    char *tmp = array_find(cache, key);
    return tmp;
}
char nset(char *key, char *value, time_t expire_in)
{
    array_push(cache, key, value, expire_in);
}

/**************缓存**************/
struct sockaddr_in getadd(char *ip, char *port)
{
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(atoi(port));
    // dest_addr.sin_addr.s_addr = htons(ip);
    if (inet_pton(AF_INET, ip, &dest_addr.sin_addr) <= 0)
    {
        d("inet_pton");
        // return NULL;
    }
    return dest_addr;
}
// 转发

/*
 * Matched pair for safe_write(). If an EINTR occurs, pick up and try
 * again.
 */

int nread(int fd, char *buffer, int count)
{
    int n = recv(fd, buffer, count, 0);
    return n;
    int len;
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
int nwrite(int fd, char *buffer, int count)
{
    if (strlen(buffer) < 0)
        return 1;
    return send(fd, buffer, count, 0);
    int len;
    int bytestosend;
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
        if ((int)len == bytestosend)
            break;
        buffer += len;
        bytestosend -= len;
    }
    return count;
}

static void usage(const char *proc)
{
    printf("usage:%s[ip][port]\n", proc);
}

static int set_no_block(int fd) // 用来设置非阻塞
{
    // return 0;
    int old_fl = fcntl(fd, F_GETFL);
    if (old_fl < 0)
    {
        perror("perror");
        return -1;
    }
    if (fcntl(fd, F_SETFL, old_fl | O_NONBLOCK))
    {
        perror("fcntl");
        return -1;
    }

    return 0;
}
void disconnect(int fd)
{
    shutdown(fd, SHUT_RDWR);
}
void nclose(client_p clientobj)
{

    if (clientobj->fd)
    {
        disconnect(clientobj->fd);
        close(clientobj->fd);
    }

    if (clientobj->rd)
    {
        disconnect(clientobj->rd);
        close(clientobj->rd);
    }
    if (clientobj)
        free(clientobj);
}

void tout(client_p client_obj)
{
    /* Set timeout for client-side I/O operations */
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    setsockopt(client_obj->fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));
    setsockopt(client_obj->fd, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(timeout));
    setsockopt(client_obj->rd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));
    setsockopt(client_obj->rd, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(timeout));
   // nclose(client_obj);
}
/***********远程信息提取*************/
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
// 提取域名端口信息
char *extract_host(char *buffer, char **host, char **ip, char **port)
{
    regex_t regex;
    int ret;

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

            *host = cutstr(buffer, start + 5, end);

            char **tmp;
            tmp = split_string(*host, ":");
            *host = tmp[0];
            *port = tmp[1];
            *ip = nget(*host);

            if ((*ip == NULL))
            {
                *ip = domain_to_ip(*host);

                // d(*ip);

                //     d("保存ip");
                nset(*host, *ip, exptime);
            }
            return *host; // return the host line
        }
    }
    else if (ret == REG_NOMATCH)
    {
        // d(buffer);
        d("匹配失败");
        return "";
        // exit(0);
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

/*************************/

/* 响应隧道连接请求  */
int send_hand(int client_sock)
{
    char *resp = "HTTP/1.1 200 Connection Established\r\n\r\n";
    int len = strlen(resp);
    char buffer[len + 1];
    strcpy(buffer, resp);

    if (nwrite(client_sock, buffer, len) < 0)
    {
        perror("Send http tunnel response  failed\n");
        return 0;
    }

    return 1;
}
void hand(client_p clientobj)
{
    if (clientobj->istsl == 1)
    {
        if (send_hand(clientobj->fd) > 0)
        {
            clientobj->istsl = 2;
        }
    }
}
void clearstr(char *buffer)
{
    memset(&buffer, 0, sizeof(buffer));
}
char *str_replace(char *orig, char *rep, char *with)
{
    char *result;  // 最终结果
    char *ins;     // 下一个待替换的位置
    char *tmp;     // 临时变量
    int len_rep;   // 替换目标的长度
    int len_with;  // 替换源的长度
    int len_front; // 前段长度（到下一个替换点的距离）
    int count;     // 替换次数

    // 检查原始字符串，待替换字符串和替换字符串是否为空
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL; // 如果待替换字符串为空，则没有替换的必要
    if (!with)
        with = "";
    len_with = strlen(with);

    // 计算替换次数
    ins = orig;
    for (count = 0; (tmp = strstr(ins, rep)); ++count)
    {
        ins = tmp + len_rep;
    }

    // 为结果字符串分配空间
    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    // 实际替换过程
    while (count--)
    {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // 跳过已替换部分
    }
    strcpy(tmp, orig);
    return result;
}
void rewrite_header(client_p clientobj)
{
    char *p = strstr(clientobj->buf, "http://");
    char *p0 = strchr(p, '\0');
    char *p5 = strstr(clientobj->buf, "HTTP/"); /* "HTTP/" 是协议标识 如 "HTTP/1.1" */
    int len = strlen(clientobj->buf);
    if (p)
    {
        char *p1 = strchr(p + 7, '/');
        if (p1 && (p5 > p1))
        {
            // 转换url到 path
            memcpy(p, p1, (int)(p0 - p1));
            int l = len - (p1 - p);
            clientobj->buf[l] = '\0';
        }
        else
        {
            char *p2 = strchr(p, ' '); // GET http://3g.sina.com.cn HTTP/1.1

            // printf("%s\n",p2);
            memcpy(p + 1, p2, (int)(p0 - p2));
            *p = '/'; // url 没有路径使用根
            int l = len - (p2 - p) + 1;
            clientobj->buf[l] = '\0';
        }
    }
    char *find = "Proxy-";
    char *rep = "";
    char *retbuf = str_replace(clientobj->buf, find, rep);
    strcpy(clientobj->buf, retbuf);

    strcat(clientobj->buf, "\n\r\n\r");
}
void forward_data(client_p clientobj)
{

    if (clientobj->ishand != 1)
        return;
    char buffer[_BUF_SIZE_];
    size_t n, lastnum = 0;
    rewrite_header(clientobj);
    // 这里不能去掉阻塞；否则无法发送消息
    //  set_no_block(clientobj->fd);
    //  set_no_block(clientobj->rd);
    n = nwrite(clientobj->rd, clientobj->buf, strlen(clientobj->buf));
    if (n < 0)
        d("ERROR writing to socket");
    while ((n = nread(clientobj->rd, buffer, _BUF_SIZE_)) > 0)
    {

        if (strlen(buffer) > 0)
        {
            nwrite(clientobj->fd, buffer, n);
        }
        // bzero(buffer, _BUF_SIZE_);
        // memset(&buffer, 0, sizeof(buffer));
        clearstr(buffer);
        if (lastnum != 0)
        {
            if (lastnum > n)
            {
                break;
            }
        }
        lastnum = n;
    }
    shutdown(clientobj->rd, SHUT_RDWR);

    shutdown(clientobj->fd, SHUT_RDWR);
}
//字符串填充
void generateString(char *str, size_t length) {  
    // 生成随机字符串  
   
      
    // 如果字符串长度不够，用特定字符填充  
    size_t actualLength = strlen(str);  
    if (actualLength < length) {  
        char fillChar = '*'; // 填充字符  
        for (size_t i = actualLength; i < length; i++) {  
            str[i] = fillChar;  
        }  
    }  
      
    str[length] = '\0'; // 添加字符串终止符  
}  
//前面补20字节{ip，端口}
void incode(client_p clientobj,char *buffer){
char *rdinfo=strcat(clientobj->ip,"-");
rdinfo=strcat(rdinfo,clientobj->port);
rdinfo=generateString(rdinfo,20);
d("dsfsfdsfsdfs")6yyyyyyyyyy;
d(rdinfo);
}
// 发送给服务器
void f_send(client_p clientobj)
{
    char buffer[_BUF_SIZE_];
    int n;
    while ((n = nread(clientobj->fd, buffer, _BUF_SIZE_)) > 0)
    {
        incode(clientobj,buffer);
        nwrite(clientobj->rd, buffer, n);
    }
    shutdown(clientobj->rd, SHUT_RDWR);
    shutdown(clientobj->fd, SHUT_RDWR);
}

// 发回给客户端
void f_back(client_p clientobj)
{
    char buffer[_BUF_SIZE_];
    int n;
    while ((n = nread(clientobj->rd, buffer, _BUF_SIZE_)) > 0)
    {
        nwrite(clientobj->fd, buffer, n);
    }

    shutdown(clientobj->rd, SHUT_RDWR);

    shutdown(clientobj->fd, SHUT_RDWR);
}
int connect_remote(client_p clientobj)
{
    if (clientobj->rd > 0)
    {
        return clientobj->rd;
    }
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    // struct hostent *server;
    // char buffer[_BUF_SIZE_];
    // const char* host_name = "www.baidu.com";
    // const char* http_request = "GET / HTTP/1.1\r\nHost: www.baidu.com\r\nConnection: close\r\n\r\n";
    // serv_addr=getadd(clientobj->ip,clientobj->port);
    portno = atoi(clientobj->port);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        d("ERROR opening socket");

    serv_addr.sin_family = AF_INET;

    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = inet_addr(clientobj->ip);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        d("ERROR connecting");
        // nclose(clientobj);
        return 0;
    }

    return sockfd;
}
// int转字符串
void tochar(int data, char *ret)
{
    static char tmp[12]; // 使用静态变量，确保其在函数返回后仍然存在
    snprintf(tmp, sizeof(tmp), "%d", data);
    strcpy(ret, tmp);
    // return tmp;
}
void toint(char data, int *ret)
{
    ret = atoi(data);
}
// 获取客户端信息ip：端口
char *getclientinfo(client_p clientobj)
{
    // char *ipinfo = malloc(INET_ADDRSTRLEN + 7);
    char *ipinfo = malloc(100);
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    getpeername(clientobj->fd, (struct sockaddr *)&client_addr, &client_addr_len);
    char client_ip[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN) != NULL)
    {
        // printf("Client IP address: %s\n", client_ip);
    }
    // printf("Client port number: %d\n", ntohs(client_addr.sin_port));
    strcat(ipinfo, client_ip);
    strcat(ipinfo, ":");
    // char port[6]; // for max uint16_t number (65535) and null-terminator
    // snprintf(port, sizeof(port), "%d", ntohs(client_addr.sin_port));
    char port[6];
    tochar(ntohs(client_addr.sin_port), port);

    strcat(ipinfo, port);
    return ipinfo;
}
int init_remote(client_p clientobj)
{
    if (clientobj->rd > 0)
    {
        return clientobj->rd;
    }
    // 提取远程信息
    char *destdomain, *destip, *destport;
    clientobj->ishand = 0;
    char *ipinfo = getclientinfo(clientobj);
    if (clientobj->isinit == 1)
    {
        d("已经初始化了");
        return clientobj->ishand;
    }

    char buftmp[_BUF_SIZE_];
    strcpy(buftmp, clientobj->buf);

    clientobj->isinit = 1; // 只初始化一次；无论是否匹配host地址
    char *host = extract_host(buftmp, &destdomain, &destip, &destport);

    d(destdomain);
    // d(destip);
    // d(destport);
    if (*host == NULL)
    {
        d("没匹配到目标,尝试端口缓存恢复");

        // dd(atoi(nget(ipinfo)));
        // close(clientobj->fd);
        // clientobj->rd = atoi(nget(ipinfo));
        return 0;
        // break;
    }
    else
    {
        clientobj->host = host;
        clientobj->ip = destip;
        clientobj->port = destport;
        // 建立远程链接
    }

    clientobj->istsl = checkTsl(buftmp);

    clientobj->rd = connect_remote(clientobj);
    d("链接2");
    char rd[100];
    tochar(clientobj->rd, rd);
    // nset(ipinfo, rd, 60);
    // 这里缓存下信息

    if (clientobj->rd > 0)
    {
        clientobj->ishand = 1;
    }
    else
    {
        clientobj->ishand = 0;
    }
    return clientobj->ishand;
}

static int start(char *port, char *ip)
{
    struct sockaddr_in local;
    local = getadd(ip, port);
    // assert(ip);
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("socket");
        exit(1);
    }

    int opt = 1; // 设置为接口复用
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(sock, (struct sockaddr *)&local, sizeof(local)) < 0)
    {
        perror("bind");
        exit(2);
    }

    if (listen(sock, _BACKLOG_) < 0)
    {
        perror("listen");
        exit(3);
    }
    return sock;
}
/******************sock5****************************/
static const char* auth_user="admin";
static const char* auth_pass="1234";
enum socksstate {
	SS_1_CONNECTED,
	SS_2_NEED_AUTH, /* skipped if NO_AUTH method supported */
	SS_3_AUTHED,
};
enum authmethod {
	AM_NO_AUTH = 0,
	AM_GSSAPI = 1,
	AM_USERNAME = 2,
	AM_INVALID = 0xFF
};
enum errorcode {
	EC_SUCCESS = 0,
	EC_GENERAL_FAILURE = 1,
	EC_NOT_ALLOWED = 2,
	EC_NET_UNREACHABLE = 3,
	EC_HOST_UNREACHABLE = 4,
	EC_CONN_REFUSED = 5,
	EC_TTL_EXPIRED = 6,
	EC_COMMAND_NOT_SUPPORTED = 7,
	EC_ADDRESSTYPE_NOT_SUPPORTED = 8,
};
static void send_auth_response(int fd, int version, enum authmethod meth) {
	unsigned char buf[2];
	buf[0] = version;
	buf[1] = meth;
	nwrite(fd, buf, 2);
}
//效验sock5类型
static enum authmethod check_auth_method(unsigned char *buf, size_t n) {
	if(buf[0] != 5) return AM_INVALID;
	size_t idx = 1;
	if(idx >= n ) return AM_INVALID;
   
	int n_methods = buf[idx];
	idx++;
   
	while(idx <= n && n_methods > 0) {
      
       
		if(buf[idx] == AM_NO_AUTH) {
			if(!auth_user) return AM_NO_AUTH;
		} else if(buf[idx] == AM_USERNAME) {
			if(auth_user) return AM_USERNAME;
		}
		idx++;
		n_methods--;
	}
    
    
	return AM_INVALID;
}
//检查密码
static enum errorcode check_credentials(unsigned char* buf, size_t n) {
	if(n < 5) return EC_GENERAL_FAILURE;
	if(buf[0] != 1) return EC_GENERAL_FAILURE;
	unsigned ulen, plen;
	ulen=buf[1];
	if(n < 2 + ulen + 2) return EC_GENERAL_FAILURE;
	plen=buf[2+ulen];
	if(n < 2 + ulen + 1 + plen) return EC_GENERAL_FAILURE;
	char user[256], pass[256];
	memcpy(user, buf+2, ulen);
	memcpy(pass, buf+2+ulen+1, plen);
	user[ulen] = 0;
	pass[plen] = 0;
	if(!strcmp(user, auth_user) && !strcmp(pass, auth_pass)) return EC_SUCCESS;
	return EC_NOT_ALLOWED;
}
/******************sock5****************************/
void sock5_hand(client_p clientobj)
{
    //1、 判断版本
    //判断是否需要验证
    //2、返回消息
    //发送密码
    //返回消息
    //3、转发
    clientobj->issock = 0;
    if (clientobj->buf[0] != 5)
    {
        return;
    }
    enum socksstate state;
    clientobj->issock = 1;
    enum authmethod am;
    ssize_t n;
    int ret;
   
    am=check_auth_method(clientobj->buf,strlen(clientobj->buf));
     dd(am);
    if(am == AM_NO_AUTH) state= SS_3_AUTHED;
    else if (am == AM_USERNAME) state= SS_2_NEED_AUTH;
    send_auth_response(clientobj->fd, 5, am);
     d("1111");
    
    if(am == AM_INVALID) return;
    unsigned char buf[1024];
    
    while((n = recv(clientobj->fd, buf, sizeof buf, 0)) > 0) {
		switch(state) {
			// case SS_1_CONNECTED:
			// 	am = check_auth_method(buf, n, &t->client);
			// 	if(am == AM_NO_AUTH) t->state = SS_3_AUTHED;
			// 	else if (am == AM_USERNAME) t->state = SS_2_NEED_AUTH;
			// 	send_auth_response(t->client.fd, 5, am);
			// 	if(am == AM_INVALID) goto breakloop;
			// 	break;
			case SS_2_NEED_AUTH:
            d("dfs");
				ret = check_credentials(buf, n);
				send_auth_response(clientobj->fd, 1, ret);
				if(ret != EC_SUCCESS)
					return;
				state = SS_3_AUTHED;
                
				break;
			case SS_3_AUTHED:
            d(buf);
				// ret = connect_socks_target(buf, n, &t->client);
				// if(ret < 0) {
				// 	send_error(t->client.fd, ret*-1);
				// 	goto breakloop;
				// }
				// remotefd = ret;
				// send_error(t->client.fd, EC_SUCCESS);
				// copyloop(t->client.fd, remotefd);
				// goto breakloop;

		}
	}
}

// 工作进程
void workclient(client_p clientobj, epoll_data_t evdata)
{
     sock5_hand(clientobj);
    if (clientobj->issock != 1)
    {
        /******这里先检查buf信息；然后连接远程******/
        // inittime();
        if (init_remote(clientobj) <= 0)
        {

            // clientobj = (client_p)evdata.ptr;
            d("尝试从epoll重载客户端对象");
            // d(clientobj->host);
            nclose(clientobj);
            return;
        }

        d("1");
         tout(clientobj);
        pid_t pid3 = fork();
        if (pid3 == 0)
        {
            if (clientobj->istsl == 0)
            {
                // 如果是http就直接由这里进行消息转发处理
                forward_data(clientobj);
                exit(1);
            }
            // 这里是https把消息转发给服务器
            f_send(clientobj);
            exit(0);
        }
        // if (pid3 > 0)
        // {
        //     int status;
        //     waitpid(pid3, &status, 0); // 等待子进程结束
        // }
        d("2");
        pid_t pid4 = fork();
        if (pid4 == 0)
        {
            // 这里是https流量建立握手
            hand(clientobj);
            // 消息回显
            f_back(clientobj);
            exit(1);
        }
        d("3");
        // if (pid4 > 0)
        // {
        //     int status;
        //     waitpid(pid4, &status, 0); // 等待子进程结束
        // }
        d("结束");
        //  nclose(clientobj);
    }
}

static int epoll_server(int listen_sock)
{
    int epoll_fd = epoll_create(256); // 生成一个专用的epoll文件描述符
    if (epoll_fd < 0)
    {
        perror("epoll_create");
        exit(1);
    }
    set_no_block(listen_sock);        // 设置监听套接字为非阻塞
    struct epoll_event ev;            // 用于注册事件
    struct epoll_event ret_ev[_MAX_]; // 数组用于回传要处理的事件
    int ret_num = _MAX_;
    int read_num = -1;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = listen_sock;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_sock, &ev) < 0) // 用于控制某个文件描述符上的事件（注册，修改，删除）
    {
        perror("epoll_ctl");
        return -2;
    }

    int done = 0;
    int i = 0;
    int timeout = 5000;
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    while (!done)
    {
        switch (read_num = epoll_wait(epoll_fd, ret_ev, ret_num, timeout)) // 用于轮寻I/O事件的发生
        {
        case 0:
            //  printf("time out\n");
            break;
        case -1:
            perror("epoll");
            exit(2);
        default:
        {
            for (i = 0; i < read_num; ++i)
            {
                if (ret_ev[i].data.fd == listen_sock && (ret_ev[i].events & EPOLLIN))
                {

                    int fd = ret_ev[i].data.fd;
                    int new_sock = accept(fd, (struct sockaddr *)&client, &len);
                    if (new_sock < 0)
                    {
                        perror("accept");
                        continue;
                    }

                    // set_no_block(new_sock); // 设置套接字为非阻塞
                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = new_sock;
                    // close(fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_sock, &ev);
                    // printf("get a new client...\n");
                }
                else // normal sock
                {

                    if (ret_ev[i].events & EPOLLIN)
                    {

                        pid = fork();
                        if (pid == 0)
                        {
                            int fd = ret_ev[i].data.fd;
                            //  client_p mem = (client_p)ret_ev[i].data.ptr;
                            client_p mem = (client_p)malloc(sizeof(client_t));
                            if (!mem)
                            {
                                perror("malloc");
                                continue;
                            }
                            mem->fd = fd;
                            memset(mem->buf, '\0', sizeof(mem->buf));
                            ssize_t _s = nread(mem->fd, mem->buf, sizeof(mem->buf) - 1); // 一次读完
                            if (_s > 0)
                            {
                                mem->buf[_s - 1] = '\0';

                                // 开进程肯定不阻塞
                                workclient(mem, ret_ev[i].data);
                                /********这里是http代理********/
                                ev.events = EPOLLOUT | EPOLLET;
                                ev.data.ptr = mem;
                                epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev);

                                exit(0);
                            }
                            else if (_s == 0)
                            {
                                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                                if (mem)
                                {
                                    // d("client close...\n");
                                    // nclose(mem);
                                }
                            }
                            else
                            {
                                continue;
                            }
                            exit(0);
                        }
                        if (pid > 0)
                        {
                            // 父进程
                            int status;
                            // d("进程回收2");
                            //   waitpid(pid3, &status, 0);
                            //   waitpid(pid4, &status, 0);
                            waitpid(pid, &status, 0); // 等待子进程结束

                            //  // 等待子进程结束
                            //  // 等待子进程结束
                        }
                    }
                    else if (ret_ev[i].events & EPOLLOUT) // 写事件准备就绪
                    {
                        // client_p mem = (client_p)ret_ev[i].data.ptr;
                        // char *msg = "http/1.0 200 ok\r\n\r\nhello bit\r\n";
                        // int fd = mem->fd;

                        // write(fd, msg, strlen(msg));
                        // close(fd);
                        // epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &ev); // 写完服务端直接退出
                        // free(mem);
                    }
                    else
                    {
                        //....
                    }
                }
            }
        }
        break;
        }
    }
}
int nstart()
{
    inittime();

    initcache();

    // if (argc != 3)
    // {
    //     usage(argv[0]);
    //     return 1;
    // }

    // int port = atoi(SERVER_PORT);
    //  char *ip=argv[1];
    char *ip = "0.0.0.0";
    char *port = SERVER_PORT;
    printf("Listening on port %s...\n", SERVER_PORT);
    int listen_sock = start(port, ip);
    epoll_server(listen_sock);
    close(listen_sock);
    return 0;
}
/* 处理僵尸进程 */

void main(int argc, char *argv[])
{
    // signal(SIGCHLD, sigchld_handler); // 防止子进程变成僵尸进程
    if (ndaemon)
    {

        if ((pid2 = fork()) == 0)
        {
            // setsid();
            // chdir("/");
            // umask(0);
            nstart();
        }
        else if (pid2 > 0)
        {
            m_pid = pid2;
            int status;
            d("进程回收");
            waitpid(pid2, &status, 0); // 等待子进程结束
            printf("mporxy pid is: [%d]\n", pid);
        }
        else
        {
            d("Cannot daemonize\n");
            exit(pid2);
        }
    }
    else
    {
        nstart();
    }
}