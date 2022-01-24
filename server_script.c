#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <semaphore.h>
#include <signal.h>
#include"server_utility_script.h"



int main()
{
// **************** signal handling ***********
signal(SIGTSTP, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	
	int sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock==-1) {
		printf("socket is not not created : 'ERROR OCCURED'\n");
		exit(0);
	}


int optval = 1;
int optlen = sizeof(optval);
if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, optlen)==-1){
		printf("set socket is failed. try again\n");
		exit(0);
	}
struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(8888);
	
	if(bind(sock,(struct sockaddr*) & server,sizeof(server))==-1)
	{
	printf("binding of socket is failed . try again\n");
	exit(0);
	}
	
	if(listen(sock,100)==-1)
	{
	printf(" listening of socket is failed\n");
	exit(0);
	}
	
	/*while(1)
	{
	int nsock;
	if((nsock=accept(sock,(struct sockaddr *) NULL,NULL))==-1)
	{
	
	printf("error occured while creating a connection\n");
	exit(0);
	}
	if(!fork())
	{
	close(sock);
	service_request(nsock);
	exit(0);
	
	
	}
	else{
	
	close(nsock);
	exit(0);
	
	}
	
	
	
	
	
	
	}*/
	while(1){ 
		int nsock;
		if((nsock = accept(sock, (struct sockaddr *)NULL, NULL))==-1){
			printf("connection error\n");
			exit(0);
		}		
		if(fork()==0) {
			close(sock);
			service_request(nsock);
			exit(0);
		} else if(fork() > 0) {
			close(nsock);
		}
	}

	

close(sock);
return 0;

}

