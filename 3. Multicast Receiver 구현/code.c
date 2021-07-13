#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 50

typedef struct
{
    int seq;
    char msg[46];
} RECEIVE_DATA;

int main(int argc, char *argv[])
{
	int recv_sock;
	int str_len;
	char buf[BUF_SIZE+1];
	struct sockaddr_in addr;
	struct ip_mreq join_addr;

	recv_sock=socket(PF_INET, SOCK_DGRAM, 0);
 	memset(&addr, 0, sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	addr.sin_port=htons(9100);
	RECEIVE_DATA red;

	if(bind(recv_sock, (struct sockaddr*) &addr, sizeof(addr))==-1)
	{
		printf("bind() error");
		close(recv_sock);
		exit(1);
	}

	join_addr.imr_multiaddr.s_addr=inet_addr("239.0.1.100");
	join_addr.imr_interface.s_addr=htonl(INADDR_ANY);

	setsockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&join_addr, sizeof(join_addr));

	char result[7][46] = {0,};
	printf("ready\n");

	int r_seq = 0;

	while(1)
	{
		str_len=recvfrom(recv_sock, buf, BUF_SIZE, 0, NULL, 0);
		if(str_len<0)
			break;

		int offset = 0;
		char id[5];

		memcpy(&id, &buf, sizeof(int));
		offset = sizeof(int);
		red.seq = atoi(id);
		memcpy(&red.msg, &buf[offset], sizeof(red.msg));

		memcpy(&result[red.seq], &red.msg, sizeof(red.msg));
		printf("REC: [%d] %s\n", red.seq, result[red.seq]);

		r_seq += (red.seq+1);
		if(r_seq >= 28){ printf("done\n\n"); break; }
	}
	FILE *fp = fopen("result123.txt", "w+");
	for(int i = 0; i < 7; i++)
	{
		fwrite(result[i], 1, strlen(result[i]), fp);
		fwrite("\n", 1, strlen("\n"), fp);
		printf("REC: [%d] %s\n", i, result[i]);
	}
	printf("file write success\n");
	fclose(fp);
	close(recv_sock);
	return 0;
}