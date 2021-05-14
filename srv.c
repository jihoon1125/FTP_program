////////////////////////////////////////////////////////////
// File Name	: srv.c							          //
// Date		: 2020/06/26						          // 
// OS		: Ubuntu 16.04 LTS 64bits				      //  
// Author	: Ham Ji Hoon 						          //	
// Student ID	: 2016722086						      //
// -------------------------------------------------------//
// TITLE: System Programming practice 4  (srv program)    //
// Description: add data connection with user			  //
////////////////////////////////////////////////////////////


#include <stdio.h>
#include <string.h> // bzero
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> //file descriptor, system call,..
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>
#include <arpa/inet.h>//inet function
#include <fcntl.h>

#define BUF_SIZE 256
#define RESULT_BUFF 50000
//************* my_stat structure to save file information ************//
typedef struct my_stat {
	char permission[11];
	char nlink[30];
	char uid[20];
	char gid[20];
	char size[32];
	char name[128];	
	char* month;
	int day;
	int hour;
	int minute;
}my_stat;

void write_mystat(my_stat* ms, struct stat* buf) {
	//////////////////////////////////////////////////////////////
	// write_mystat        					   					//
	// =========================================================// 
	// Input: ms -> my_stat structure to write data		    	//
	//	      buf -> stat structure to read data 	    		// 
	// OUtput: void						    					//
	// Purpose:read data from buf and save to ms		    	//
	//////////////////////////////////////////////////////////////

	struct tm* time;//to get time data
	struct group* grp;//to get group id
	struct passwd* pwd;//to get user id

//******** if file type is directory, add slash in file name ********//
	if (S_ISDIR(buf->st_mode))
		strcat(ms->name, "/");

//****** write nlink, size by string type for space_control function *******//
	sprintf(ms->nlink, "%ld", buf->st_nlink);
	sprintf(ms->size, "%ld", buf->st_size);

//************* write uid, gid *************///
	pwd = getpwuid(buf->st_uid);
	grp = getgrgid(buf->st_gid);
	strcpy(ms->uid, pwd->pw_name);
	strcpy(ms->gid, grp->gr_name);

//******** write month data by appropriate string *********//
	time = localtime(&(buf->st_mtime));
	if ((time->tm_mon) + 1 == 1)
		ms->month = "Jan";
	else if ((time->tm_mon) + 1 == 2)
		ms->month = "Feb";
	else if ((time->tm_mon) + 1 == 3)
		ms->month = "Mar";
	else if ((time->tm_mon) + 1 == 4)
		ms->month = "Apr";
	else if ((time->tm_mon) + 1 == 5)
		ms->month = "May";
	else if ((time->tm_mon) + 1 == 6)
		ms->month = "Jun";
	else if ((time->tm_mon) + 1 == 7)
		ms->month = "Jul";
	else if ((time->tm_mon) + 1 == 8)
		ms->month = "Aug";
	else if ((time->tm_mon) + 1 == 9)
		ms->month = "Sep";
	else if ((time->tm_mon) + 1 == 10)
		ms->month = "Oct";
	else if ((time->tm_mon) + 1 == 11)
		ms->month = "Nov";
	else if ((time->tm_mon) + 1 == 12)
		ms->month = "Dec";

//******** write the other time data *********//
	ms->day = time->tm_mday;
	ms->hour = time->tm_hour;
	ms->minute = time->tm_min;

}


void check_mode(struct stat* buf, my_stat* ms) {
	//////////////////////////////////////////////////////////////
	// check_mode       					    				//
	// =========================================================// 
	// Input: ms -> my_stat structure to write permission	    //
	//	      buf -> stat structure to read mode	    		// 
	// OUtput: void						    					//
	// Purpose:read mode from buf and save permission to ms	    //
	//////////////////////////////////////////////////////////////

//****** Check if file type is directory ******//
	if (S_ISDIR(buf->st_mode))
		ms->permission[0] = 'd';
	else if (S_ISLNK(buf->st_mode))
		ms->permission[0] = 'l';
	else if (S_ISCHR(buf->st_mode))
		ms->permission[0] = 'c';
	else if (S_ISBLK(buf->st_mode))
		ms->permission[0] = 'b';
	else if (S_ISSOCK(buf->st_mode))
		ms->permission[0] = 's';
	else if (S_ISFIFO(buf->st_mode))
		ms->permission[0] = 'P';
	else
		ms->permission[0] = '-';

//****** CHECK USER PERMISSION ******//
	if (buf->st_mode & S_IRUSR)
		ms->permission[1] = 'r';
	else
		ms->permission[1] = '-';
	if (buf->st_mode & S_IWUSR)
		ms->permission[2] = 'w';
	else
		ms->permission[2] = '-';
	if (buf->st_mode & S_IXUSR)
		ms->permission[3] = 'x';
	else if (buf->st_mode & S_ISUID)
		ms->permission[3] = 's';
	else
		ms->permission[3] = '-';

//****** CHECK GROUP PERMISSION ******//
	if (buf->st_mode & S_IRGRP)
		ms->permission[4] = 'r';
	else
		ms->permission[4] = '-';
	if (buf->st_mode & S_IWGRP)
		ms->permission[5] = 'w';
	else
		ms->permission[5] = '-';
	if (buf->st_mode & S_IXGRP)
		ms->permission[6] = 'x';
	else if (buf->st_mode & S_ISGID)
		ms->permission[6] = 's';
	else
		ms->permission[6] = '-';

//****** CHECK THE OTHERS PERMISSION ******//	
	if (buf->st_mode & S_IROTH)
		ms->permission[7] = 'r';
	else
		ms->permission[7] = '-';
	if (buf->st_mode & S_IWOTH)
		ms->permission[8] = 'w';
	else
		ms->permission[8] = '-';
	if (buf->st_mode & S_IXOTH)
		ms->permission[9] = 'x';
	else
		ms->permission[9] = '-';

	ms->permission[10] = 0;//null

}

