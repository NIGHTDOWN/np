Client side :
          err = connect(fd, addr, addrlen);

          if ( err == -1 && errno == EINPROGRESS )
          {
              // check if this is a true error,
              // or wait until connect times out
              fd_set fdset;
              FD_ZERO(&fdset);
              FD_SET(sfd, &fdset);
              timeval tv =  {F_sockwaitconnect, 0};                                                   TEMP_FAILURE_RETRY(err = select(fd + 1,\
                                NULL,\
                                &fdset,\
                                NULL,\
                                &tv));

                  // what happened?
                  if ( err == 1 )
                  {
                      connect was successful
                  }
                  else
                     return 0;

            const SSL_METHOD *method;
            SSL_CTX *cctx;
            SSL *cssl;

            FILE *fp;
            fp = stdout;

            ERR_clear_error();

            method = TLSv1_client_method();
            cctx = SSL_CTX_new(method);

            if ( cctx == NULL )
            {
                ERR_print_errors_fp(stdout);
                return 0;
            }

         SSL_CTX_set_verify(cctx, SSL_VERIFY_PEER, NULL);
         SSL_CTX_set_verify_depth(cctx, 4);
         if (SSL_CTX_load_verify_locations(cctx, "mycert.pem", NULL) == 0)
              return 0;

         SSL_CTX_set_options(cctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_COMPRESSION);

         ERR_clear_error();

         cssl = SSL_new(cctx);   /* create new SSL connection state */
         SSL_set_fd(cssl, fd);   * attach the socket descriptor */

         ERR_clear_error();
         int rconnect = SSL_ERROR_WANT_READ;
         while ( rconnect == SSL_ERROR_WANT_READ || rconnect == SSL_ERROR_WANT_WRITE )
         {
             char *buf = (char *) malloc(124);
             ERR_error_string(SSL_get_error(cssl, rconnect), buf);
             ERR_clear_error();
             if ( rconnect == SSL_ERROR_WANT_READ ) {
                 int err = 0;
                 fd_set fdset;
                 FD_ZERO(&fdset);
                 FD_SET(fd, &fdset);
                 timeval tv =  {F_sockwaitconnect, 0};
                 TEMP_FAILURE_RETRY(err1 = select(fd + 1,\
                                                  &fdset,\
                                                  NULL,\
                                                    NULL,\
                                                    &tv));
                  // what happened?
                  if ( err == 1 )
                  {
                      rconnect = SSL_connect(cssl);
                   }
               }
            }
            X509 *cert;
            cert = SSL_get_peer_certificate(cssl);
            char line[2000+1];
            if ( cert != NULL )
            {
              X509_NAME_oneline(X509_get_subject_name(cert), line, MAX_SIZE);
              X509_NAME_oneline(X509_get_issuer_name(cert), line1, MAX_SIZE);
              X509_free(cert);
            }

            ERR_clear_error();
            r = SSL_write(cssl, buffer, len);
            < check error >


Server side :
    int res = pselect(max_fd + 1,   // host socket file descriptor
                      &fd_setw,      // set of ds wait 4 incoming data
                      NULL,          // no write operations
                      NULL,          // no exception operations
                      &tm,           // how much time to wait
                      &sig_set);     // block all signals
     if ( event on listening socket )
     {
          client = accept(sfd, &peer, &peerl);
     }
     else       // incoming data to receive on existing connection
     {
          SSL *ssl;
          FILE *fp = stdout;
          if ( !ctx )
          {
              return 0;
          }

           ERR_clear_error();

           ssl = SSL_new(ctx);
           SSL_set_fd(ssl, soc);

           int ret = SSL_accept(ssl);
           while (ret <= 0) {
              ERR_print_errors_fp(fp);
              char *buf = (char *) malloc(124);
              ERR_error_string(SSL_get_error(ssl, ret), buf);
              ERR_clear_error();
              ret = SSL_accept(ssl);
            }

            X509 *cert;
            cert = SSL_get_peer_certificate(ssl);
            char line[2000+1];
            if ( cert != NULL )
            {
               X509_NAME_oneline(X509_get_subject_name(cert), line, MAX_SIZE);
               X509_NAME_oneline(X509_get_issuer_name(cert), line1, MAX_SIZE);
               X509_free(cert);
             }
             // get data and analyze result
             int rc = 0;
             bool recv_called = false;
             rc = SSL_read(ssl, buffer, len);
             < check error >
     }

在执行上述所有操作之前，服务器将在非阻塞套接字上打开，绑定(bind)并侦听任何新的传入客户端连接。

当我运行上面的代码时，客户端执行connect()，而服务器执行accept()。
服务器现在正在pselect()等待任何fd准备接收数据。
另一方面，客户端位于SSL_connect()处，并不断收到SSL_ERROR_WANT_READ错误。 select()返回套接字已准备好读取。
我的猜测是客户端正在等待握手的SSL_accept()部分？我不知道为什么服务器正在等待pselect()。 SSL_accept()周围的代码是错误的，即，它循环并且不查找WANT_READ和WANT_WRITE错误，但我在代码中没有提到这一点。