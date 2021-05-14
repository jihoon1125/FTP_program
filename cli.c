////////////////////////////////////////////////////////////
// File Name	: cli.c							          //
// Date		: 2020/06/26						          // 
// OS		: Ubuntu 16.04 LTS 64bits				      //  
// Author	: Ham Ji Hoon 						          //	
// Student ID	: 2016722086						      //
// -------------------------------------------------------//
// TITLE: System Programming practice 4      (cli program)//
// Description: add data connection being server too      //
////////////////////////////////////////////////////////////

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> //file descriptor, system call,...
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>

#define BUF_SIZE 256
#define RCV_BUFF 50000

char* convert_addr_to_str(struct in_addr s_addr, unsigned short s_port, char* port_msg){
	 /////////////////////////////////////////////////////////////
    // convert_addr_to_str                                      //
    // =========================================================// 
    // Input: s_addr -> client's ip address                     //
    //        s_port -> client's new port number                //
	 //       port_msg -> result message                    	//
    // OUtput: port_msg				    			            //
    // Purpose: make port message to send server	 	    	//
    //////////////////////////////////////////////////////////////
	char result[BUF_SIZE]={0};
	char buf[BUF_SIZE]={0};   
	strcpy(result, "PORT "); //start with 'PORT '
	strcat(result, inet_ntoa(s_addr));//change to dotted decimal
	for(int i=0; i<strlen(result); i++){
		if(result[i] == '.')
			result[i] = ',';//dot to comma		
	}

	strcat(result, ",");
	sprintf(buf, "%d", s_port&0x00ff);//change s_port to little endian way
	strcat(result, buf);
	strcat(result, ",");
	sprintf(buf, "%d", (s_port&0xff00)/256);
	strcat(result, buf);
	
	strcpy(port_msg, result);
	return port_msg;

}
/* printing rcv_buff on stdout stream */
void process_result(char* rcv_buff)
{
    write(STDOUT_FILENO, rcv_buff, strlen(rcv_buff));//stdout buffer from server result
}

char transfer_mode = 'I';

