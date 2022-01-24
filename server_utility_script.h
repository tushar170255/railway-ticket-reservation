
#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

struct account_info
{
int id;
char name [20] ;
char password [20] ;
};

struct train_info{

int id;
char name[20];
int no;
int available_seat;
int lastused_seat;


};

struct booking_info
{

int id;
int type;
int account_no;
int train_id;
char train_name[20];
int startseat_no;
int endseat_no;
int cancelled;


};

char *ACC[3] = {"./database/account_section/customer", "./database/account_section/agent", "./database/account_section/administration"};

int seat_reservation(int sock,int id,int type)
{
	
	int fd = open("./database/train_database", O_RDWR);

		struct flock lock;
		lock.l_type = F_WRLCK;
		lock.l_start = 0;
		lock.l_len = 0;
		lock.l_whence = SEEK_SET;
		lock.l_pid = getpid();
		
		fcntl(fd, F_SETLKW, &lock);

		struct train_info temp;
		int fp = lseek(fd, 0, SEEK_END);
		int total_train = fp / sizeof(struct train_info);
		write(sock, &total_train, sizeof(int));
		lseek(fd, 0, SEEK_SET);
		while(fp != lseek(fd, 0, SEEK_CUR))
		{
			read(fd, &temp, sizeof(struct train_info));
			write(sock, &temp.id, sizeof(int));
			write(sock, &temp.no, sizeof(int));	
			write(sock, &temp.available_seat, sizeof(int));	
			write(sock, &temp.name, sizeof(temp.name));		
		}
		memset(&temp,0,sizeof(struct train_info));
		int tid, seat;
		read(sock, &tid, sizeof(tid));
		lseek(fd, tid*sizeof(struct train_info), SEEK_SET);
		read(fd, &temp, sizeof(struct train_info));
		write(sock, &temp.available_seat, sizeof(int));
		read(sock, &seat, sizeof(seat));
		if(seat>0){
			temp.available_seat -= seat;
			int fd2 = open("./database/booking_information", O_RDWR);
			fcntl(fd2, F_SETLKW, &lock);
			struct booking_info booking;
			int fp2 = lseek(fd2, 0, SEEK_END);
			if(fp2 > 0)
			{
				lseek(fd2, -1*sizeof(struct booking_info), SEEK_CUR);
				read(fd2, &booking, sizeof(struct booking_info));
				booking.id++;
			}
			else 
			{booking.id = 0;}
			booking.type = type;
			booking.account_no = id;
			booking.train_id = tid;
			booking.cancelled = 0;
			strcpy(booking.train_name, temp.name);
			booking.startseat_no = temp.lastused_seat + 1;
			booking.endseat_no = temp.lastused_seat + seat;
			temp.lastused_seat = booking.endseat_no;
			write(fd2, &booking, sizeof( struct booking_info));
			lock.l_type = F_UNLCK;
			fcntl(fd2, F_SETLK, &lock);
		 	close(fd2);
			lseek(fd, -1*sizeof(struct train_info), SEEK_CUR);
			write(fd, &temp, sizeof(temp));
		}
		fcntl(fd, F_SETLK, &lock);
	 	close(fd);
	 	return seat;

}

void already_booked_ticket(int sock, int id, int type){
	int fd = open("./database/booking_information", O_RDONLY);
	struct flock lock;
	lock.l_type = F_RDLCK;
	lock.l_start = 0;
	lock.l_len = 0;
	lock.l_whence = SEEK_SET;
	lock.l_pid = getpid();
	
	fcntl(fd, F_SETLKW, &lock);

	int fp = lseek(fd, 0, SEEK_END);
	int t= 0;
	if(fp == 0)
	{	write(sock, &t, sizeof(t));}
	
	else{
		struct booking_info booking[10];
		while(fp>0 && t<10){
			struct booking_info temp;
			fp = lseek(fd, -1*sizeof(struct booking_info), SEEK_CUR);
			read(fd, &temp, sizeof(struct booking_info));
			if(temp.account_no == id && temp.type == type)
				booking[t++] = temp;
			fp = lseek(fd, -1*sizeof(struct booking_info), SEEK_CUR);
		}
		write(sock, &t, sizeof(t));
		fp=0;
		for(fp=0;fp<t;fp++){
		
		
			write(sock, &booking[fp].id, sizeof(booking[fp].id));
			write(sock, &booking[fp].train_name, sizeof(booking[fp].train_name));
			write(sock, &booking[fp].startseat_no, sizeof(int));
			write(sock, &booking[fp].endseat_no, sizeof(int));
			write(sock, &booking[fp].cancelled, sizeof(int));}
			
		
	}	
	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &lock);
	close(fd);
}



