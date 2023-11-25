要补充加载证书和私钥，你可以按照以下步骤进行操作：

创建SSL上下文：

c
SSL_CTX *ctx = SSL_CTX_new(SSLv23_server_method());
加载证书文件：

c
if (SSL_CTX_load_verify_locations(ctx, "server.crt", NULL) != 1) {  
    // 处理证书加载失败的情况  
}
加载私钥文件：

c
if (SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM) != 1) {  
    // 处理私钥加载失败的情况  
}
在加载私钥文件时，你需要指定正确的文件类型。这里我们使用了SSL_FILETYPE_PEM表示PEM格式的私钥文件。

确认私钥和证书匹配：

c
if (SSL_CTX_check_private_key(ctx) != 1) {  
    // 处理私钥和证书不匹配的情况  
}
以上步骤完成后，你就可以使用SSL_CTX对象创建SSL连接了。请确保在创建SSL连接之前完成以上步骤，以确保证书和私钥的加载和验证。