int conv_cmd(char* buff, char* cmd_buff, char* filename, int fd){
    //////////////////////////////////////////////////////////////
    // conv_cmd    			                                    //
    // =========================================================// 
    // Input: buff ->  user command buffer                      //
    //        cmd_buff ->  FTP command buffer                   //
    // OUtput: -1 error, 0 successs     			            //
    // Purpose: To convert user command to FTP command 	    	//
    //////////////////////////////////////////////////////////////
    char* error_msg;
	char** argv;
	int argc = 1;	
	opterr = 0;
	optind = 0;
  	int aflag = 0, bflag = 0;	
	char* opt_arr = "";	
	int index, c;	    
	
	/* let's mimic argc, argv in earnest */
	char arr1[256] = { 0 };
	char arr2[256] = { 0 };	
	/* make two input_buf clone */
	strcpy(arr1, buff);
	strcpy(arr2, buff);

	/* calculate argc */
	if (strtok(arr1, " "))
		argc++;

	while (strtok(NULL, " ") != NULL)
	{
		argc++;
	}

	/* argv memory allocation */
	argv = (char**)malloc(sizeof(char*) * argc);
	argv[0] = "./srv.c";//feature of main function argv

	/*	parsing clone to argv	*/
	for (int i = 1; i < argc; i++)
	{
		if (i == 1)
			argv[i] = strtok(arr2, " ");
		else
			argv[i] = strtok(NULL, " ");
	}	

	// setting no problem in getopt

	
while(1){
//*************** ls -> NLST *******************//
		if (!strcmp("ls", argv[1])) {
			strcpy(cmd_buff, "NLST");
			opt_arr = "al";// setting opt_arr as "al", so that we can get get option -a, -l		
			break;	
		}	
//*************** list -> LIST *******************//
		if (!strcmp("ls", argv[1])) {
			strcpy(cmd_buff, "LIST");
			opt_arr = "al";// setting opt_arr as "al", so that we can get get option -a, -l		
			break;	
		}	

//*************** pwd -> PWD *******************//
		if (!strcmp("pwd", argv[1])) {
			strcpy(cmd_buff, "PWD");
			break;
		}

//************ cd -> CWD, cd .. -> CDUP **************//
		if (!strcmp("cd", argv[1])) {
			if (argc > 2) {
				if (!strcmp("..", argv[2])) {// check if cd .. 
					strcpy(cmd_buff, "CDUP");//if cd .. -> CDUP 
					break;
					//isCDUP=1;
				}
			}
			strcpy(cmd_buff, "CWD");//if not cd .., cd -> CWD
			break;
		}
//***************mkdir -> MKD*******************//
		if (!strcmp("mkdir", argv[1])) {
			strcpy(cmd_buff, "MKD");
			break;
		}

//***************delete -> DELE*******************//
		if (!strcmp("delete", argv[1])) {
			strcpy(cmd_buff, "DELE");
			break;
		}

//***************rmdir -> RMD*******************//
		if (!strcmp("rmdir", argv[1])) {
			strcpy(cmd_buff, "RMD");
			break;
		}

//***************rename -> RNFR&RNTO*******************//
		if (!strcmp("rename", argv[1])) {
			char listen[BUF_SIZE]={0};
			strcpy(cmd_buff, "RNFR ");
			strcat(cmd_buff, argv[2]);
			strcat(cmd_buff, "\n");
			write(fd, cmd_buff, strlen(cmd_buff)+1);
			read(fd, listen, BUF_SIZE);
			write(STDOUT_FILENO, listen, strlen(listen)+1);
			memset(cmd_buff, 0, sizeof(cmd_buff));
			if(!strncmp(listen, "350", 3))
				{
					strcpy(cmd_buff, "RNTO ");
					strcat(cmd_buff, argv[3]);
					strcat(cmd_buff, "\n");
					return 0;
				}
			break;
		}
//***************quit -> QUIT*******************//
		if (!strcmp("quit", argv[1])) {
			strcpy(cmd_buff, "QUIT");
			break;
		}

//***************get -> RETR*******************//
		if (!strcmp("get", argv[1])) {
			strcpy(cmd_buff, "RETR");
			strcpy(filename, argv[2]);
			break;
		}
//***************put -> STOR*******************//
		if (!strcmp("put", argv[1])) {
			strcpy(cmd_buff, "STOR");
			strcpy(filename, argv[2]);
			break;
		}

//***************BIN -> TYPE I*******************//
		if (!strcmp("bin", argv[1])||!strcmp("binary", argv[2])) {
			strcpy(cmd_buff, "TYPE I\n");			
			transfer_mode = 'I';
			return 0;
		}
//***************ASCII -> TYPE A*******************//
		if (!strcmp("ascii", argv[1])||!strcmp("ascii", argv[2])) {
			strcpy(cmd_buff, "TYPE A\n");		
			transfer_mode = 'A';
			return 0;
		}

//****If not one of above commands, write command exception****//
		strcpy(cmd_buff, "comx");//srv will identify this message
		strcat(cmd_buff, "\n");
		return -1;
	}


//*********** parsing options of commands ***********//
	while ((c = getopt(argc, argv, opt_arr)) != -1)
	{
		switch (c)//option case
		{
		case 'a'://set aflag
			strcat(cmd_buff, " -a");//option for NLST
			break;
		case 'l':// set bflag
			strcat(cmd_buff, " -l");//option for NLST
			break;		
		default:
			strcpy(cmd_buff, "optx");//other options are not needed, srv will identify this message
			strcat(cmd_buff, "\n");
			return -1;
		}


	}

	if (!strncmp(cmd_buff, "CDUP", 4))//if CDUP having one more argument, so print argument from optind + 2
		index = optind + 2;	
	else
	index = optind + 1;// print from optind +1

	for (; index < argc; index++) {//filling output_buf with arguments
		strcat(cmd_buff, " ");//option and arguments are distinguished by space
		strcat(cmd_buff, argv[index]);	
}
	strcat(cmd_buff, "\n");
	return 0;//exit
}

