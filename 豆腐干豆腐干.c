#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/rsa.h>

// 生成RSA密钥对
RSA *generate_key_pair()
{
    RSA *rsa = RSA_new();
    BIGNUM *bne = BN_new();
    int ret = BN_set_word(bne, RSA_F4);
    if (ret != 1)
    {
        RSA_free(rsa);
        BN_free(bne);
        return NULL;
    }
    ret = RSA_generate_key_ex(rsa, 2048, bne, NULL);
    if (ret != 1)
    {
        RSA_free(rsa);
        BN_free(bne);
        return NULL;
    }
    return rsa;
}

// 实现decrypt函数，用于解密数据
unsigned char decrypt(unsigned char c, RSA *rsa)
{
    int result = RSA_private_decrypt(c, NULL, rsa, RSA_PKCS1_PADDING);
    if (result == -1)
    {
        return 0; // 解密失败
    }
    else
    {
        return result; // 解密成功，返回解密后的数据
    }
}
char *encrypt_to_string(char *msg, RSA *rsa) {  
    int len = RSA_size(rsa);  
    char *result = (char *)malloc(len + 1); // 分配足够的空间来存储加密后的数据  
    int i = 0;  
    for (; msg[i] != '\0'; i++) {  
        result[i] = encrypt(msg[i], rsa); // 将每个字符加密并存储到结果中  
    }  
    result[i] = '\0'; // 添加字符串结束符  
    return result;  
}

int main()
{
    int client_fd, server_fd;
    char buf[256];
    RSA *rsa_pubkey, *rsa;
    int verify, choice;
    unsigned char session_key[256 / 8];

    // 生成RSA密钥对
    rsa = generate_key_pair();
    if (rsa == NULL)
    {
        printf("Failed to generate RSA key pair.\n");
        return EXIT_FAILURE;
    }

    // 连接服务器
    printf("Connecting to server...\n");
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345); // 服务器端口号
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }
    printf("Connected to server.\n");

    // 从服务器获取公钥并验证
    printf("Getting server's public key...\n");
    if (recv(client_fd, buf, 256, 0) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    rsa_pubkey = PEM_read_RSAKey(NULL, NULL, buf);
    if (rsa_pubkey == NULL)
    {
        printf("Invalid public key from server.\n");
        exit(EXIT_FAILURE);
    }
    verify = RSA_verify(NID_sha1, (unsigned char *)"hello world".data(), (unsigned char *)"hello world".data(), RSA_size(rsa_pubkey), RSA_public_e(rsa_pubkey));
    if (verify == 0)
    {
        printf("Failed to verify server's public key.\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Verified server's public key.\n");
    }

    // 与服务器协商生成对话密钥
    printf("Generating session key with server...\n");
    choice = random() % 2; // 随机选择加密或解密密钥
    if (choice == 0)
    { // 加密密钥，发送给服务器
        for (int i = 0; i < sizeof(session_key); i++)
        {
            session_key[i] = random() % 256; // 随机生成加密密钥
        }
        if (send(client_fd, session_key, sizeof(session_key), 0) == -1)
        { // 发送加密密钥给服务器
            perror("send");
            exit(EXIT_FAILURE);
        }
        printf("Sent session key to server.\n");
    }
    else
    { // 解密密钥，从服务器获取并验证
        if (recv(client_fd, session_key, sizeof(session_key), 0) == -1)
        { // 从服务器获取加密密钥
            perror("recv");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < sizeof(session_key); i++)
        {
            session_key[i] = decrypt(session_key[i], rsa); // 解密服务器发送的加密密钥
        }
        if (strcmp(buf, "hello world") != 0)
        { // 验证解密后的密钥是否正确
            printf("Failed to verify server's session key.\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("Verified server's session key.\n");
        }
    } // 使用对话密钥进行加密通信
    printf("Using session key for encryption communication...\n");
    const char *msg = "Hello, server!";
    int len = strlen(msg) + 1;
    unsigned char encrypted[len];
    for (int i = 0; i < len; i++)
    {
        encrypted[i] = encrypt(msg[i], rsa); // 使用对话密钥加密消息
    }
    if (send(client_fd, encrypted, len, 0) == -1)
    { // 发送加密后的消息给服务器
        perror("send");
        exit(EXIT_FAILURE);
    }
    printf("Sent encrypted message to server.\n");
    if (recv(client_fd, buf, MAX_BUF_SIZE, 0) == -1)
    { // 接收服务器回复的消息
        perror("recv");
        exit(EXIT_FAILURE);
    }
    printf("Received message from server: %s\n", buf);

    // 关闭连接
    printf("Closing connection...\n");
    close(client_fd);
    exit(EXIT_SUCCESS);
}