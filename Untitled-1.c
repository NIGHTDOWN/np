#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

// 生成RSA密钥对
RSA *generate_rsa_keypair(int key_size) {
    RSA *rsa = RSA_generate_key(key_size, RSA_F4, NULL, NULL);
    if (!rsa) {
        fprintf(stderr, "Failed to generate RSA key pair: %s
", ERR_reason_error_string(ERR_get_error()));
        exit(1);
    }
    return rsa;
}

// 生成证书请求
BIO *generate_certificate_request(RSA *rsa) {
    X500Name *subject_name = X500Name_new();
    X509_NAME_add_entry_by_txt(subject_name, "CN", "localhost", -1, X509_NAME_ENTRY_FLAG_NS, NULL, NULL);
    X509_REQ *req = X509_REQ_new();
    X509_REQ_set_version(req, X509_VR_ANY);
    X509_REQ_set_pubkey(req, rsa);
    X509_REQ_set_subject_name(req, subject_name);
    X509_REQ_sign(req, rsa, EVP_sha256(), NULL);
    BIO *bio = BIO_new(BIO_s_mem());
    if (!bio) {
        fprintf(stderr, "Failed to create BIO object: %s
", ERR_reason_error_string(ERR_get_error()));
        exit(1);
    }
    X509_REQ_to_bio(bio, req);
    return bio;
}

// 生成证书
BIO *generate_certificate(RSA *rsa, BIO *req_bio) {
    X509 *cert = X509_new();
    X509_set_version(cert, X509_VR_ANY);
    X509_set_issuer_name(cert, X509_get_issuer_name(req_bio));
    X509_set_subject_name(cert, X509_get_subject_name(req_bio));
    X509_set_pubkey(cert, X509_get_pubkey(req_bio));
    unsigned char cert_signature[EVP_MAX_MD_SIZE];
    unsigned int cert_signature_len = 0;
    if (!X509_sign(cert, rsa, cert_signature, &cert_signature_len)) {
        fprintf(stderr, "Failed to sign certificate: %s
", ERR_reason_error_string(ERR_get_error()));
        exit(1);
    }
    BIO *bio = BIO_new(BIO_s_mem());
    if (!bio) {
        fprintf(stderr, "Failed to create BIO object: %s
", ERR_reason_error_string(ERR_get_error()));
        exit(1);
    }
    BIO_write(bio, cert_signature, cert_signature_len);
    return bio;
}

int main() {
    // 生成RSA密钥对
    RSA *rsa = generate_rsa_keypair(2048);
    if (!rsa) {
        return 1;
    }

    // 生成证书请求
    BIO *req_bio = generate_certificate_request(rsa);
    if (!req_bio) {
        return 1;
    }

    // 生成证书
    BIO *cert_bio = generate_certificate(rsa, req_bio);
    if (!cert_bio) {
        return 1;
    }

    // 将证书和证书请求输出到文件
    FILE *cert_file = fopen("client.crt", "w");
    if (!cert_file) {
        fprintf(stderr, "Failed to open certificate file: %s
", strerror(errno));
        return 1;
    }
    fwrite(cert_bio->ptr, cert_bio->length, 1, cert_file);
    fclose(cert_file);
    FILE *req_file = fopen("client.csr", "w");
    if (!req_file) {
        fprintf(stderr, "Failed to open certificate request file: %s
", strerror(errno));
        return 1;
    }
    fwrite(req_bio->ptr, req_bio->length, 1, req_file);
    fclose(req_file);

    // 将客户端密钥输出到文件
    FILE *key_file = fopen("client.key", "w");
    if (!key_file) {
        fprintf(stderr, "Failed to open client key file: %s
", strerror(errno));
        return 1;
    }
    RSA_print_fp(key_file, rsa, 0);
    fclose(key_file);

    // 清除库函数中的资源
    RSA_free(rsa);
    X500Name_free(X500_NAME_get(X509_get_issuer_name(cert_bio)));
    X509_NAME_free(X509_get_subject_name(cert_bio));
    X509_free(cert_bio);
    X509_REQ_free(req_bio);
    BIO_free(cert_bio);
    BIO_free(req_bio);
    return 0;
}