void log_in(int sockfd){
	//////////////////////////////////////////////////////////////
    // log_in		    	                                    //
    // =========================================================// 
    // Input:   sockfd -> client's socket descriptor		    //
    // OUtput: void						  			            //
    // Purpose: get ip blocked message and log in if accessible //
    //////////////////////////////////////////////////////////////
	int n;
	char user[BUF_SIZE], *passwd, buf[BUF_SIZE];

	/* (CHECK IP ACCEPTABLE) */
	n = read(sockfd, buf, BUF_SIZE);
	buf[n] = '\0';

	if(!strncmp(buf, "431",3)){	
		write(STDOUT_FILENO, "431 This client can't access. Close the session\n", 49);	
		close(sockfd);
		return;
		}
	else if(!strncmp(buf, "220",3)){
	 write(STDOUT_FILENO, "220 ACCEPTED! FTP server ready\n", 32);
	while(1){
		char user_buf[256];
		char pw_buf[256]={0};
		/* pass user name, passwd to server */
		printf("Input ID: ");
		fflush(stdout);//output buffer clear
		strcpy(user_buf, "USER ");
		read(STDIN_FILENO, user, BUF_SIZE);
		user[strlen(user)-1] = '\0';//remove enter
		strcat(user_buf, user);
		write(sockfd, user_buf, strlen(user_buf)+1);//id
		read(sockfd, buf, BUF_SIZE);
		write(STDOUT_FILENO, buf, strlen(buf)+1);

		if(!strncmp(buf, "331", 3)){// if id checked, enter password
		passwd = getpass("Input passwd: ");
		strcpy(pw_buf, "PASS ");
		strcat(pw_buf, passwd);
		write(sockfd, pw_buf, strlen(pw_buf)+1);		
		n = read(sockfd, buf, BUF_SIZE);
		write(STDOUT_FILENO, buf, strlen(buf)+1);
		
		/* log in success */
		if(!strncmp(buf, "230" ,3)){		
			break;
		}
		/* LOGIN FAIL */
		else if(!strncmp(buf, "430" ,3)){			
				printf("** Log-in failed **\n");				
			}
		/* three times fail */
			else{
				//buf disconnection				
				printf("** Connection closed **\n");
				close(sockfd);
				break;
			}
		}

		/* ID not checked */
		else{
			printf("username doesn't exist\n");
			close(sockfd);
			break;
		}
		

	}
}
else
	return;
}


