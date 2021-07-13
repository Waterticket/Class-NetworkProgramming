#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

typedef struct
{
    int id;
    char name[30];
    int cmd_code;
} FILE_REQ;

typedef struct
{
    int cmd_code;
    char msg[1000];
} FILE_RES;

void itoa(int i, char *st)
{
    sprintf(st, "%d", i);
    return;
}


void error_handling(char *message);

int main()
{
        int sock;
        struct sockaddr_in serv_addr;
        char message[BUF_SIZE];
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
        str_len=read(sock, message, sizeof(message));
        if(str_len==-1)
                error_handling("read() error!");

        printf("Message from server: %s \n", message);
        int offset = 0;
        char id[5], cmd_code[5];
        memset(&id, 0, sizeof(id));
        memset(&cmd_code, 0, sizeof(cmd_code));

        FILE_REQ req;
        req.id = 3307;
        strcpy(req.name, "Waterticket");
        req.cmd_code = 1030;

        printf(" [SET] %d %s %d\n", req.id, req.name, req.cmd_code);

        memset(&message, 0, sizeof(message));

        // msg에 4바이트 만큼 id값을 넣기
        itoa(req.id, id);
        memcpy(&message, &id, sizeof(int));
        offset = sizeof(int); // 4 bytes

        // msg에 30바이트(sizeof req.name)만큼 name 넣기
        memcpy(&message[offset], &req.name, sizeof(req.name));
        offset += sizeof(req.name); // 30 bytes

        // msg에 4바이트만큼 cmd_code 넣기
        itoa(req.cmd_code, cmd_code);
        memcpy(&message[offset], &cmd_code, sizeof(int));
        // file info request

        int req_len = write(sock, message, BUF_SIZE);
        printf(" [SEND] (%d) %s\n", req_len, message);

        memset(&message, 0, sizeof(message));
        int res_len = read(sock, message, BUF_SIZE);
        printf(" [RECIEVE] (%d): %s\n", res_len, message);

        FILE_RES res;
        memcpy(&id, &message, sizeof(int)); // rmsg에 맨 앞부터 sizeof(int) (4byte) 만큼을 잘라서 id에 넣기.
        offset = sizeof(int); // offset 재설정 (위에서 사용한 offset을 다시 초기화)
        res.cmd_code = atoi(id); // res.id에 int로 변환한 id값 넣기

        memcpy(&res.msg, &message[offset], sizeof(res.msg)); // rmsg[offset] 부터 sizeof(rheader.name) (30byte)를 잘라서 rheader.name에 넣기.

        printf(" [DISASSEMBLE] %d %s\n", res.cmd_code, res.msg);

        char file_name[BUF_SIZE];
        memset(&file_name, 0, sizeof(file_name));
        memcpy(&file_name, &res.msg, sizeof(res.msg));

        if(res.cmd_code != 200)
                error_handling("NOT 200");

        req.cmd_code = 2030;

        // msg에 4바이트 만큼 id값을 넣기
        itoa(req.id, id);
        memcpy(&message, &id, sizeof(int));
        offset = sizeof(int); // 4 bytes

        // msg에 30바이트(sizeof req.name)만큼 name 넣기
        memcpy(&message[offset], &req.name, sizeof(req.name));
        offset += sizeof(req.name); // 30 bytes

        // msg에 4바이트만큼 cmd_code 넣기
        itoa(req.cmd_code, cmd_code);
        memcpy(&message[offset], &cmd_code, sizeof(int));

        // file info request
        req_len = write(sock, message, BUF_SIZE);
        printf(" [SEND] (%d) %s\n", req_len, message);

        memset(&message, 0, sizeof(message));
        res_len = read(sock, message, BUF_SIZE);
        printf(" [RECIEVE] (%d): %s\n", res_len, message);

        memcpy(&id, &message, sizeof(int)); // rmsg에 맨 앞부터 sizeof(int) (4byte) 만큼을 잘라서 id에 넣기.
        offset = sizeof(int); // offset 재설정 (위에서 사용한 offset을 다시 초기화)
        res.cmd_code = atoi(id); // res.id에 int로 변환한 id값 넣기

        memcpy(&res.msg, &message[offset], sizeof(res.msg)); // rmsg[offset] 부터 sizeof(rheader.name) (30byte)를 잘라서 rheader.name에 넣기.

        printf(" [DISASSEMBLE] %d %s\n", res.cmd_code, res.msg);

        if(res.cmd_code != 200)
                error_handling("NOT 200");
        FILE *fp;
        fp = fopen(file_name, "w+");

        if (fp == NULL) // 파일을 열기 실패했을 경우
        {
            printf("File open error!\n");
            exit(1);
        }

        printf(" Successfully opened file\n"); // 파일을 성공적으로 열음


        while(1)
        {
                memset(&message, 0, sizeof(message));
                res_len = read(sock, message, BUF_SIZE);
                if(res_len <= 0) break;
                //printf(" [FILE_REC] (%d): %s\n", res_len, message);

                fwrite(message, res_len, 1, fp);
        }
        fclose(fp);

        printf(" FILE TRANS SUCCESS!\n");

        close(sock);
        return 0;
}

void error_handling(char *message) 22
{
        fputs(message, stderr);
        fputc('\n', stderr);
        exit(1);
}