//*******************************************************
int customer_and_agent(int sock, int id, int type)
{
	int choice;
	read(sock, &choice, sizeof(choice));
	
	
	if(choice== 2)
	{
	
		
		int remaining_seats=seat_reservation(sock,id,type);
		if(remaining_seats<=0)
			choice = -1;
		write(sock, &choice, sizeof(choice));
		return 1;
		//-----------------------------------------------------------------
	}
	
	if(choice == 1)
	{
		//------------------------------------------------viewing bookings
		already_booked_ticket(sock, id, type);
		
		write(sock, &choice, sizeof(choice));
		return 2;
		//----------------------------------------------------------------------
	}
	if(choice == 4)
	{
		//-----------------------------------------------------------update booking
		already_booked_ticket(sock, id, type);

		int fd1 = open("./database/train_database", O_RDWR);
		int fd2 = open("./database/booking_information", O_RDWR);
		struct flock lock;
		lock.l_type = F_WRLCK;
		lock.l_start = 0;
		lock.l_len = 0;
		lock.l_whence = SEEK_SET;
		lock.l_pid = getpid();

		fcntl(fd1, F_SETLKW, &lock);
		fcntl(fd2, F_SETLKW, &lock);

		int t;
		struct train_info temp1;
		struct booking_info temp2;
		read(sock, &t, sizeof(int));	//Read the Booking ID to updated
		// read the booking to be updated
		lseek(fd2, 0, SEEK_SET);
		lseek(fd2, t*sizeof(struct booking_info), SEEK_CUR);
		read(fd2, &temp2, sizeof(temp2));
		lseek(fd2, -1*sizeof(struct booking_info), SEEK_CUR);
		printf("%d %s %d\n", temp2.id, temp2.train_name, temp2.endseat_no);
		// read the train details of the booking
		lseek(fd1, 0, SEEK_SET);
		lseek(fd1, (temp2.train_id-1)*sizeof(struct train_info), SEEK_CUR);
		read(fd1, &temp1, sizeof(temp1));
		lseek(fd1, -1*sizeof(struct train_info), SEEK_CUR);
		printf("%d %s %d\n", temp1.id, temp1.name, temp1.available_seat);


		read(sock, &t, sizeof(int));	//Increase or Decrease


		if(t==1)
		{//increase---------------------------------------------------
			read(sock, &t, sizeof(int)); //No of Seats
			if(temp1.available_seat>= t){
				temp2.cancelled = 1;
				temp1.available_seat += t;
				write(fd2, &temp2, sizeof(temp2));

				int total_seat = temp2.endseat_no - temp2.startseat_no + 1 + t;
				struct booking_info booking;

				int fp2 = lseek(fd2, 0, SEEK_END);
				lseek(fd2, -1*sizeof(struct booking_info), SEEK_CUR);
				read(fd2, &booking, sizeof(struct booking_info));

				booking.id++;
				booking.type = temp2.type;
				booking.account_no = temp2.account_no;
				booking.id = temp2.id;
				booking.cancelled = 0;
				strcpy(booking.train_name, temp2.train_name);
				booking.startseat_no = temp1.lastused_seat + 1;
				booking.endseat_no = temp1.lastused_seat + total_seat;

				temp1.available_seat -= total_seat;
				temp1.lastused_seat = booking.endseat_no;

				write(fd2, &booking, sizeof(booking));
				write(fd1, &temp1, sizeof(temp1));
			}
			else{
				choice = -2;
				write(sock, &choice, sizeof(choice));
			}
		}
		else
		{//decrease	------------------------------------------------		
			read(sock, &t, sizeof(int)); //No of Seats
			if(temp2.endseat_no - t +1< temp2.startseat_no){
				//temp2.cancelled = 1;
				//temp1.available_seat += t;
				choice=-3;
			}
			else if(temp2.endseat_no - t +1== temp2.startseat_no)
			{
			temp2.cancelled=1;
			temp1.available_seat+=t;
			
			}
			
			else{
				temp2.endseat_no -= t;
				temp1.available_seat += t;
			}
			write(fd2, &temp2, sizeof(temp2));
			write(fd1, &temp1, sizeof(temp1));
		}
		lock.l_type = F_UNLCK;
		fcntl(fd1, F_SETLK, &lock);
		fcntl(fd2, F_SETLK, &lock);
		close(fd1);
		close(fd2);
		//if(choice>0)
		write(sock, &choice, sizeof(choice));
		return 3;

	}
	if(choice == 3)
	 {
		//----------------------------------------------------------cancel booking
		already_booked_ticket(sock, id, type);


		struct flock lock;
		lock.l_type = F_WRLCK;
		lock.l_start = 0;
		lock.l_len = 0;
		lock.l_whence = SEEK_SET;
		lock.l_pid = getpid();

		int fd1 = open("./database/train_database", O_RDWR);
		int fd2 = open("./database/booking_information", O_RDWR);
		fcntl(fd1, F_SETLKW, &lock);
		//fcntl(fd2, F_SETLKW, &lock);


		int t;
		struct train_info temp1;
		struct booking_info temp2;
		read(sock, &t, sizeof(int));	//Read the Booking ID to deleted
		//lseek(fd2, 0, SEEK_SET);
		lseek(fd2, t*sizeof(struct booking_info), SEEK_SET);
		
		

		lock.l_start = t*sizeof(struct booking_info);
		lock.l_len = sizeof(struct booking_info);
		fcntl(fd2, F_SETLKW, &lock);

		
		read(fd2, &temp2, sizeof(temp2));
		if(id==temp2.account_no){
		
		lseek(fd2, -1*sizeof(struct booking_info), SEEK_CUR);
		printf("%d %s %d\n", temp2.train_id, temp2.train_name, temp2.endseat_no);
		
		
		// read the train details of the booking
		//lseek(fd1, 0, SEEK_SET);

		lseek(fd1, (temp2.train_id)*sizeof(struct train_info), SEEK_SET); ///// -1 removed
		lock.l_start = (temp2.train_id)*sizeof(struct train_info);
		lock.l_len = sizeof(struct train_info);
		fcntl(fd1, F_SETLKW, &lock);
		read(fd1, &temp1, sizeof(temp1));
		lseek(fd1, -1*sizeof(struct train_info), SEEK_CUR);
		printf("%d %s %d\n", temp1.id, temp1.name, temp1.available_seat);
		temp1.available_seat += temp2.endseat_no - temp2.startseat_no + 1;
		temp2.cancelled=1;
		write(fd2, &temp2, sizeof(temp2));
		write(fd1, &temp1, sizeof(temp1));
		
		lock.l_type = F_UNLCK;
		fcntl(fd1, F_SETLK, &lock);
		fcntl(fd2, F_SETLK, &lock);
		close(fd1);
		close(fd2);
		
		write(sock, &choice, sizeof(choice));
		
		}
		
		
		else{
		choice=-5;
		
		write(sock,&choice,sizeof(choice));
		
		}
		return 4;
	}
	if(choice==5)
	{
	int fd = open("./database/train_database", O_RDWR);

		struct flock lock;
		lock.l_type = F_WRLCK;
		lock.l_start = 0;
		lock.l_len = 0;
		lock.l_whence = SEEK_SET;
		lock.l_pid = getpid();
		
		fcntl(fd, F_SETLKW, &lock);

		struct train_info temp;
		int fp = lseek(fd, 0, SEEK_END);
		int total_train = fp / sizeof(struct train_info);
		write(sock, &total_train, sizeof(int));
		lseek(fd, 0, SEEK_SET);
		while(fp != lseek(fd, 0, SEEK_CUR))
		{
			read(fd, &temp, sizeof(struct train_info));
			write(sock, &temp.id, sizeof(int));
			write(sock, &temp.no, sizeof(int));	
			write(sock, &temp.available_seat, sizeof(int));	
			write(sock, &temp.name, sizeof(temp.name));		
		}
	
	
	close(fd);
	return 5;
	
	}
	if(choice == 6)
	 {
		write(sock, &choice, sizeof(choice));
		return -1;
	}
	
	return 0;
}

