#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
void already_booked_ticket(int sock);
int actions(int sock, int type);
int administration(int sock, int choice);
int customerandagent(int sock, int choice);

void already_booked_ticket(int sock){
	int t;
	read(sock, &t, sizeof(int));
	if(t== 0) printf("NO EXISTING RECORD OF TICKETS\n");
	else printf("BOOKED_TICKETS:\n");
	while(!getchar());
	while(t--){
		char name[20];
		int id, startseat_no, endseat_no, cancelled;
		
		read(sock,&id, sizeof(id));
		read(sock,&name, sizeof(name));
		read(sock,&startseat_no, sizeof(int));
		read(sock,&endseat_no, sizeof(int));
		read(sock,&cancelled, sizeof(int));
		if(!cancelled)
		printf("BookingID: %d\t1st Ticket: %d\tLast Ticket: %d\tTRAIN :%s\n", id, startseat_no,endseat_no, name);
	}
	printf("ENTER KEY TO CONTINUE SIR ......\n");
	while(getchar()!='\n');
	getchar();
}
int customerandagent(int sock, int choice)
{
	switch(choice)
	{
		case 2:
		{
			//book tickets------------------------------------------------
			int total_train, id, avseats, no, required_seats;
			char name[20];
			write(sock, &choice, sizeof(choice));
			read(sock, &total_train, sizeof(total_train));
			printf("ID\tTrain_NO\tAVAILABLE_SEAT\tTRAIN NAME\n");
			while(total_train--){
				read(sock, &id, sizeof(id));
				read(sock, &no, sizeof(no));
				read(sock, &avseats, sizeof(avseats));
				read(sock, &name, sizeof(name));
				if(strcmp(name, "deleted")!=0){
				printf("%d\t%d\\t\t\t%d\t\t%s\n", id, no, avseats, name);
			}
			
			}
			printf("Enter the train ID: "); 
			scanf("%d", &id);
			write(sock, &id, sizeof(id));
			read(sock, &avseats, sizeof(avseats));
			printf("Enter the number of seats you want :  "); 
			scanf("%d", &required_seats);
			if(avseats>=required_seats && required_seats>0)
				write(sock, &required_seats, sizeof(required_seats));
			else{
				required_seats = -1;
				write(sock, &required_seats, sizeof(required_seats));
			}
			read(sock, &choice, sizeof(choice));
			
			if(choice !=-1) printf("Tickets booked successfully\n");
			else printf("Tickets were not booked. Please try again.\n");
			printf("Press any key to continue...\n");
			while(getchar()!='\n');
			getchar();
			while(!getchar());
			return 1;
			
		//--------------------------------------------------------------	
		}
		case 1:{
			//View your bookings----------------------------------------
			
			write(sock, &choice, sizeof(choice));
			already_booked_ticket(sock);
			read(sock, &choice, sizeof(choice));
			return 2;
			
			//--------------------------------------------------------------
		}
		case 4:
		{
			//-----------------------------------------------------update bookings
			int temp;
			write(sock, &choice, sizeof(choice));
			already_booked_ticket(sock);
			printf("please enter the booking id to be updated: "); scanf("%d", &temp);
			write(sock, &temp, sizeof(int));	//Booking ID
			printf("What information do you want to update:\n1.Increase No of Seats\n2. Decrease No of Seats\nenter here: ");
			scanf("%d", &temp);
			write(sock, &temp, sizeof(int));	//Increase or Decrease
			if(temp == 1){
				printf("How many tickets do you want to increase: ");scanf("%d",&temp);
				write(sock, &temp, sizeof(int));	//No of Seats
			}else if(temp == 2){
				printf("How many tickets do you want to decrease: ");scanf("%d",&temp);
				write(sock, &temp, sizeof(int));	//No of Seats		
			}
			read(sock, &choice, sizeof(choice));
			if(choice == -2)
				printf("updation failed\n");
				else if(choice==-3)
				{
				printf("you can reduce the number of tickets less than you have currently BOSS||\n");
				}
			else printf("updation succeded.\n");
			while(getchar()!='\n');
			getchar();
			return 3;
		}
		case 3: 
		{
			//-------------------------------------------------cancel booking
			write(sock, &choice, sizeof(choice));
			already_booked_ticket(sock);
			int temp;
			printf("Enter the booking id to be deleted: "); scanf("%d", &temp);
			write(sock, &temp, sizeof(int));	//Booking ID
			read(sock, &choice, sizeof(choice));
			if(choice==-5)
			{
			printf("enter the valid booking id please\n");
			
			}
			if(choice == -2)
				printf("Operation failed. No more available seats\n");
			else printf("Operation succeded.\n");
			while(getchar()!='\n');
			getchar();
			return 3;
			}
			
		case 5:
		{
		int total_train, id, avseats, no, required_seats;
			char name[20];
			write(sock, &choice, sizeof(choice));
			read(sock, &total_train, sizeof(total_train));
			printf("TRAIN ID\tTRAIN_NO\tAVAILABLE_SEAT\tTRAIN NAME\n");
			while(total_train--){
				read(sock, &id, sizeof(id));
				read(sock, &no, sizeof(no));
				read(sock, &avseats, sizeof(avseats));
				read(sock, &name, sizeof(name));
				if(strcmp(name, "deleted")!=0){
				printf("%d\t%d\t\t\t%d\t\t%s\n", id, no, avseats, name);
			}
			
			}
			printf("Press any key to continue...\n");
			while(getchar()!='\n');
			getchar();
			while(!getchar());
			
			return 5;
		
		
		
		
		
		}	
		case 6: 
		{
			//-------------------------------------logout
			write(sock, &choice, sizeof(choice));
			read(sock, &choice, sizeof(choice));
			if(choice == 6) printf("Logged out successfully.\n");
			while(getchar()!='\n');
			getchar();
			return -1;
			break;
		}
		default: return -1;
	}
}