void space_control(my_stat** ms, int entry_num, char* std, int index, char* output_buf)
{
	//////////////////////////////////////////////////////////////
	// space_control      					    				//
	// =========================================================// 
	// Input: ms -> my_stat structure to read control object    //
	//	      entry_num ->  to loop entry_num times	    		// 
	//	      std -> standard item to execute space        		//
	//	      index -> ms's index to execute		   			//
	//	      output_buf-> output_buf to add space	    		//
	// OUtput: void						    					//
	// Purpose: arrange output_buf by controlling space count   //
	//////////////////////////////////////////////////////////////

	int max_length = 0;//reset max_length


	//****** arrange space before adding size ******//	
	if (!strcmp(std, "size")) {
		for (int i = 0; i < entry_num; i++)
		{
			if (strlen(ms[i]->size) > max_length)
				max_length = strlen(ms[i]->size);
		}
		for (int i = 0; i <= max_length - strlen(ms[index]->size); i++)
			strcat(output_buf, " ");
		return;
	}

	//****** arrange space before adding nlink ******//	
	else if (!strcmp(std, "nlink")) {
		for (int i = 0; i < entry_num; i++)
		{
			if (strlen(ms[i]->nlink) > max_length)
				max_length = strlen(ms[i]->nlink);
		}

		for (int i = 0; i <= max_length - strlen(ms[index]->nlink); i++)
			strcat(output_buf, " ");
		return;
	}

	//****** arrange space before adding uid ******//	
	else if (!strcmp(std, "uid")) {
		for (int i = 0; i < entry_num; i++)
		{
			if (strlen(ms[i]->uid) > max_length)
				max_length = strlen(ms[i]->uid);
		}
		for (int i = 0; i <= max_length - strlen(ms[index]->uid); i++)
			strcat(output_buf, " ");
		return;
	}

	//****** arrange space before adding gid ******//	
	else if (!strcmp(std, "gid")) {
		for (int i = 0; i < entry_num; i++)
		{
			if (strlen(ms[i]->gid) > max_length)
				max_length = strlen(ms[i]->gid);
		}
		for (int i = 0; i <= max_length - strlen(ms[index]->gid); i++)
			strcat(output_buf, " ");
		return;

	}

}

int string_value(char* arr1, char* arr2)
{
	//////////////////////////////////////////////////////////////
	// string_value      					   					//
	// =========================================================// 
	// Input: arr1, arr2-> operands to compare		    		//	
	// OUtput: if arr1 is longer, return 1			    		//
	//		   if arr2 is longer or same, return -1	    		//
	// Purpose: To arrange output_buf by ascending ascii order  //
	//////////////////////////////////////////////////////////////


//*********** shorter string's length will be value *************//
	int value = 0;
	if (strlen(arr1) > strlen(arr2))
		value = strlen(arr2);
	else
		value = strlen(arr1);

	for (int i = 0; i < value; i++)
	{
		if (arr1[i] > arr2[i])
			return 1;
		else if (arr1[i] == arr2[i]) {
			if (i == value - 1)
			{
				if (strlen(arr1) > strlen(arr2))//if ascii value is same until value-1, compare string length and return
					return 1;
				else
					return -1;
			}
		}

		else return -1;

	}
}

