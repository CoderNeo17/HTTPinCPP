#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<cstring>

int main(){
    int socketFD = socket(AF_INET,SOCK_STREAM,0);

    sockaddr_in _addr;
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(8080);
    _addr.sin_addr.s_addr = INADDR_ANY;
    // const char *ip = "172.29.176.1";
    // inet_pton(AF_INET,ip,&_addr.sin_addr);

    if(bind(socketFD,(sockaddr *)&_addr,sizeof(_addr)) == -1){
        std::cout<<"Binding of Socket to Address failed!\n";
    }

    if(listen(socketFD,2) == -1){
        std::cout<<"Socket couldn't be setup to listen\n";
    }
    int client = accept(socketFD,nullptr,nullptr);

    char in_buffer[1024] = {0};
    recv(client,in_buffer,1024,0);
    std::cout<<"recieved the message: "<<in_buffer<<"\n";
    
    close(socketFD);

    return 0;
}