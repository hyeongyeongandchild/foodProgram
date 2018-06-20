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
typedef struct temp // 학번 이름 남은 횟수 구조체
{
    int stunum_temp;
    char name_temp[100];
    int foodnum_temp;
} temp;

void error_handling(char *message);
int cnt=0;
int main(int argc, char *argv[])
{
    temp NEW_temp[1000];//새로운학번의 정보를 받는 구조체
    int len_size;
    int A_num=0;
    int Money=0;
    int serv_sock;
    socklen_t clnt_adr_sz;
    char message[BUFSIZ]; // 그외의 나머지 응답
    char message2[100];// 학번의 추가여부에 대한 응답
    char message3[100];// /new id 이름받기
    char message4[100];//FOOD TIKET RECHARGE 여부
    int wanting_Quntity;//충전을 원하는 식권의양
    char Hi[100];
    int str_len = 0;
    int str_len2 = 0;//
    int str_len3 = 0 ;
    int str_len4 = 0;
    int i=0;
    int count = 0;
    char index;
    struct sockaddr_in serv_adr, clnt_adr;
    FILE* fin, *fmenu , *fin2;
    item file;
    int menucheck = 1;
    char menuname[20], menuinfo[50];
    char menus[100][100];
    char temp[100], filename[30] ,temp2[100];
    int QWE =1;
    int ASK_MASAGE=1;
    
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
        
        str_len = 0;
        str_len = recvfrom(serv_sock, message, sizeof(message), 0, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
        QWE = 1;
        
        message[str_len] = 0;
        
        printf("Message from client: %s\n", message);
                
        if(str_len==10)// 학번을 받았을때
        {
            strcpy(temp2, message);//학번 저장
            strcat(message, ".txt"); // "학번" + ".txt"
            strcpy(filename, message);
            fin = fopen(message, "r");
            
            if(fin == NULL){// 이전에 없던 학번이 들어 왔을때 , 여부 묻기
                
                QWE=0;
                str_len2=0;
                ASK_MASAGE=0;//초기화
                sendto(serv_sock, &ASK_MASAGE, sizeof(ASK_MASAGE), 0, (struct sockaddr *)&clnt_adr, clnt_adr_sz);
                memset(&message2, 0, sizeof(message2));
                while(str_len2==0)//학번 받을때 까지 와일
                {
                    
                    str_len2 = recvfrom(serv_sock, message2, sizeof(message2), 0, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
                    message2[str_len2] = 0;
                    printf("message2 : %s\n", message2);
                }
                if(message2[0]=='Y')// 새로운 학번 파일을 만들기
                {
                    
                    str_len3 = 0;
                    memset(&message3, 0, sizeof(message3));
                    while(str_len3==0)//이름받을때 까지 와일
                    {
                        
                        str_len3 = recvfrom(serv_sock, message3, sizeof(message3), 0, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
                        message3[str_len3] = 0;
                        printf("message3 : %s\n", message3);
                    }
                    
                    
                    //새로운 값 NEW_temp 구조체에 넣어주기
                    NEW_temp[cnt].stunum_temp = atoi(temp2);//temp2 는 학번 저장값
                    strcpy(NEW_temp[cnt].name_temp,message3); // message3는 새로운 아이디의 이름
                    NEW_temp[cnt].foodnum_temp = 20;// 충전값
                    
                    //쓰기 형식으로 파일열기
                    fin2=fopen(filename,"w");
                    if(fin2!=NULL)
                    {
                        
                        fprintf(fin2,"%d %s %d",NEW_temp[cnt].stunum_temp,NEW_temp[cnt].name_temp,NEW_temp[cnt].foodnum_temp);
                        
                        fclose(fin2);
                    }
                    else if(fin2==NULL)
                    {
                        printf("fin2 file open error\n");
                        exit(1);
                    }
                    
                }
                else// 안만들기
                {
                    continue;
                }
            
            }
            
            if(fin==NULL)//이전에 학번이 없었을때 파일오픈해주기
            {
                fin = fopen(message, "r");
            }
            
            if(QWE!=0)
            {
                ASK_MASAGE=1;
                sendto(serv_sock, &ASK_MASAGE, sizeof(ASK_MASAGE), 0, (struct sockaddr *)&clnt_adr, clnt_adr_sz);
            }
            fscanf(fin,"%d %s %d\n", &file.stunum, file.name, &file.foodnum);
            printf("%d %s %d\n",file.stunum, file.name, file.foodnum);
            printf("%d\n",ASK_MASAGE);
            if(file.foodnum<=0)//식권 충전시키기
            {
                
                fclose(fin);
                
                fin=fopen(message,"w");
                Money=0;
//                printf("1_1\n");
                sendto(serv_sock, &Money, sizeof(Money), 0, (struct sockaddr *)&clnt_adr, sizeof(clnt_adr));
//                printf("2_1\n");
                str_len4 = 0;
                memset(&message4, 0, sizeof(message4));
                while(str_len4==0)
                {
                    str_len4 = recvfrom(serv_sock, message4, sizeof(message4), 0, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
                    message4[str_len4] = 0;
                    printf("message4 : %s\n", message4);
                }
                if(message4[0]=='Y')
                {
                    
                    recvfrom(serv_sock,&wanting_Quntity,sizeof(wanting_Quntity), 0, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
                    file.foodnum = wanting_Quntity;
                    fprintf(fin,"%d %s %d",file.stunum, file.name, file.foodnum);
                    fclose(fin);
                    continue;
                    
                }
                else
                    continue;
                
            }
            else//충전시킬필요 없다
            {
                Money=1;
//                printf("1_2\n");
                sendto(serv_sock, &Money, sizeof(Money), 0, (struct sockaddr *)&clnt_adr, clnt_adr_sz);
//                printf("2_2\n");
                //continue;
            }
                
            fclose(fin);
            //printf("1_3\n");
            //printf("1)%d %s %d\n",file.stunum,file.name,file.foodnum);
            len_size = sendto(serv_sock, &file, sizeof(file), 0, (struct sockaddr *)&clnt_adr, clnt_adr_sz);
            //printf("%d\n",len_size);
            //printf("2_3\n");
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