void sort_ASCII(my_stat** ms, int entry_num) {
	//////////////////////////////////////////////////////////////
	// sort_ASCII	     					    				//
	// =========================================================// 
	// Input: ms-> sorting object, entry_num->total entry number//	
	// OUtput: void						   						//
	// Purpose:  arrange file data by ascending ascii order	    //
	//////////////////////////////////////////////////////////////


	my_stat* temp;

//******** Use bubble sort algorithm ********//
	for (int i = entry_num - 1; i > 0; i--) {
			for (int j = 0; j < i; j++) {			
			if (string_value(ms[j]->name, ms[j + 1]->name) == 1) {
				temp = ms[j];
				ms[j] = ms[j + 1];
				ms[j + 1] = temp;
			}
		}
	}

/* when arranged in ascii order, ./ and ../ order is changed so rechange */
	temp = ms[0];
	ms[0] = ms[1];
	ms[1] = temp;
}
void print_mystat(my_stat** ms, int entry_num, int aflag, char* result_buff) {
	//////////////////////////////////////////////////////////////
	// print_mystat	     					    				//
	// =========================================================// 
	// Input: ms-> printing object				   				//
	//		  aflag -> check if a option		    			//
	//		  entry_num-> total entry number	    			//
	//		  result_buff-> buffer to save results	    		//
	// OUtput: void						    					//
	// Purpose:  to write output_buf in stdout buffer	    	//
	//////////////////////////////////////////////////////////////
		
	char output_buf[1024] = { 0 };
	char itoa[30] = { 0 };


//************ fill output_buf with data and space_control each time *************//
	for (int i = 0; i < entry_num; i++) {
		if ((ms[i]->name[0] == '.') && !aflag)//if aflag is 0, not printing hidden files
			continue;
		itoa[0] = '\0';
		strcpy(output_buf, ms[i]->permission);

		space_control(ms, entry_num, "nlink", i, output_buf);
		strcat(output_buf, ms[i]->nlink);

		space_control(ms, entry_num, "uid", i, output_buf);
		strcat(output_buf, ms[i]->uid);

		space_control(ms, entry_num, "gid", i, output_buf);
		strcat(output_buf, ms[i]->gid);

		space_control(ms, entry_num, "size", i, output_buf);
		strcat(output_buf, ms[i]->size);

		strcat(output_buf, " ");
		strcat(output_buf, ms[i]->month);

//************* arrange space directly of day, hour, minute, name ************//
		strcat(output_buf, " ");
		if (ms[i]->day < 10)
			sprintf(itoa, " %d", ms[i]->day);
		else
			sprintf(itoa, "%d", ms[i]->day);
		strcat(output_buf, itoa);
		strcat(output_buf, " ");
		if (ms[i]->hour < 10)
			sprintf(itoa, "0%d", ms[i]->hour);
		else
			sprintf(itoa, "%d", ms[i]->hour);
		strcat(output_buf, itoa);
		strcat(output_buf, ":");
		if (ms[i]->minute < 10)
			sprintf(itoa, "0%d", ms[i]->minute);
		else
			sprintf(itoa, "%d", ms[i]->minute);
		strcat(output_buf, itoa);
		strcat(output_buf, " ");
		strcat(output_buf, ms[i]->name);
		strcat(output_buf, "\n");

        strcat(result_buff, output_buf);    	
	}
}

