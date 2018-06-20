#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>

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
	int hasstu; // 학번 있는지없는지 식별
	char yesno[2];
	char newname[20];
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
	int check = 1;
	int money;
	char moneyYN[2];
	
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
		printf("\033[0;34m");
		fputs("Input student number: ", stdout); // 학번 입력은 노란색
		printf("\033[0m");
		signal(SIGALRM,handler);
		scanf(" %s", message);
		signal(SIGALRM,SIG_IGN);
		check = 2;

		//send student number
		sendto(sock, message, strlen(message), 0, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
		//memset(message, '0', strlen(message));
		//printf("1\n");	
		//file 있나 없나
		recvfrom(sock, &hasstu, sizeof(hasstu), 0, (struct sockaddr*)&from_adr, &adr_sz);
		//printf("hasstu\n");
		
		while(hasstu == 0) {
			printf("\nThere is no such name. Do you want to apply for meal?\nIf you apply, want to apply, insert (Y). If not, insert (N).\n");
			scanf("%s", yesno);
			sendto(sock, yesno, strlen(yesno), 0, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
			if(yesno[0] == 'Y'){ // 급식 신청하면
				check = 1;
				printf("\nInsert new name.\n");
				scanf("%s", newname); // 새 이름 입력받아서
				sendto(sock, newname, strlen(newname), 0, (struct sockaddr*)&serv_adr, sizeof(serv_adr)); // 서버에 보내줌
				break; // 이 파일로 진행
			}
			else if(yesno[0] == 'N'){ // 급식 신청 안하면
				printf("\n");
				check = 0;
				break; // 나감
			}
			else { // 이상한 글자 쓰면
				printf("\nStrange letter. Insert again.\n");
				continue;
			}
		}
		if(check == 0)
			continue;
		
		
		//printf("moneycheck\n");
		recvfrom(sock, &money, sizeof(money), 0, (struct sockaddr*)&from_adr, &adr_sz);

		if(money == 0) { // 남은 횟수 0 이하
			printf("\033[1;31m");
                        printf("\a\a\a\nNo money!\n\n"); // 알람 3번 : 급식 수 모자람
                        printf("\033[0m");
			printf("\nDo you want to charge more?\nIf you want, insert (Y). If not, insert (N)\n");
			scanf("%s", moneyYN);
			sendto(sock, moneyYN, strlen(moneyYN), 0, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
			if(moneyYN[0] == 'Y') { // 충전한다
				printf("\nHow much do you want to charge?\n");
				scanf("%d", &money);
				sendto(sock, &money, sizeof(money), 0, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
				printf("\nCharging finished. Restart.\n");
			}
			else
				printf("\nNot charging. Restart.\n");
			continue;
		}
		// else: 남은 횟수 영 초과
		//printf("OK\n");

		//recieve student information
		memset(&file, 0, sizeof(file));
		str_len = recvfrom(sock, &file, sizeof(file), 0, (struct sockaddr*)&from_adr, &adr_sz);
        	
        	printf("\nstudent number : %d\n", file.stunum);
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
		printf("\033[0;35m");
		printf("Input menu (1,2,3) : ");
		printf("\033[0m");
		scanf(" %c",&menu_n);
		getchar();
		printf("\n");
		while(1)
		{
			if(menu_n == '1' || menu_n == '2' || menu_n == '3')
				break;
			else
			{
				printf("\033[1;31m"); // 붉은색으로 변경
				printf("\a\aWrong number!!\nInput menus are 1, 2, 3\n\n"); // 알람 2번 : 메뉴 1,2,3 안고름
				printf("\033[0m");
				printf("\033[0;35m");
				printf("Input menu (1,2,3) : ");
				printf("\033[0m");
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
			printf("\n");
			printf("\033[1;31m");
			printf("\a%s\n",message); // 알람 1번 : 메뉴 3 - 1000원 추가
			printf("\033[0m");
			printf("\n");
			
			memset(&file,0,sizeof(file));
			str_len = recvfrom(sock, &file,sizeof(file),0,(struct sockaddr*)&from_adr,&adr_sz);
                        printf("student number : %d\n", file.stunum);
                        printf("student name : %s\n",file.name);
                        printf("student food count : %d\n",file.foodnum);			
		}
		memset(&file, 0, sizeof(file));
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
