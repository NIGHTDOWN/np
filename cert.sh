#!/bin/bash  
  
# 生成私钥文件  
openssl genpkey -algorithm RSA -out private.key  
  
# 创建证书请求  
openssl req -new -key private.key -out certificate.csr  
  
# 自签名证书  
openssl x509 -req -days 365 -in certificate.csr -signkey private.key -out certificate.crt
# 脚本将按照步骤生成私钥文件（private.key）、证书请求文件（certificate.csr）和自签名证书文件（certificate.crt）。

# 请注意，自签名证书在安全性和可信度方面可能不如由权威CA颁发的证书。因此，在生产环境中，建议使用由权威CA颁发的通用证书。