void NLST(char** argv, int argc, char* result_buff) {
	//////////////////////////////////////////////////////////////
	// NLST			     				   						//
	// =========================================================// 
	// Input: argv -> parsing objects 			    			//
	//		  argc -> size of argv			    				//
	//		  result_buff -> buffer to save results			    //
	// OUtput: void						    					//
	// Purpose:  NLST implementation			    			//
	//////////////////////////////////////////////////////////////
	int c;
	int aflag = 0;
	int lflag = 0;
	DIR* dir;
	struct dirent* dp;
	struct my_stat** mystat_buf;
	struct stat buf;
	struct stat buf_temp;
	char pathname[128] = { 0 };	
	int mystat_bufnum = 0;
	opterr = 0;
	optind = 0;
/********** parsing command by option using getopt ***************/
	while ((c = getopt(argc, argv, "al")) != -1)
	{
		switch (c)//option case
		{
		case 'a'://set aflag
			aflag = 1;
			break;
		case 'l':// set lflag
			lflag = 1;
			break;

		default:
			//printf("Not a given option: %c\n", optopt);//exception handling
			break;

		}
	}

	
	int init_pathlength = 0;
	int entry_stage = 0;

	if (optind + 1 == argc)//argument 0
		strcpy(pathname, ".");
	else if (strncmp("/", argv[optind + 1], 1) == 0)//argument 1, absolute path
	{
		strcpy(pathname, argv[optind + 1]);		
	}
	else//argument 1, relative path
	{
		strcpy(pathname, "./");
		strcat(pathname, argv[optind + 1]);		
	}	

	if (lstat(pathname, &buf_temp))//write file data in buf_temp
	{
        strcpy(result_buff,"not existing pathname\n");		
		return;
	}

	strcat(pathname, "/");
	init_pathlength = strlen(pathname);

/********** check if file type is directory ***********/
	if (S_ISDIR(buf_temp.st_mode)) {
		if ((dir = opendir(pathname)) == NULL)//open directory stream
		{
             strcpy(result_buff, "opendir failed\n");			
			return;
		}

		while ((dp = readdir(dir)) != 0)//calcuating total entry counts
			mystat_bufnum++;

		rewinddir(dir);

		/**************	mystat_buf memory allocation	***************/
		mystat_buf = (my_stat**)malloc(sizeof(my_stat*) * mystat_bufnum);
		for (int i = 0; i < mystat_bufnum; i++)
			mystat_buf[i] = (my_stat*)malloc(sizeof(my_stat));

		/*******read directory entry and write file data in each mystat_buf*******/
		while ((dp = readdir(dir)) != 0) {
			pathname[init_pathlength] = '\0';
			strcat(pathname, dp->d_name);
			lstat(pathname, &buf);
			strcpy(mystat_buf[entry_stage]->name, dp->d_name);
			write_mystat(mystat_buf[entry_stage], &buf);
			check_mode(&buf, mystat_buf[entry_stage]);
			entry_stage++;
		}
		sort_ASCII(mystat_buf, mystat_bufnum);//arrange mystat_buf in ascii ascending order

		if (lflag)//if lflag is on, ls-l
		{
			print_mystat(mystat_buf, mystat_bufnum, aflag, result_buff);//ls -l implementation related to aflag
			closedir(dir);
			return;
		}

		else//if  lflag is off, just print filename
		{
			int j = 0;
			for (int i = 0; i < mystat_bufnum; i++)
			{
				if ((mystat_buf[i]->name[0] == '.') && !aflag)//if aflag is off, not print hidden files
					continue;
                strcat(result_buff, mystat_buf[i]->name);

				if ((j % 5 == 4) && (i != mystat_bufnum - 1))//insert enter when print 5 times
					strcat(result_buff, "\n");
                  
				else
                    strcat(result_buff, " ");
					
				j++;
			}
            strcat(result_buff, "\n");		

			/*****	memory free	 ****/
			for (int i = 0; i < mystat_bufnum; i++)
				free(mystat_buf[i]);
			free(mystat_buf);
			closedir(dir);
			return;
		}


	}

	else {//if file type is not directory
		char* dummy;
		char filename[128];

		/* we must parse filename only, removing path*/
		strcpy(filename, strtok(pathname, "/"));
		while ((dummy = strtok(NULL, "/")) != NULL)
			strcpy(filename, dummy);


		if (lflag)//if lflag is on, do it like directory case only one time
		{
			mystat_buf = (my_stat**)malloc(sizeof(my_stat*));
			mystat_buf[0] = (my_stat*)malloc(sizeof(my_stat));
			write_mystat(mystat_buf[0], &buf_temp);
			strcpy(mystat_buf[0]->name, filename);
			check_mode(&buf_temp, mystat_buf[0]);
			print_mystat(mystat_buf, 1, 0, result_buff);
			free(mystat_buf[0]);
			free(mystat_buf);

		}
		else//if lflag is off, just print filename only
		{
			strcat(filename, "\n");
            strcpy(result_buff, filename);			
		}
	}

}