int administration(int sock, int choice){
	switch(choice)
	{
	
	case 1:
	{
	
	int total_train, id, avseats, no, required_seats;
			char name[20];
			write(sock, &choice, sizeof(choice));
			read(sock, &total_train, sizeof(total_train));
			printf("ID\tTRAIN_NO\tAVAILABLE_SEAT\tTRAIN NAME\n");
			while(total_train--){
				read(sock, &id, sizeof(id));
				read(sock, &no, sizeof(no));
				read(sock, &avseats, sizeof(avseats));
				read(sock, &name, sizeof(name));
				if(strcmp(name, "deleted")!=0){
				printf("%d\t%d\t\t\t%d\t%s\n", id, no, avseats, name);
			}
			
			}
			printf("Press any key to continue...\n");
			while(getchar()!='\n');
			getchar();
			while(!getchar());
			
			return choice;
	
	
	
	}
		case 2:
		{
			int no;
			char name[20];
			write(sock, &choice, sizeof(choice));
			printf("Enter Name of train\n");
			scanf("%s", name);
			printf("Enter Train No. : \n");
			scanf("%d", &no);
			write(sock, &name, sizeof(name));
			write(sock, &no, sizeof(no));
			read(sock, &choice, sizeof(choice));
			if(choice == 1 )
		    printf("New Train of name : %s is added successfully.\n",name);
		    
			while(getchar()!='\n');
			getchar();
			return choice;
			break;
		}
		case 4:
		{
			int total_train;
			write(sock, &choice, sizeof(choice));
			read(sock, &total_train, sizeof(int));
			while(total_train>0){
				int id, no;
				char name[20];
				read(sock, &id, sizeof(id));
				read(sock, &name, sizeof(name));
				read(sock, &no, sizeof(no));
				if(strcmp(name, "deleted")!=0){
					printf("%d.\t%d\t%s\n", id, no, name);}
					
				total_train--;
			}
			printf("\nchoose the train id to be deleted: ");
		    scanf("%d", &total_train);
			write(sock, &total_train, sizeof(int));
			read(sock, &choice, sizeof(choice));
			if(choice !=-5) printf("that train does not exist anymore in our system\n");
			else printf("Deletion failed ,try later\n");
			while(getchar()!='\n');
			getchar();
			return choice;
			break;
		}
		case 3:{
			int total_train;
			write(sock, &choice, sizeof(choice));
			read(sock, &total_train, sizeof(int));
			while(total_train>0){
				int id, no;
				char name[20];
				read(sock, &id, sizeof(id));
				read(sock, &name, sizeof(name));
				read(sock, &no, sizeof(no));
				if(!strcmp(name, "deleted"));else{
				printf("%d.\t%d\t%s\n", id+1,no, name);}
				
				total_train--;
			}
			printf("Enter 0 to cancel.\nEnter the train ID to modify: "); scanf("%d", &total_train);
			write(sock, &total_train, sizeof(int));
			printf("UPDATE (choose)\n\n\n\n1. Train Name\n2. Train No.\n3. Available Seats\n");
			printf("ENTER HERE\n ");scanf("%d", &total_train);
			write(sock, &total_train, sizeof(int));
			if(total_train == 2 || total_train == 3){
				read(sock, &total_train, sizeof(int));
				printf("Current Value: %d\n", total_train);				
				printf("Enter New Value: ");scanf("%d", &total_train);
				write(sock, &total_train, sizeof(int));
			}
			else{
				char name[20];
				read(sock, &name, sizeof(name));
				printf("Current Value Of name: %s\n", name);
				printf("Enter New Value of name  : ");scanf("%s", name);
				write(sock, &name, sizeof(name));
			}
			read(sock, &choice, sizeof(choice));
			if(choice == 3) printf("Train Data Modified Successfully\n");
			while(getchar()!='\n');
			getchar();
			return choice;
			break;
		}
		case 5:{
			write(sock, &choice, sizeof(choice));
			char pass[20],name[10];
			printf("Write your Name\n ");scanf("%s", name);
			strcpy(pass, getpass("Enter a secret password( DO NO SHARE WITH ANY ONE: \n"));
			write(sock, &name, sizeof(name));
			write(sock, &pass, sizeof(pass));
			read(sock, &choice, sizeof(choice));
			printf("The Account Number alloted for this ADMIN (root user ) is : %d\n", choice);
			read(sock, &choice, sizeof(choice));
			if(choice == 4)printf("NEW ADMIN IS CREATED SUCCESSFULLY\n");
			while(getchar()!='\n');
			getchar();
			return choice;
			break;
		}
		case 6: {
			int type, total_user, id;
			write(sock, &choice, sizeof(choice));
			printf("What kind of account do you want to delete?\n");
			printf("1. Customer\n2. Agent\n3. Admin\n");
			printf("Your Choice: ");
			scanf("%d", &type);
			write(sock, &type, sizeof(type));
			read(sock, &total_user, sizeof(total_user));
			while(total_user--){
				char name[10];
				read(sock, &id, sizeof(id));
				read(sock, &name, sizeof(name));
				if(strcmp(name, "deleted")!=0){
				printf("%d\t%s\n", id, name);
				}
			}
			printf("Enter the ID of the user you wants  to delete: ");scanf("%d", &id);
			write(sock, &id, sizeof(id));
			read(sock, &choice, sizeof(choice));
			if(choice == 5) printf("USER NO MORE EXIST : DELETED SUCCESSFULY\n");
			while(getchar()!='\n');
			getchar();
			return choice;
		}
		case 7: {write(sock, &choice, sizeof(choice));
			read(sock, &choice, sizeof(choice));
			if(choice==6) printf("Logged out successfully.\n");
			while(getchar()!='\n');
			getchar();
			return -1;
			break;}
		default: return -1;
	}
}

