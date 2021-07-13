#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char *message);

int main()
{
        int sock;
        struct sockaddr_in serv_addr;
        char message[1024];
        int str_len;
        int port = 9100;
        char ip[20]="192.168.100.3";

        sock=socket(PF_INET, SOCK_STREAM, 0);
        if(sock == -1)
                error_handling("socket() error");

        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family=AF_INET;
        serv_addr.sin_addr.s_addr=inet_addr(ip);
        serv_addr.sin_port=htons(port);

        if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1) 
                error_handling("connect() error!");

        memset(&message, 0, sizeof(message));
        str_len=read(sock, message, sizeof(message)-1);
        if(str_len==-1)
                error_handling("read() error!");

        printf("Message from server: %s \n", message);  

        while(1){
                char str[1024];
                memset(&str, 0, sizeof(str));
                scanf("%s", str);

                if(strcmp(str, "Q") == 0 || strcmp(str, "q") == 0)
                        return 0;

                write(sock, str, sizeof(str));

                char msg2[1024];
                memset(&msg2, 0, sizeof(msg2));
                read(sock, msg2, sizeof(msg2));
                printf("MSG: %s\n", msg2);
        }

        close(sock);
        return 0;
}

void error_handling(char *message)
{
        fputs(message, stderr);
        fputc('\n', stderr);
        exit(1);
}