char oldpath[BUF_SIZE]={0};
char newpath[BUF_SIZE]={0};
static char transfer_mode = 'I';
int cmd_process(char* buff, char* result_buff, int clientfd, int datafd)
{
    //////////////////////////////////////////////////////////////
    // cmd_process  		   									//
    // =========================================================// 
    // Input: buff-> command from client                  		//
    //        result_buff-> buffer to save result               //
    // OUtput: -1 error, 0 successs     						//
    // Purpose: command execution                     			//
    //////////////////////////////////////////////////////////////
	
    if(buff==NULL)
        return -1;

	char port_buff[BUF_SIZE]={0}; //size 256
    char* error_msg;
	char** argv;
	int argc = 1;
	char input_buf[1024];	
	opterr = 0;	

	buff[strlen(buff)-1] = '\0';//string set   

	/*	command error exception	 */
	if (strcmp("comx",buff) == 0) {
        strcpy(result_buff,"not existing command\n");	
		return -1;
	}

	/*	option error exception  */
	if (strcmp("optx", buff) == 0) {
	     strcpy(result_buff,"not existing option\n");	
		return -1;
	}

	/* let's mimic argc, argv in earnest */
	char arr1[1024] = { 0 };
	char arr2[1024] = { 0 };	
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


	if (!strcmp(argv[1], "NLST")) //NLST
	{	
		NLST(argv, argc, result_buff);
		write(datafd, result_buff, strlen(result_buff+1));	
		return 0;		
	}

	if (strcmp(argv[1], "LIST") == 0) //LIST
	{
		/* make mini argv like NLST */
		if (argc > 2) {//if pathname exists
			char* arr[5];
			arr[0] = "./srv.c";
			arr[1] = "NLST";
			arr[2] = "-a";
			arr[3] = "-l";
			arr[4] = argv[2];
			NLST(arr, 5, result_buff);//same with NLST -a -l
		}
		else {//if no pathname
			char* arr[4];
			arr[0] = "./srv.c";
			arr[1] = "NLST";
			arr[2] = "-a";
			arr[3] = "-l";
			NLST(arr, 4, result_buff);//same with NLST -a -l
		}

		write(datafd, result_buff, strlen(result_buff+1));
		return 0;		
			   		
	}

    if(!strcmp(argv[1], "QUIT")){
        if (argc > 2) {//error handling
			strcpy(result_buff, "Error! must be non argument\n");					
		}
		else {
			strcpy(result_buff, "QUIT");			
		}       
    }
   
   if (strcmp(argv[1], "PWD") == 0) //pwd
	{	
			char cwd[1024];
			getcwd(cwd, 1024);
			strcat(cwd, "\n");
			strcpy(result_buff, "257 ");
			strcat(result_buff, cwd);
			write(clientfd, result_buff, strlen(result_buff)+1);
			write(STDOUT_FILENO, result_buff, strlen(result_buff)+1);			
	}

	if ((strcmp(argv[1], "CWD") == 0))//cd
	{
			if (!chdir(argv[2])) {
				strcpy(result_buff, "250 CWD command succeeds.\n");
				write(clientfd, result_buff, strlen(result_buff)+1);
				write(STDOUT_FILENO, result_buff, strlen(result_buff)+1);
			}
			else {//error handling
				strcpy(result_buff, "550 ");
				strcat(result_buff, argv[2]);
				strcat(result_buff, ": Can't find such file or directory.\n");
				write(clientfd, result_buff, strlen(result_buff)+1);
				write(STDOUT_FILENO, result_buff, strlen(result_buff)+1);								
			}		
	}

	if ((strcmp(argv[1], "CDUP") == 0))//cdup
	{		
		if (argc > 2) {//error handling
			strcpy(result_buff, "550 ");
			strcat(result_buff, argv[2]);
			strcat(result_buff, ": Can't find such file or directory.\n");
			write(clientfd, result_buff, strlen(result_buff)+1);
			write(STDOUT_FILENO, result_buff, strlen(result_buff)+1);
		}
		
			if (!chdir("..")) {
				strcpy(result_buff, "250 CDUP command succeeds.\n");
				write(clientfd, result_buff, strlen(result_buff)+1);
				write(STDOUT_FILENO, result_buff, strlen(result_buff)+1);							
			}
			
		}
		
	

	if (strcmp(argv[1], "MKD") == 0) //mkdir
	{
		
			for (int i = 2; i < argc; i++) {
				if (!mkdir(argv[i], 0775)){
					strcpy(result_buff, "250 MKD command succeeds.\n");
					write(clientfd, result_buff, strlen(result_buff)+1);
					write(STDOUT_FILENO, result_buff, strlen(result_buff)+1);
				}
				else//error handling
				{
					strcpy(result_buff, "550 ");
					strcat(result_buff, argv[2]);
					strcat(result_buff, ": Can't create directory.\n");
					write(clientfd, result_buff, strlen(result_buff)+1);
					write(STDOUT_FILENO, result_buff, strlen(result_buff)+1);								
				}
			}			
	
	}

	if (strcmp(argv[1], "RMD") == 0) //rmdir
	{		
		
			for (int i = 2; i < argc; i++) {
				if (!rmdir(argv[i])){
					strcpy(result_buff, "250 RMD command succeeds.\n");
					write(clientfd, result_buff, strlen(result_buff)+1);
					write(STDOUT_FILENO, result_buff, strlen(result_buff)+1);
				}					
				else//error handling
				{
					strcpy(result_buff, "550 ");
					strcat(result_buff, argv[2]);
					strcat(result_buff, ": Can't remove directory.\n");
					write(clientfd, result_buff, strlen(result_buff)+1);
					write(STDOUT_FILENO, result_buff, strlen(result_buff)+1);							
				}
			}			
		

	}

	if (strcmp(argv[1], "DELE") == 0) //remove
	{		
			for (int i = 2; i < argc; i++) {
				if (!unlink(argv[i])){
					strcpy(result_buff, "250 DELE command succeeds.\n");
					write(clientfd, result_buff, strlen(result_buff)+1);
					write(STDOUT_FILENO, result_buff, strlen(result_buff)+1);
				}
				else//error handling
				{
					strcpy(result_buff, "550 ");
					strcat(result_buff, argv[i]);
					strcat(result_buff,": Can't find such file or directory.\n");
					write(clientfd, result_buff, strlen(result_buff)+1);
					write(STDOUT_FILENO, result_buff, strlen(result_buff)+1);								
				}
			}		
	}

	if (strcmp(argv[1], "RNFR") == 0) //rename FROM
	{
			DIR *dp = opendir("./");
			struct dirent *entry=0;
			while((entry=readdir(dp))!=0)
				{
					if(!strcmp(entry->d_name, argv[2])){
						write(clientfd, "350 File exists, ready to rename.\n",35);
						write(STDOUT_FILENO, "350 File exists, ready to rename.\n",35);
						strcpy(oldpath, entry->d_name);
						break;
					}
				}
			if(entry==0){
				strcpy(result_buff, "550 ");
				strcat(result_buff, argv[2]);
				strcat(result_buff, ": can't be renamed.\n");
				write(clientfd, result_buff, strlen(result_buff)+1);
				write(STDOUT_FILENO, result_buff, strlen(result_buff)+1);
			}	
	}	
				
	if (strcmp(argv[1], "RNTO") == 0){ //rename TO	
			strcpy(newpath, argv[2]);	
			if (!rename(oldpath, newpath))
			{
				write(clientfd, "250 RNTO command succeeds.\n",28);	
				write(STDOUT_FILENO, "250 RNTO command succeeds.\n",28);			
			}
			else//error handling
			{
				strcpy(result_buff, "550 ");
				strcat(result_buff, argv[2]);
				strcat(result_buff, ": can't be renamed.\n");
				write(clientfd, result_buff, strlen(result_buff)+1);
				write(STDOUT_FILENO, result_buff, strlen(result_buff)+1);							
			}
		
	}	

	if (strcmp(argv[1], "RETR") == 0) //get
	{
		unsigned char newfd;		
		char c[1];
		/* RETR 명령어(get) */
		newfd = open(argv[2], O_RDONLY);
		if(transfer_mode == 'I')//BIN mode
		{
			while(read((unsigned char)newfd, c, 1))//EOF in linux
		{						
			write(datafd, c, 1);			
		}	
		c[0] = EOF;
			write(datafd, c, 1);
	}
		else if(transfer_mode =='A'){//ascii mode
		char ascii_buf[2];
		ascii_buf[0] = 13;
		ascii_buf[1] = 10;
			while(read(newfd, c, 1)>0)//EOF in linux
		{
			if(c[0] == 10)
				write(datafd, ascii_buf, 2);
			else
				write(datafd, c, 1);			
		}
		c[0] = EOF;
			write(datafd, c, 1);//WINDOW EOF
		}
		else{
			close(newfd);
			close(datafd);
			return 1;
		}
		close(newfd);
		
		return 0;
	}

	if (strcmp(argv[1], "STOR") == 0) //rename
	{
		/* STOR(put) 명령어 */
		FILE* newfd;
		int c[1];		
		newfd = fopen(argv[2], "wb");
		if(transfer_mode == 'I')//BIN mode
		{
			while(read(datafd, c, 1))//EOF in linux
		{
			if(c[0]==EOF)
			 break;
			fputc(c[0], newfd);			
		}		
			}
		else if(transfer_mode == 'A'){//ascii mode
			while(read(datafd, c, 1))//EOF in window
		{
			if(c[0]==EOF)
				break;
			else if(c[0]==13)//revise EOL
				fputc(' ', newfd);
			else
				fputc(c[0], newfd);
		}	
	}
		else{
			fclose(newfd);
			close(datafd);
			return -1;
		}
		fclose(newfd);		
		return 0;
	}

    if (strcmp(argv[1], "TYPE") == 0) //rename
	{
		/* TYPE 명령어 */
		if(!strcmp(argv[2], "I")){
			transfer_mode = 'I';
			write(clientfd, "201 Type set to I\n",19);
			write(STDOUT_FILENO,  "201 Type set to I\n", 19);
		}
		else if(!strcmp(argv[2], "A")){
			transfer_mode = 'A';
			write(clientfd, "201 Type set to A\n",19);
			write(STDOUT_FILENO,  "201 Type set to A\n", 19);
		}
		else{
			write(clientfd, "502 Type doesn't set\n",22);
			write(STDOUT_FILENO, "502 Type doesn't set\n",22);	
		}	
		}
	

	/* memory free */	
		free(argv);
		return 0;
}