int actions(int sock, int type)
{
	int choice = 0;
	if(type == 1 || type == 2){
		system("clear");
		printf("=======================================================\n");
		printf("\t\t\t\t1.VIEW BOOKED SEAT\n");
		printf("\t\t\t\t2.BOOK TICKET\n");
		printf("\t\t\t\t3. CANCEL BOOOKED TICKET \n");
		printf("\t\t\t\t4.UPDATE BOOKING \n");
		printf("\t\t\t\t5 SHOW TRAIN LIST\n");
		printf("\t\t\t\t6. LOGOUT\n");
		printf("=======================================================\n");
		printf("Enter Your Choice: ");
		scanf("%d", &choice);
		return customerandagent(sock, choice);
	}
	else{
		system("clear");
		printf("=======================================================\n");
		printf("\t\t\t\t1. SHOW TRAIN LIST\n");
		printf("\t\t\t\tt2. ADD NEW TRAIN\n");
		printf("\t\t\t\t3. MODIFY EXISTING TRAIN\n");
		printf("\t\t\t\t4. DELETE EXISTING TRAIN\n");
		
		printf("\t\t\t\t5. ADD NEW ADMIN USER\n");
		printf("\t\t\t\t6. DELETE EXISTING ADMIN USER\n");
		printf("\t\t\t\t7. Logout\n");
		printf("=======================================================\n");
		printf("Enter Your Choice: ");
		scanf("%d", &choice);
		return administration(sock, choice);
	}
}