int main (int argc, char **argv)
{   
    //////////////////////////////////////////////////////////////
    // main    		    	                                    //
    // =========================================================// 
    // Input:   argc, argv -> server IP address, port number    //
    // OUtput: -1 error, 0 successs     			            //
    // Purpose: sending FTP command to server program 	    	//
    //////////////////////////////////////////////////////////////
   struct sockaddr_in server_control, server_data; 
   struct sockaddr_in client_data;    
   char buff[BUF_SIZE]={0}, response_buff[BUF_SIZE]={0};//256
   char cmd_buff[BUF_SIZE] = {0};//256
   char rcv_buff[RCV_BUFF]={0};//50000
   char* port_msg;
   int controlfd, datafd, n; 
   int serverfd;
   int len;
   char *hostport;//port message to send server
   char filename[256];
   port_msg = (char*)malloc(BUF_SIZE);
    /*open control connection socket*/
    if((controlfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        write(STDERR_FILENO,"Can't create socket.\n", strlen("Can't create socket.\n"));
        return -1;
    }  

	/*open data connection socket*/
	 if((datafd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        write(STDERR_FILENO,"Can't create socket.\n", strlen("Can't create socket.\n"));
        return -1;
    }  

    /* initialize server_control */
    memset(&server_control, 0, sizeof(server_control));
    server_control.sin_family= PF_INET;
    server_control.sin_addr.s_addr = inet_addr(argv[1]);
    server_control.sin_port = htons(atoi(argv[2]));

    /* initialize client_data */
    bzero((char *)&client_data, sizeof(client_data));
    client_data.sin_family = PF_INET;
    client_data.sin_addr.s_addr = inet_addr(argv[1]);
    client_data.sin_port = htons(12345);

	/* bind data connection socket */
    if(bind(datafd, (struct sockaddr *)&client_data,sizeof(client_data)) <0){
        write(STDERR_FILENO, "Server: can't bind\n", strlen("Server: can't bind\n"));
        return -1;
    }

    /* connecting control connection socket to server */
    if(connect(controlfd, (struct sockaddr*)&server_control, sizeof(server_control))<0){
        write(STDOUT_FILENO, "Can't connect.\n", strlen("Can't connect.\n"));
        return -1;
    }   
    
	 log_in(controlfd);//log in
while(1){
         memset(buff, 0, sizeof(buff));//reset buff
       	 memset(cmd_buff, 0, sizeof(cmd_buff));//reset cmd_buff
         write(STDOUT_FILENO,"> ",3);	
         read(STDIN_FILENO, buff, BUF_SIZE);
         buff[strlen(buff)-1]='\0';//remove enter 
		
		 if(conv_cmd(buff, cmd_buff, filename, controlfd) < 0)/*convert user command to FTP command, if conversion failed, transmit raw message */
        {
            write(STDERR_FILENO, "command convert failed, transmit raw message..\n", BUF_SIZE);
			exit(1);
              }
		
		if((!strncmp(cmd_buff, "NLST", 4)||!strncmp(cmd_buff, "LIST", 4))||((!strncmp(cmd_buff, "STOR", 4))||(!strncmp(cmd_buff, "RETR", 4))))
		 {
			 hostport = convert_addr_to_str(client_data.sin_addr, client_data.sin_port, port_msg);
		 write(controlfd, hostport, strlen(hostport)+1);//write include null
		 listen(datafd, 5);
		 len = sizeof(server_data);
         serverfd = accept(datafd, (struct sockaddr*)&server_data, &len);//accept from server	
		 
		 read(controlfd, response_buff, BUF_SIZE);
		 write(STDOUT_FILENO, response_buff, strlen(response_buff));//print message from server	

		 if(strncmp("200", response_buff, 3)!=0)
		 {
			  printf("port command not successed\n");
			  exit(1);
		 }         

        write(controlfd, cmd_buff, strlen(cmd_buff)+1); //write cmd_buff to control connection            
		read(controlfd, rcv_buff, RCV_BUFF);//read buffer to rcv_buff		
		write(STDOUT_FILENO, rcv_buff, strlen(rcv_buff)+1);//print message 

		if(!strncmp(cmd_buff, "NLST", 4)||!strncmp(cmd_buff, "LIST", 4)){
			if((n = read(serverfd, rcv_buff, RCV_BUFF)) > 0){					    
        		rcv_buff[n] = '\0';
                process_result(rcv_buff); /*display command result */		    	
				printf("\nOK. %d bytes is received\n", n);
                        }			
                else//reading from server failed
				{
					printf("server disconnected!\n");
					}            
		}		

		else if(!strncmp(cmd_buff, "RETR", 4)){//get
			char c[1];
			int count=0;
			FILE* newfd;
			newfd = fopen(filename, "wb");//open new file
			if(transfer_mode = 'I'){// binary mode
			while(read(serverfd, c, 1)>0){
				if(c[0]==EOF)//it means end of file
					break;
				fputc(c[0], newfd);
				count++;				
			}			
			fclose(newfd);
			}
			else{// ascii mode
				while(read(serverfd, c, 1)>0){
				if(c[0]==EOF)
					break;
				fputc(c[0], newfd);
				count++;			
				}
			
			fclose(newfd);
			}
			printf("\nOK. %d bytes is received.\n", count);//byte amount print
		}

		else if(!strncmp(cmd_buff, "STOR", 4)){//put
			char c[1];
			int count =0;
			int newfd;
			newfd = open(filename, O_RDONLY);
			if(transfer_mode = 'I'){//binary mode
			while(read(newfd, c, 1)){
				write(serverfd, c, 1);
				count++;
			}
			c[0] = EOF;
			write(serverfd, c, 1);
			close(newfd);
			}
			else{//ascii mode
			while(read(newfd, c, 1)){
				write(serverfd, c, 1);
				count++;
			}
			c[0] = EOF;
			write(serverfd, c, 1);
			close(newfd);
			}
			printf("\nOK. %d bytes is sent.\n", count);
		}			
			 close(serverfd); //close data connection
			}   
			else
			{
				write(controlfd, cmd_buff, strlen(cmd_buff)+1);//no need of data connection
			}
			

			read(controlfd, rcv_buff, RCV_BUFF);//display received message			
			write(STDOUT_FILENO, rcv_buff, strlen(rcv_buff)+1);
			if(!strncmp("221", rcv_buff, 3))
				break;
			
}
	close(serverfd);//close data connection socket
	close(controlfd);  //close control connection socket
	close(datafd);//closing data connection listening socket
	return 0; 
}