int client_info(struct sockaddr_in *client_addr)
{
    //////////////////////////////////////////////////////////////
	// client_info			                				    //
	// =========================================================// 
	// Input: &client_addr -> sockaddr_in of client             //
	// OUtput: -1 error, 0 successs     					    //
	// Purpose: print client IP, client port        		    //
	//////////////////////////////////////////////////////////////
    char buf[1024];
    char* addr;
    addr = inet_ntoa(client_addr->sin_addr);//convert ip address to dotted decimal
    sprintf(buf, "%d", ntohs(client_addr->sin_port));//port number
    if(strcmp("127.0.0.1", addr)!=0) return -1;
    if(client_addr->sin_port < 0) return -1;
    write(STDOUT_FILENO, "==========Client info==========\nclient IP: ",44);
    write(STDOUT_FILENO, addr, strlen(addr));
    write(STDOUT_FILENO, "\n\n\nclient port: ", strlen("\n\n\nclient port: "));
    write(STDOUT_FILENO, buf, strlen(buf));
    write(STDOUT_FILENO, "\n===============================\n", 34);
    return 0;
}

int user_match(char *user, char *passwd){
    //////////////////////////////////////////////////////////////
	// user_match			                				    //
	// =========================================================// 
	// Input: user, passwd -> info about user                   //
	// OUtput: 0 error, 1 successs     			    		    //
	// Purpose: check if private info is matching     		    //
	//////////////////////////////////////////////////////////////
    FILE *fp;
    struct passwd *pw;

    fp = fopen("passwd", "r");
     /* if successed return 1, else return 0*/

	if(passwd == NULL){//just id check
		while((pw = fgetpwent(fp)) != NULL){
        if(!strcmp(user, pw->pw_name)){//if id is found
				return 1;}		
		else
		{
			continue;
		}		
	} 
	}

    while((pw = fgetpwent(fp)) != NULL){
        if(!strcmp(user, pw->pw_name)){//if id is found
            if(!strcmp(passwd, pw->pw_passwd))//check password
                return 1;
            else
                continue;
        }
    }

    return 0;   
}