//***********************************ADMINISTRATION **********************************
int administration(int sock, int id){
	int choice;
	read(sock, &choice, sizeof(choice));
	if(choice==1)
	{
	int fd = open("./database/train_database", O_RDWR);

		struct flock lock;
		lock.l_type = F_WRLCK;
		lock.l_start = 0;
		lock.l_len = 0;
		lock.l_whence = SEEK_SET;
		lock.l_pid = getpid();
		
		fcntl(fd, F_SETLKW, &lock);

		struct train_info temp;
		int fp = lseek(fd, 0, SEEK_END);
		int total_train = fp / sizeof(struct train_info);
		write(sock, &total_train, sizeof(int));
		lseek(fd, 0, SEEK_SET);
		while(fp != lseek(fd, 0, SEEK_CUR))
		{
			read(fd, &temp, sizeof(struct train_info));
			write(sock, &temp.id, sizeof(int));
			write(sock, &temp.no, sizeof(int));	
			write(sock, &temp.available_seat, sizeof(int));	
			write(sock, &temp.name, sizeof(temp.name));		
		}
	
	
	close(fd);
	
	
	
	
	
	return choice;
	}
	if(choice == 2){
		//add a train
		int id = 0;
		int no; 
		char name[20];
		read(sock, &name, sizeof(name));
		read(sock, &no, sizeof(no));
		struct train_info temp1, temp2;

		temp1.id = id;
		temp1.no = no;
		strcpy(temp1.name, name);
		temp1.available_seat = 30;
		temp1.lastused_seat = 0;

		int fd = open("./database/train_database", O_RDWR);
		struct flock lock;
		lock.l_type = F_WRLCK;
		lock.l_start = 0;
		lock.l_len = 0;
		lock.l_whence = SEEK_SET;
		lock.l_pid = getpid();

		fcntl(fd, F_SETLKW, &lock);

		int fp = lseek(fd, 0, SEEK_END);
		if(fp == 0){
			write(fd, &temp1, sizeof(temp1));
			lock.l_type = F_UNLCK;
			fcntl(fd, F_SETLK, &lock);
			close(fd);
			write(sock, &choice, sizeof(choice));
		}
		else{
			lseek(fd, -1 * sizeof(struct train_info), SEEK_CUR);
			read(fd, &temp2, sizeof(temp2));
			temp1.id = temp2.id + 1;
			write(fd, &temp1, sizeof(temp1));
			write(sock, &choice, sizeof(choice));	
			lock.l_type = F_UNLCK;
			fcntl(fd, F_SETLK, &lock);
			close(fd);
		}
		return choice;
	}
	if(choice== 4){
		int fd = open("./database/train_database", O_RDWR);

		struct flock lock;
		lock.l_type = F_WRLCK;
		lock.l_start = 0;
		lock.l_len = 0;
		lock.l_whence = SEEK_SET;
		lock.l_pid = getpid();
		
		fcntl(fd, F_SETLKW, &lock);

		int fp = lseek(fd, 0, SEEK_END);
		int total_train = fp / sizeof(struct train_info);
		printf(" total no of train in the system :%d\n",total_train);
		write(sock, &total_train, sizeof(int));
		lseek(fd, 0, SEEK_SET);
		struct train_info temp;
		while(fp != lseek(fd, 0, SEEK_CUR)){
			//printf("FP :%d  FD :%ld\n",fp,lseek(fd, 0, SEEK_CUR));
			read(fd, &temp, sizeof(struct train_info));
			write(sock, &temp.id, sizeof(int));
			write(sock, &temp.name, sizeof(temp.name));
			write(sock, &temp.no, sizeof(int));			
		}
		//int train_id=-1;
		read(sock, &total_train, sizeof(int));
		if(total_train >0) 
		{
			struct train_info temp;
			
			lseek(fd, (total_train)*sizeof(struct train_info), SEEK_SET);
			read(fd, &temp, sizeof(struct train_info));			
			printf("%s is deleted\n", temp.name);
			strcpy(temp.name,"deleted");
			lseek(fd, -1*sizeof(struct train_info), SEEK_CUR);
			write(fd, &temp, sizeof(struct train_info));
			
		}
		write(sock, &total_train, sizeof(int));
		lock.l_type = F_UNLCK;
		fcntl(fd, F_SETLK, &lock);
		close(fd);
		return choice;
	}
	if(choice == 3){
		int fd = open("./database/train_database", O_RDWR);

		struct flock lock;
		lock.l_type = F_WRLCK;
		lock.l_start = 0;
		lock.l_len = 0;
		lock.l_whence = SEEK_SET;
		lock.l_pid = getpid();
		
		fcntl(fd, F_SETLKW, &lock);

		int fp = lseek(fd, 0, SEEK_END);
		int total_train = fp / sizeof(struct train_info);
		write(sock, &total_train, sizeof(int));
		lseek(fd, 0, SEEK_SET);
		while(fp != lseek(fd, 0, SEEK_CUR)){
			struct train_info temp;
			read(fd, &temp, sizeof(struct train_info));
			write(sock, &temp.id, sizeof(int));
			write(sock, &temp.name, sizeof(temp.name));
			write(sock, &temp.no, sizeof(int));			
		}
		read(sock, &total_train, sizeof(int));

		struct train_info temp;
		lseek(fd, 0, SEEK_SET);
		lseek(fd, (total_train-1)*sizeof(struct train_info), SEEK_CUR);
		read(fd, &temp, sizeof(struct train_info));			

		read(sock, &total_train,sizeof(int));
		if(total_train == 1){
			char name[20];
			write(sock, &temp.name, sizeof(temp.name));
			read(sock, &name, sizeof(name));
			strcpy(temp.name, name);
		}
		else if(total_train == 2){
			write(sock, &temp.no, sizeof(temp.no));
			read(sock, &temp.no, sizeof(temp.no));
		}
		else{
			write(sock, &temp.available_seat, sizeof(temp.available_seat));
			read(sock, &temp.available_seat, sizeof(temp.available_seat));
		}

		total_train= 3;
		printf("%s\t%d\t%d\n", temp.name, temp.no, temp.available_seat);
		lseek(fd, -1*sizeof(struct train_info), SEEK_CUR);
		write(fd, &temp, sizeof(struct train_info));
		write(sock, &total_train, sizeof(int));

		lock.l_type = F_UNLCK;
		fcntl(fd, F_SETLK, &lock);
		close(fd);
		return choice;
	}
	if(choice == 5){
		int type=3, fd, acc_no=0;
		char pass[20], name[10];
		struct account_info temp;
		read(sock, &name, sizeof(name));
		read(sock, &pass, sizeof(pass));

		if((fd = open(ACC[type-1], O_RDWR))==-1)printf("File Error\n");
		struct flock lock;
		lock.l_type = F_WRLCK;
		lock.l_start = 0;
		lock.l_len = 0;
		lock.l_whence = SEEK_SET;
		lock.l_pid = getpid();

		fcntl(fd,F_SETLKW, &lock);
		int fp = lseek(fd, 0, SEEK_END);
		fp = lseek(fd, -1 * sizeof(struct account_info), SEEK_CUR);
		read(fd, &temp, sizeof(temp));
		temp.id++;
		strcpy(temp.name, name);
		strcpy(temp.password, pass);
		write(fd, &temp, sizeof(temp));
		write(sock, &temp.id, sizeof(temp.id));
		lock.l_type = F_UNLCK;
		fcntl(fd, F_SETLK, &lock);

		close(fd);
		choice=4;
		write(sock, &choice, sizeof(choice));
		return choice;
	}
	if(choice== 6){
		int type, acc_no;
		struct account_info temp;
		read(sock, &type, sizeof(type));

		int fd = open(ACC[type - 1], O_RDWR);
		struct flock lock;
		lock.l_type = F_WRLCK;
		lock.l_start = 0;
		lock.l_whence = SEEK_SET;
		lock.l_len = 0;
		lock.l_pid = getpid();

		fcntl(fd, F_SETLKW, &lock);

		int fp = lseek(fd, 0 , SEEK_END);
		int total_user = fp/ sizeof(struct account_info);
		write(sock, &total_user, sizeof(int));

		lseek(fd, 0, SEEK_SET);
		while(fp != lseek(fd, 0, SEEK_CUR)){
			read(fd, &temp, sizeof(struct account_info));
			write(sock, &temp.id, sizeof(temp.id));
			write(sock, &temp.name, sizeof(temp.name));
		}

		read(sock, &acc_no, sizeof(acc_no));
		if(id == 0){write(sock, &choice, sizeof(choice));}
		else{
			lseek(fd, 0, SEEK_SET);
			lseek(fd, (acc_no-1)*sizeof(struct account_info), SEEK_CUR);
			read(fd, &temp, sizeof(struct account_info));
			lseek(fd, -1*sizeof(struct account_info), SEEK_CUR);
			strcpy(temp.name,"deleted");
			strcpy(temp.password, "");
			write(fd, &temp, sizeof(struct account_info));
			write(sock, &choice, sizeof(choice));
		}

		lock.l_type = F_UNLCK;
		fcntl(fd, F_SETLK, &lock);

		close(fd);

		return choice;
	}
	if(choice == 7) {
		write(sock,&choice, sizeof(choice));
		return -1;
	}
}
//************************************** LOG IN UTILITY*************************************************
int login(int sock)
{
	int type, acc_no, fd, v=1, i=0, success=0;
	char pass[20];
	struct account_info acc;
	read(sock, &type, sizeof(type));
	read(sock, &acc_no, sizeof(acc_no));
	read(sock, &pass, sizeof(pass));
	if(type==1)
	{
		if((fd = open("./database/account_section/customer", O_RDWR))==-1)
			printf("File Error\n");	
	}
	else if(type==2)
	{
		if((fd = open("./database/account_section/agent", O_RDWR))==-1)
			printf("File Error\n");	
	}	
	else if(type==3)
	{
		if((fd = open("./database/account_section/administration", O_RDWR))==-1)
			printf("File Error\n");	
	}	
	struct flock lock;
	
	lock.l_start = (acc_no-1)*sizeof(struct account_info);
	lock.l_len = sizeof(struct account_info);
	lock.l_whence = SEEK_SET;
	lock.l_pid = getpid();

	if(type == 1){
	// --------------------------------------------------User
		lock.l_type = F_WRLCK;
		fcntl(fd,F_SETLK, &lock);
		lseek(fd, (acc_no - 1)*sizeof(struct account_info), SEEK_CUR);
		read(fd, &acc, sizeof(struct account_info));
		if(acc.id == acc_no){
			if(!strcmp(acc.password, pass)){
				write(sock, &v, sizeof(v));
				while(customer_and_agent(sock, acc.id, type)!=-1);
				success = 1;
			}
		}
		lock.l_type = F_UNLCK;
		fcntl(fd, F_SETLK, &lock);
		close(fd);
		if(success)
		return 3;
	//-------------------------------------------------------------------
	}
	else if(type == 2){
	//--------------------------------------------------- Agent
		
		lock.l_type = F_RDLCK;
		fcntl(fd,F_SETLK, &lock);
		lseek(fd, (acc_no - 1)*sizeof(struct account_info), SEEK_SET);
		read(fd, &acc, sizeof(struct account_info));
		if(acc.id == acc_no){
			if(!strcmp(acc.password, pass)){
				write(sock, &v, sizeof(v));
				while(customer_and_agent(sock, acc.id, type)!=-1);
				success = 1;
			}
		}
		lock.l_type = F_UNLCK;
		fcntl(fd, F_SETLK, &lock);
		close(fd);
		if(success) return 3;
			
	}
	else if(type == 3){
		// -----------------------------------------------------Admin
		lock.l_type = F_WRLCK;
		int ret = fcntl(fd,F_SETLK, &lock);
		if(ret != -1){
			
			lseek(fd, (acc_no - 1)*sizeof(struct account_info), SEEK_SET);///////CUR
			read(fd, &acc, sizeof(struct account_info));
			if(acc.id == acc_no){
				if(!strcmp(acc.password, pass)){
					write(sock, &v, sizeof(v));
					while(-1!=administration(sock, acc.id));
					success = 1;
				}
			}
			lock.l_type = F_UNLCK;
			fcntl(fd, F_SETLK, &lock);
		}
		close(fd);
		if(success)
		return 3;
	}
	write(sock, &i, sizeof(i));
	return 3;
}

