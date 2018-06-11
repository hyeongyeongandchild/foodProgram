#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
//char server_ip[14]= {1,9,2,.,1,6,8,.,4,3,.,2,5,2};
int server_port =  9998;

typedef struct item
{
	int stunum;
	char name[100];
	int foodnum;
} item;

void error_handling(char *message);

int main(int argc, char *argv[])
{
	int sock;
	char message[BUFSIZ];
	int str_len;
	socklen_t adr_sz;
	item file;
	char server_ip[15];
	strcpy(server_ip,"192.168.43.252");

	struct sockaddr_in serv_adr, from_adr;
	if(argc!=3){
		printf("Usage : %s\n", argv[0]);
		exit(1);
	}

	sock=socket(PF_INET, SOCK_DGRAM, 0);
	if(sock == -1)
		error_handling("socket() error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));
	
	while(1)
	{
		fputs("Input student number: ", stdout);
		gets(message);		
		//message[strlen(message)-1] = '\0';
		//printf("%s", message);				
		if(!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
			break;
		
		sendto(sock, message, strlen(message), 0, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
		
		memset(&message, 0, sizeof(message));

		str_len = recvfrom(sock, &file, sizeof(file), 0, (struct sockaddr*)&from_adr, &adr_sz);	
		
		message[str_len]=0;
		printf("student number : %d\n", file.stunum);
		printf("student name : %s\n",file.name);
		printf("student food count : %d\n",file.foodnum);
		memset(&message, 0, sizeof(message));
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