int log_auth(int clientfd){
     //////////////////////////////////////////////////////////////
	// log_auth 			                				    //
	// =========================================================// 
	// Input: clientfd -> client socketscriptor                 //
	// OUtput: 0 error, 1 successs     			    		    //
	// Purpose: censoring login                      		    //
	//////////////////////////////////////////////////////////////
    char user[BUF_SIZE], passwd[BUF_SIZE];
	char uid[BUF_SIZE]={0};
	char pw[BUF_SIZE]={0};
    int n, count = 1;
	char message[BUF_SIZE]={0};
  
    while(1)
    {		
        read(clientfd, user,BUF_SIZE);    
		if(!strcmp(strtok(user, " "), "USER"));
			strcpy(uid, strtok(NULL, " "));
		 printf("** User is trying to log-in (%d/3)\n", count);
		if(user_match(uid, NULL)==1){
			strcpy(message, "331 Password is required for ");		
			strcat(message,uid);
			strcat(message, "\n");
			write(clientfd, message, strlen(message)+1);
			write(STDOUT_FILENO, message, strlen(message)+1);
		}
		else{
			if(count <3){
			write(clientfd, "430 Invalid username of password\n", 34);	
			write(STDOUT_FILENO, "430 Invalid username of password\n", 34);	
			}			
			 if(count >=3){  //if failed 3 times, disconnection message and return 0            
               write(clientfd, "530 Failed to log-in\n", 22);
			   write(STDOUT_FILENO, "530 Failed to log-in\n", 22);
               return 0;
            }
			count++;
			continue;
		}

        read(clientfd, passwd,BUF_SIZE);
		if(!strcmp(strtok(passwd, " "), "PASS"));
			strcpy(pw, strtok(NULL, " "));		
        /* after getting client information, send OK */  
       
        if((n = user_match(uid, pw)) == 1){  //check id, pw matching         
            strcpy(message, "230 User ");		
			strcat(message,uid);
			strcat(message, " logged in.\n");		
			write(clientfd, message, strlen(message)+1);
			write(STDOUT_FILENO, message, strlen(message)+1);
           break;
        }

        else if(n ==0){
            printf("**Log-in failed **\n");
            if(count >=3){  //if failed 3 times, disconnection message and return 0            
               write(clientfd, "530 Failed to log-in\n", 22);
			   write(STDOUT_FILENO, "530 Failed to log-in\n", 22);
               return 0;
            }
           write(clientfd, "430 Invalid username or password\n", 34);//invalid message send
		   write(STDOUT_FILENO, "430 Invalid username or password\n", 34);
           count++;
           continue;
        }
    }

    return 1;
}

