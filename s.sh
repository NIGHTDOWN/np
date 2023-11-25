pkill -9 epoll
rm -rf *.c
clear
 wget 10.172.10.161/poll.c
gcc -o epoll poll.c  -std=c99 -lssl -lcrypto -D_SVID_SOURCE

./epoll


# curl -x socks5://10.172.10.5:8088 https://huyintong.com
# 在上面的命令中，将proxy_address替换为SOCKS5代理服务器的实际地址，port替换为代理服务器的端口号，https://example.com替换为您要请求的目标URL。

# 如果您的SOCKS5代理服务器需要进行身份验证，可以使用-U或--proxy-user选项来指定用户名和密码。示例如下：

# shell
# curl -x socks5://admin:1234@10.172.10.5:8088 https://huyintong.com

# curl --socks5  38.0.0.0:1080 --proxy-user lumexxn:sexxcs http://ip.sb
