# pkill -9 epoll
# rm -rf *.c
# clear
#  wget 10.172.10.161/poll.c
# gcc -o epoll poll.c  -std=c99 -lssl -lcrypto -D_SVID_SOURCE
gcc -o object.o -c poll.c -o object.o -std=c99 -lssl -lcrypto -D_SVID_SOURCE
gcc -shared -o libshared.so object.o