int main (int argc, char **argv)
{
    //////////////////////////////////////////////////////////////
	// main     			                				    //
	// =========================================================// 
	// Input: argc, argv -> port number                         //
	// OUtput: -1 error, 0 successs     					    //
	// Purpose: server main                           		    //
	//////////////////////////////////////////////////////////////
    struct sockaddr_in server_addr, client_control, client_data;	
    int controlfd, datafd, clientfd;
    int length;
    char *host_ip;//buffer to store client's IP, port number
    char temp[25]={0};
    unsigned short port_num;
    char buff[BUF_SIZE]={0}; //size 256
    char rcv_buff[BUF_SIZE]={0}; //size 256   
    char result_buff[RESULT_BUFF]={0};//size 50000
    int len, n;
    char *addr;
	char ip_string[30];
    char *addr_token[4];
    char *check_token[4];
    int ipcheck=0;
	FILE* fp_checkIP;

	fp_checkIP = fopen("access.txt", "rt");
     
    /* open control connection socket */
    if((controlfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
      write(STDERR_FILENO, "Server: can't open\n", strlen("Server: can't open\n"));
        return -1;
    }
   

    /* initialize server_addr */
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = PF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    /* bind control socket*/
    if(bind(controlfd, (struct sockaddr *)&server_addr,sizeof(server_addr)) <0){
        write(STDERR_FILENO, "Server: can't bind\n", strlen("Server: can't bind\n"));
        return -1;
    }

    listen(controlfd, 5);//listen
      
    len = sizeof(client_control);
    clientfd = accept(controlfd, (struct sockaddr*)&client_control, &len);//accept

	addr = inet_ntoa((&client_control)->sin_addr);

    if(client_info(&client_control) < 0) /*display client ip and port */
        write(STDERR_FILENO, "client_info() err!!\n", strlen("client_info() err!!\n"));

	 addr_token[0] = strtok(addr,".");
        for(int i=1; i<4; i++)        
            addr_token[i] = strtok(NULL,".");

        /* check access.txt if it is blocked IP */
        while(fgets(ip_string, 30, fp_checkIP)!=NULL){	 
	    ip_string[strlen(ip_string)-1]='\0';
            check_token[0] = strtok(ip_string,".");
            for(int i=1; i<4; i++)        
            check_token[i] = strtok(NULL,".");

            for(int i=0; i<4; i++){
                if(strcmp(check_token[i],"*")!=0){  
                    if(strcmp(check_token[i], addr_token[i])!=0)                        
                        break;
                                 }                
                if(i==3)
                    ipcheck = 1;//it means this ip is not blocked
            }	      
            if(ipcheck == 1)
                break;
            else 
                continue;
        }

    /* sending message of ip blocking result */
        if(ipcheck ==1){
            write(clientfd, "220 ACCEPTED! FTP server ready\n", 32);
			write(STDOUT_FILENO, "220 ACCEPTED! FTP server ready\n", 32);
			 }
        else{
            write(clientfd, "431 This client can't access. Close the session\n",49);
            write(STDOUT_FILENO, "431 This client can't access. Close the session\n", 49);
            close(clientfd);}
           
    /* start user matching */
        if(log_auth(clientfd) == 0){
            printf("** Fail to log-in **\n");
            close(clientfd);           
            }
        printf("** Success to log-in **\n");
      	
	while(read(clientfd, buff, BUF_SIZE) > 0)//get msg from client socket
				{
					if (strncmp(buff, "PORT", 4) == 0) //PORT message
				{
					write(STDOUT_FILENO, buff, strlen(buff)+1);
					write(STDOUT_FILENO, "\n", 2);				
					strtok(buff, " ");  
					char temp[BUF_SIZE]; 
					strcpy(temp, strtok(NULL, ","));   
					for(int i=0; i<3; i++){
						strcat(temp, ".");
						strcat(temp, strtok(NULL, ","));        
					}

					port_num = atoi(strtok(NULL, ","))*256 + atoi(strtok(NULL,","));//calculate port number  

					/* initialize client_data */
					bzero((char *)&client_data, sizeof(client_data));
					client_data.sin_family = PF_INET;
					client_data.sin_addr.s_addr = inet_addr(temp);
					client_data.sin_port = htons(port_num);     
					
					/* open data connection socket */
					if((datafd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
					write(STDERR_FILENO, "Server: can't open\n", strlen("Server: can't open\n"));
						return -1;
					}
					/* connecting datafd to client */
					if(connect(datafd, (struct sockaddr*)&client_data, sizeof(client_data))<0){
						write(clientfd, "550 Failed to access\n", 22);//send success message
						write(STDOUT_FILENO, "550 Failed to access\n", 22);
						return -1;
					}   

					write(clientfd, "200 Port command performmed successfully\n", 42);//send success message
					write(STDOUT_FILENO, "200 Port command performmed successfully\n", 42);

					read(clientfd, rcv_buff, BUF_SIZE);
					if(!strncmp(rcv_buff, "NLST", 4)||!strncmp(rcv_buff, "LIST", 4))
					{
					write(clientfd, "150 Opening data connection...\n", 32);
					write(STDOUT_FILENO, "150 Opening data connection...\n", 32);
					}
					else{//get or put
					if(transfer_mode == 'I'){
					write(clientfd, "150 Opening binary mode data connection...\n", 44);
					write(STDOUT_FILENO, "150 Opening binary mode data connection...\n", 44);}
					else{
					write(clientfd, "150 Opening ascii mode data connection...\n", 43);
					write(STDOUT_FILENO, "150 Opening ascii mode data connection...\n", 43);
					}
					}
				
					if(cmd_process(rcv_buff, result_buff, clientfd, datafd)!=0){//command execute and result(ls command)
						write(clientfd, "550 Failed transmission.\n", 26);
						write(STDOUT_FILENO, "550 Failed transmission.\n", 26);
					}
					else{
					write(clientfd, "226 Result is sent successfully.\n", 34);//send success message
					write(STDOUT_FILENO, "226 Result is sent successfully.\n", 34);
					}
					close(datafd);
					memset(buff, 0, sizeof(buff));//buff reset
					memset(result_buff, 0, sizeof(result_buff));//buff reset
					continue;
					}
					if(!strncmp(buff, "QUIT", 4)){
						write(clientfd, "221 Goodbye.\n", 14);
						write(STDOUT_FILENO, "221 Goodbye.\n", 14);
						close(clientfd);//close server socket	
						break;					
					}				
					cmd_process(buff, result_buff, clientfd, datafd);//command execute and result							
				}			  
       
       
    }    


