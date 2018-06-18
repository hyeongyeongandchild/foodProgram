#include <stdio.h>
#include <ctype.h>
#include <pthread.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <signal.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
//#define PORT 9998

char errormessage[50] = "Wrong number\nInsert again\n";

typedef struct item // 학번 이름 남은 횟수 구조체
{
    int stunum;
    char name[100];
    int foodnum;
} item;

void error_handling(char *message);

int main(int argc, char *argv[])
{
    int A_num=0;
    int serv_sock;
    socklen_t clnt_adr_sz;
    char message[BUFSIZ];
    char Hi[100];
    int str_len = 0;
    int i=0;
    int count = 0;
    char index;
    struct sockaddr_in serv_adr, clnt_adr;
    FILE* fin, *fmenu;
    item file;
    int menucheck = 1;
    char menuname[20], menuinfo[50];
    char menus[100][100];
    char temp[100], filename[30];
    
    if(argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }
    
    serv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(serv_sock == -1)
        error_handling("socket() error");
    
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));
    
    if(bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");
    
    while(1)
    {
        memset(&message, 0, sizeof(message));
        clnt_adr_sz=sizeof(clnt_adr);
        
        str_len = recvfrom(serv_sock, message, sizeof(message), 0, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
        
        
        message[str_len] = 0;
        
        printf("Message from client: %s\n", message);
                
        if(str_len==10)// 학번을 받았을때
        {
            strcat(message, ".txt"); // "학번" + ".txt"
            strcpy(filename, message);
            fin = fopen(message, "r");
            
            if(fin == NULL){
                printf("file open error\n");
                printf("message : %s\n", message);
                exit(1);
            }
            fscanf(fin, "%d %s %d\n", &file.stunum, file.name, &file.foodnum);
            fclose(fin);
            
            sendto(serv_sock, &file, sizeof(file), 0, (struct sockaddr *)&clnt_adr, clnt_adr_sz);
            
            for(i=1;i<4;i++) // 1, 2, 3번 메뉴 열어서 각각 출력
            {
                
                switch(i)
                {
                    case 1 :
                        strcpy(menuinfo,"\nMenu 1 is BREAD : \n\n");
                        sendto(serv_sock, &menuinfo, strlen(menuinfo), 0, (struct sockaddr *)&clnt_adr, clnt_adr_sz);
                        break;
                    case 2 :
                        strcpy(menuinfo,"\nMenu 2 is GENERAL : \n\n");
                        sendto(serv_sock, &menuinfo, strlen(menuinfo), 0, (struct sockaddr *)&clnt_adr, clnt_adr_sz);
                        break;
                    case 3 :
                        strcpy(menuinfo,"\nMenu 3 is SPECIAL : \n\n");
                        sendto(serv_sock, &menuinfo, strlen(menuinfo), 0, (struct sockaddr *)&clnt_adr, clnt_adr_sz);
                        break;
                }
                sprintf(menuname, "%d", i);
                strcat(menuname, ".txt");
                fmenu = fopen(menuname, "r");
                while(1) {
			fgets(menus[count], sizeof(menus[count])-1,fmenu);
			if(feof(fmenu))
				break;
			
                    sendto(serv_sock, &menus[count] , strlen(menus[count]), 0, (struct sockaddr *)&clnt_adr, clnt_adr_sz);
                    count++;
                }
                fclose(fmenu);
            	
		}
        }
        
        else if(str_len == 1)//음식 메뉴를 받았을 때
        {
            
            menucheck = 1;

            if(strcmp(message,"3")==0)
            {
                strcpy(temp, "Give staff additional 1000won");
                sendto(serv_sock, &temp, strlen(temp), 0, (struct sockaddr *)&clnt_adr, clnt_adr_sz);
            }

            if(menucheck == 1) { // 메뉴가 1, 2, 3 중 하나가 들어왔으면
                file.foodnum--; // 남은 급식 횟수를 1 줄이고
//                printf("%d\n",file.foodnum);
                fin = fopen(filename, "w"); // 학생정보 파일을 새로 열어서
                fprintf(fin, "%d %s %d", file.stunum, file.name, file.foodnum); // 입력해줌
//                printf("%d %s %d\n", file.stunum, file.name, file.foodnum); // 입력해
                fclose(fin);
                sendto(serv_sock, &file, sizeof(file), 0, (struct sockaddr *)&clnt_adr, clnt_adr_sz);

            }
        }
        
        else // 학번도 음식 메뉴도 안 들어왔을 때
            sendto(serv_sock, &errormessage, strlen(errormessage), 0, (struct sockaddr *)&clnt_adr, clnt_adr_sz); // 에러메시지 보냄
    }
    close(serv_sock);
    
    return 0;
    
}
void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

