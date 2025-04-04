#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<cstring>

int main(){
    int clientFD = socket(AF_INET,SOCK_STREAM,0);

    sockaddr_in _addr;
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(8080);
    const char *ip = "172.29.182.114";
    inet_pton(AF_INET,ip,&_addr.sin_addr);

    connect(clientFD,(struct sockaddr*)&_addr,sizeof(_addr));

    char send_buffer[1024] = {"Hello World!"};
    send(clientFD,send_buffer,1024,0);

    close(clientFD);

    return 0;
}