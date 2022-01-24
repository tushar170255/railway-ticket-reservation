#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "client_utility_script.h"


int main(int argc,int argv[])
{
//if(argc!=3){
//		printf("Invalid number of arguments given. You have to provide 1st ip address and then portno");
//		exit(0);
//	}
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1){
		printf("socket creation failed\n");
		exit(0);
		}
struct sockaddr_in client;
	client.sin_family=AF_INET;
	client.sin_port= htons(8888);
	client.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if(connect(sock, (struct sockaddr *)&client, sizeof(client))==-1){
		printf	("connection  failed: error occured\n");
		exit(0);
	}
	
	printf("connection established\n");
	
	while(1)
{
	int choice;
	system("clear");
	printf("\t\t\t*****************************\n");
	printf("\t*WELCOME TO INDIAN RAILWAY TICKET BOOKING SYSTEM (IRCTC)*\n");
	printf("\t\t\t*****************************\n");
	printf("\t\t\t\t1. Login In\n");
	printf("\t\t\t\t2. Create Accounts\n");
	printf("\t\t\t\t3. Exit\n");
	printf("\t\t\t*****************************\n");
	printf("Enter Your Choice\n");
	scanf("%d", &choice);
	write(sock, &choice, sizeof(choice));
	if(choice==1)
	{
		//*************************************login in**********************************************************
		int type, acc_no;
		char pass[20];
		printf("Enter the type of account you wanted to log in :\n\n\n");
		printf("1. Customer\n2. Agent\n3. Administration\n\n\n");
		printf("Enter here: \n");
		scanf("%d", &type);
		printf("Enter Your Account Number: \n");
		scanf("%d", &acc_no);
		strcpy(pass,getpass("Enter your password: \n"));

		write(sock, &type, sizeof(type));
		write(sock, &acc_no, sizeof(acc_no));
		write(sock, &pass, strlen(pass));

		int validity;
		read(sock, &validity, sizeof(validity));
		if(validity == 1){
			while(actions(sock, type)!=-1);
			system("clear");
			
		}
		else
		{
			printf("please check your details : they are incorrect .\n");
			while(getchar()!='\n');
			getchar();
			
		}
		
		}
	
		else if(choice==2)
	{
		//***********************************create accounts********************************
				int type, acc_no;
		char pass[20],  name[10];
		printf("Enter the Account type\t:\n\n\n");
		printf("\t\t\t1. Customer\n\t\t\t2. Agent\n\t\t\t3. Administration\n\n\n");
		printf("Enter your choice: \n");
		scanf("%d", &type);
		printf("Enter your name: \n");
		scanf("%s", name);
		strcpy(pass,getpass("Enter the password: \n"));
		
		write(sock, &type, sizeof(type));
		write(sock, &name, sizeof(name));
		write(sock, &pass, strlen(pass));

		read(sock, &acc_no, sizeof(acc_no));
		printf("%d",acc_no);
		printf("Keep safe your account no for further login: %d\n", acc_no);
		while(getchar()!='\n');
		getchar();		
	//-----------------------------------------------------------------------------	
	}
	else
	{
		//exiting
		exit(0);
	}
	
	
	
	}
	
	

close(sock);

return 0;
}
