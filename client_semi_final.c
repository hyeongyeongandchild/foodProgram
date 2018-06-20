#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#define BUF_SIZE 30
int server_port =  9998;

typedef struct item
{
	int stunum;
	char name[100];
	int foodnum;
} item;

void handler(int num);
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int sock;
	char message[BUFSIZ];
	char menu_n;
	char menu[100][100];
	int str_len;
	socklen_t adr_sz;
	item file;
	int j=0;
	int i=0;
	char num[1];
	char menuinfo[100];
	
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
		signal(SIGALRM,handler);
		scanf(" %s", message);
		signal(SIGALRM,SIG_IGN);
		if(!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
			break;
		
		//send student number
		sendto(sock, message, strlen(message), 0, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
		memset(&message, 0, sizeof(message));
		
		//recieve student information
		str_len = recvfrom(sock, &file, sizeof(file), 0, (struct sockaddr*)&from_adr, &adr_sz);
        
        if(file.foodnum<=0)
        {
            printf("Charge money!\n");
            continue;
        }
        printf("student number : %d\n", file.stunum);
        printf("student name : %s\n",file.name);
        printf("student food count : %d\n",file.foodnum);
        memset(&message, 0, sizeof(message));

		
		//recieve menu information
		for(j=0;j<3;j++)
		{
			str_len=recvfrom(sock,menuinfo,sizeof(menuinfo)-1,0,(struct sockaddr*)&from_adr,&adr_sz);
			menuinfo[str_len] = 0;
			printf("%s",menuinfo);
			
			str_len=recvfrom(sock,num,sizeof(num),0,(struct sockaddr*)&from_adr,&adr_sz);
			while(1)
			{	
				memset(&menuinfo,0,sizeof(menuinfo));
				str_len=recvfrom(sock,menuinfo,sizeof(menuinfo),0,(struct sockaddr*)&from_adr,&adr_sz);
				printf("%s",menuinfo);
				if(i==atoi(num)-1)
					break;	
				i++;	
			}
			i=0; 
		}	
			
		//send menu number
		printf("\n");
		printf("Input menu (1,2,3) : ");
		scanf(" %c",&menu_n);
		getchar();
		while(1)
		{
			if(menu_n == '1' || menu_n == '2' || menu_n == '3')
				break;
			else
			{
				
				printf("Input menu : ");
				scanf(" %c",&menu_n);
				getchar();
			}
		}	
		sendto(sock, &menu_n, sizeof(menu_n), 0, (struct sockaddr*)&serv_adr,sizeof(serv_adr));
		
		
		//recive update
		if(menu_n == '1' || menu_n == '2'){
			memset(&file,0,sizeof(file));
			str_len = recvfrom(sock, &file,sizeof(file),0,(struct sockaddr*)&from_adr,&adr_sz);
			printf("student number : %d\n", file.stunum);
      			printf("student name : %s\n",file.name);
			printf("student food count : %d\n",file.foodnum);
		}
		else if(menu_n == '3')
		{
			memset(&message,0,sizeof(message));
			str_len = recvfrom(sock, message,sizeof(message),0,(struct sockaddr*)&from_adr,&adr_sz);
			printf("%s\n",message);
			
			memset(&file,0,sizeof(file));
			str_len = recvfrom(sock, &file,sizeof(file),0,(struct sockaddr*)&from_adr,&adr_sz);
                        printf("student number : %d\n", file.stunum);
                        printf("student name : %s\n",file.name);
                        printf("student food count : %d\n",file.foodnum);			
		}

		printf("\n");
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
void handler(int num)
{
	printf("Hurry up! Input please!");
	exit(0);
}