//***************** Create account********************************
int create_account(int sock)
{
	int type, fd, acc_no=0;
	char pass[20], name[10];
	struct account_info acc;

	read(sock, &type, sizeof(type));
	read(sock, &name, sizeof(name));
	read(sock, &pass, sizeof(pass));

	if(type==1)
	{
		if((fd = open("./database/account_section/customer", O_RDWR))==-1)
			printf("File Error\n");	
	}
	else if(type==2)
	{
		if((fd = open("./database/account_section/agent", O_RDWR))==-1)
			printf("File Error\n");	
	}	
	else if(type==3)
	{
		if((fd = open("./database/account_section/administration", O_RDWR))==-1)
			printf("File Error\n");	
	}	
	struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_start = 0;
	lock.l_len = 0;
	lock.l_whence = SEEK_SET;
	lock.l_pid = getpid();

	fcntl(fd,F_SETLKW, &lock);
printf("successfully created\n");
	int position = lseek(fd, 0, SEEK_END);

	if(position==0)
	{
	//----------------------------1st time entry in  register
		acc.id = 1;
		strcpy(acc.name, name);
		strcpy(acc.password, pass);
		write(fd, &acc, sizeof(acc));
		write(sock, &acc.id, sizeof(acc.id));
	}
	else{
		//------------------------------------not 1st entry in register
		position = lseek(fd, -1 * sizeof(struct account_info), SEEK_END);
		read(fd, &acc, sizeof(acc));
		acc.id++;
		strcpy(acc.name, name);
		strcpy(acc.password, pass);
		write(fd, &acc, sizeof(acc));
		write(sock, &acc.id, sizeof(acc.id));
	}

	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &lock);

	close(fd);
	return 3;
}

//***************service request function***************************************
void service_request(int sock){
	int choice;
	read(sock, &choice, sizeof(int));
	printf("Client connected\n");
	while(1)
	{		
		if(choice==1) 
		{
		login(sock);
			read(sock, &choice, sizeof(int));
		}
		else if(choice==2) 
		{
			create_account(sock);
			read(sock, &choice, sizeof(int));
		}
		if(choice==3)
		{ 
			break;
		}	
	}
	close(sock);
	printf("Client disconnected\n");
}
