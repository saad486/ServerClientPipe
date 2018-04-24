#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include<ctype.h>

#define START "Welcome : Enter the commands to your liking\n"
#define DIVIDE "Error : division by zero\n"
#define MESSAGE "Invalid command, type help\n"
#define NOELEMENTS "No elements in the list\n"
#define HELP "=========================\nWelcome to our program guide:\nType add number1 number2 ...\nType sub number1 number2 ...\nType mult number1 number2 ...\nType div number1 number2 ...\nType run argument1 ...\nType list : To view currenly running processes and list[ all] for all processes executed\nType kill PID, to kill an actively running prcocess\nType exit : to exit our program\nNote: All the operations are not case-sensitve, example: Add, aDD, ADd are valid\nMinimun number of inputs for arthimetic operation is Two\n=========================\n"

int checkDigit(char * token);

struct Node{
	int id;
	int active;
	char name[15];
	

};

//Node array 
struct Node * processArray[30];//if typed exit then it will be freed
int processCounter = 0;

	int pd[2];
	int pd1[2];


void printArray()
{	
	int i = 0;
	char buff[2000];
	int count = 0;
	for(;i<processCounter;i++)
		{
			count += sprintf(&buff[count],"Process ID = %d , Active = %d, Process name = %s\n",processArray[i]-> id,processArray[i]->active,processArray[i]->name);
		}
		write(pd1[1],buff,count);
}
void printActiveArray()
{
	int i = 0;
	int count=0;
	int activeProcesses = 0;
		char buff[1000];
	
	for(;i<processCounter;i++)
		{
			if(processArray[i]->active == 1) 
			  {
			  	count += sprintf(&buff[count],"Process ID = %d, Process name = %s\n",processArray[i]->id,processArray[i]->name);
			  	activeProcesses++;
			  }
			 buff[count] = '\0';
			 count++;
		}
		
	if( activeProcesses > 0)	
		write(pd1[1],buff,count);
	else write(pd1[1],"Currently no active processes listed\n",strlen("Currently no active processes listed\n"));	
}
void inactiveProcess(int search)
{
	int i;
	for(i = 0; i<processCounter;i++)
	{
		if(processArray[i]->id == search)
			{ processArray[i]->active = 0;
			  break;
			}		
	}

}

static void signalhandler(int signo)
{
	int status;
	int pid;
	
	
		pid = waitpid(-1,&status,0);
		
		
		inactiveProcess(pid);
	
}
char* methodArithmetic(char * token, char * buff)
{
	char * identifier = token;
	int sum = 0;
	int firstNumber = 0;
	int divideZero = 0;
	int check = 0;
	int arguments = 0;
	
	if((token = strtok(NULL," \n")) == NULL)
		{
			strcpy(buff,MESSAGE);
			return buff;
		}
	
	while(token != NULL)
	{
		check = checkDigit(token);
					if(check == 0)
						break;
			
						
		if(strcasecmp("add",identifier) == 0)
			{
				sum += atoi(token);
			}
		else if(strcasecmp("sub",identifier) == 0)
			{	
			
				if(firstNumber == 0)
					{	sum = atoi(token);
						firstNumber = 1;
					}
				else sum -= atoi(token);
			}
		else if(strcasecmp("mult",identifier) == 0)
			{
				if(firstNumber == 0)
					{	sum = atoi(token);
						firstNumber = 1;
					}
				else sum *= atoi(token);
			}
		else if(strcasecmp("div",identifier) == 0)
			{
				if(firstNumber == 0)
					{	sum = atoi(token);
						firstNumber = 1;
					}
				else 
					{ 
						if( atoi(token) == 0)
								{	divideZero = 1;
									break;
								}
						sum /= atoi(token);
					}	
						
			}
			arguments++;
			token = strtok(NULL," \n");
		}
		
	
	if(check == 0)
		strcpy(buff,"Invalid arguments\n");
	
	else if(divideZero == 1)
				strcpy(buff,DIVIDE);
				
			
	else if ( arguments == 1)
		strcpy(buff,"Too few arguments! Atleast two required\n");
			
	else
		 	{	int count = sprintf(buff,"%d",sum);
		 		buff[count] = '\n';
		 		count++;
		 		buff[count] = '\0';
		 	}
		
	return buff; 	

}	
int checkDigit(char * token)
{
	int i = 0;
	char buff[20];
	strcpy(buff, token);
	int c = 0;
	
	while(i < strlen(token))
	{
		 c  = isdigit(buff[i]);
		 
			if(c == 0)
				break;
				
				i++;
	
	}
	if(c  == 0)
		return 0;
	else return 1;
	


}


int main()
{
	
	
	int count = write(STDOUT_FILENO, START, strlen(START));
	

		if(count == -1)
		 {	
		 	perror("error: ");
		 	exit(EXIT_FAILURE);
		 }
	
	int pipeCheck = pipe(pd);
	
		if(pipeCheck == -1)
			{
				perror("error");
				exit(1);	
			}
	pipeCheck = pipe(pd1);
			
			if(pipeCheck == -1)
			{
				perror("error");
				exit(1);	
			}
	
	
	int pid = fork();
		
		if(pid == -1)
			{
				perror("error");
				exit(1);
			}
			
	if(signal(SIGCHLD,signalhandler) == SIG_ERR)
	{
		perror("error");
	}
	
		
//############Server####################	
	

if(pid == 0)
{	
	
	close(pd[1]);
	
	close(pd1[0]);
		
for(;;)		
{
	int pipeClose;
		
	char buff[500];
		
	int rcount = read(pd[0],buff, 500);
		
		if( rcount == -1)
			{
					perror("error");
			}
		
				
	char * temp[20]; //array of pointers
		
	//boolean true false
	int doTokenize = 0;
	int skip = 0;
	int runProgram = 0;
	int quit = 0;
	int listHelp = 0;
	int counterTemp = 0;
			
	//
		      	
	int countToken = 0;//token to be used later to free space from heap (temp)
	char * token;
	char * buffArithmetic = (char *)malloc(50 * sizeof(char));
	char * buffList = (char *)malloc(1000*sizeof(char));
				
		
		        

	if((rcount-1) == 0)
		token = "a";//garbage value
	else
		token = strtok(buff," ");
				
	//comparing cases
	if(strcasecmp("add",token) == 0)
	{
		char * tempBuff = methodArithmetic(token, buffArithmetic);
		
		int ccount = write(pd1[1], tempBuff, strlen(buffArithmetic));
		 			
			if(ccount == -1)
		 		perror("error");
		 	
		 	
	}
	else if(strcasecmp("sub",token) == 0)
	{
		
		char * tempBuff = methodArithmetic(token, buffArithmetic);
		
		int ccount = write(pd1[1], tempBuff, strlen(buffArithmetic));
		 			
			if(ccount == -1)
		 		perror("error");
		 		
		 	
	}
	else if(strcasecmp("mult",token) == 0)
	{
	
	char * tempBuff = methodArithmetic(token, buffArithmetic);
		
		int ccount = write(pd1[1], tempBuff, strlen(buffArithmetic));
		 			
			if(ccount == -1)
		 		perror("error");
		 		
			
	}
	else if(strcasecmp("div",token) == 0)
	{
	
	char * tempBuff = methodArithmetic(token, buffArithmetic);
		
		int ccount = write(pd1[1], tempBuff, strlen(buffArithmetic));
		 			
			if(ccount == -1)
		 		perror("error");
		 		
		
	}
	else if(strcasecmp("list",token) == 0)
	{
		if(processCounter > 0)
		{	
			token = strtok(NULL," \n");
			
			if(token == NULL)
					printActiveArray();
					
				
			else if(strcmp(token,"all") == 0)	//check why tokenizing first and checking caused the problem
				{	
					 if((token = strtok(NULL," \n")) == NULL)
						printArray();
						
					else write(pd1[1],"Invalid command\n",strlen("Invalid command\n"));
					}
	
		else write(pd1[1],"Invalid command\n",strlen("Invalid command\n"));
	
	}
	else write(pd1[1],"List is currently Empty\n",strlen("List is currently Empty\n"));
	}
	else if(strcasecmp("exit",token) == 0)
	{
		token = strtok(NULL," \n");
		
		if(token!=NULL)
			{
				write(pd1[1],"Invalid command\n",strlen("Invalid command\n"));
			}
		else {
			int count = kill(pid,SIGTERM);
				if(count == -1)
					perror("error");
					
					
		}
		
	}
	else if(strcasecmp("help",token) == 0)
	{
		token = strtok(NULL," \n");
		
		if(token == NULL)
			write(pd1[1],HELP,strlen(HELP));
		else write(pd1[1],MESSAGE,strlen(MESSAGE));		
	}
	else if(strcasecmp("kill",token) == 0)
	{
		
		token = strtok(NULL," \n");
		
		if(token==NULL)
			{
				write(pd1[1],"Invalid command\n",strlen("Invalid command\n"));
			}
		else {
				char * stoken = token;
				
				token = strtok(NULL," \n");
				
				if(token == NULL)
				{
					int pid = 0;
					int kcount = sscanf(stoken,"%d",&pid);
					
						int check = checkDigit(stoken);
					
						if(check == 0)
						{
							write(pd1[1],"Invalid arguments\n",strlen("Invalid arguments\n"));
						}
					
						else {
							kcount = kill(pid,SIGTERM);
							
									if(kcount == -1)
										write(pd1[1],"Invalid process ID\n",strlen("Invalid process ID\n"));
									else write(pd1[1],"Process Terminated\n",strlen("Process Terminated\n"));
					
						}
					}
					else write(pd1[1],"Invalid arguments\n",strlen("Invalid arguments\n"));	
		}
	
	
	}
	else if(strcasecmp("run",token) == 0)
		{	
			if((token = strtok(NULL," \n")) == NULL)
			  	write(pd1[1],MESSAGE,strlen(MESSAGE));
			else{  	
			
			int i = 0;
			while(token != NULL)
			{	if( i == 0)
				{	temp[i] = (char *)malloc(30*sizeof(char));
					strcpy(temp[i],token);
					i++;
				}
				temp[i] = (char *)malloc(30*sizeof(char));
				strcpy(temp[i],token);
				i++;
				token = strtok(NULL, " \n");
			}	
    		
		temp[i] = NULL;
		counterTemp = i;
		skip = 1;
		
		}
		
    }	

	else
		count = write(pd1[1], MESSAGE, strlen(MESSAGE));
			 
		
	//running process
	if(skip == 1)
	{	
		int status;
		
		int sd[2];
		
		

		
		int serverPipe = pipe(sd);
	
		if(serverPipe == -1 )
			{
				perror("error");
			}
				fcntl(sd[1],F_SETFD,FD_CLOEXEC);
		
		int serverPid = fork();
		 	if(serverPid == -1)
		 		perror("error");
		 		
		 
				
		if(serverPid == 0)
		{	
			//write(sd[1],"saad",4);
			count = execvp(temp[0],&temp[1]);
				if(count == -1)
					{	
						close(sd[0]);
						perror("error");
						write(sd[1],"salman",6);
						close(sd[1]);
						exit(99);
					}	
				
		}
		else if(serverPid > 0)
		{
			int waitChild = waitpid(serverPid, &status,WNOHANG);
			
			close(sd[1]);
			
			char tempbuff[50];
			
			int count = read(sd[0],tempbuff,50);

		
			
			if(count == 0)
				{
					
					processArray[processCounter] = (struct Node *)(malloc(50*sizeof(struct Node))); 
					processArray[processCounter] -> id = serverPid;
					strcpy(processArray[processCounter] -> name, temp[0]);
					processArray[processCounter] -> active = 1;
					processCounter++;
			  		write(pd1[1],"Executing...\n",strlen("Executing...\n"));
			  		
				}	
				
			else 
				{		printf("%d\n",count);
						write(pd1[1],"Invalid program execution\n",strlen("Invalid program execution\n"));
				}
		close(sd[0]);	
		}	
				
	}
	int j = 0;
	for(;j<counterTemp;j++)
			free(temp[j]);
			
			free(buffArithmetic);
		
}
	
}				


//############Client##########################
	
else if(pid > 0) 
{	
	close(pd[0]);
	
	close(pd1[1]);
	
	for(;;)
	 {	
		int pipeClose;
	
		char buff[500];
		
		count = read(STDIN_FILENO, buff, 500);
	
			if(count == -1)
			 {	
			 	perror("error");
			 	exit(EXIT_FAILURE);
			 }
			 
		buff[count-1] = '\0';
		
		int pipeCount = write(pd[1], buff, count);
		
			if(pipeCount == -1)
			 {	
			 	perror("error: dd");
			 	exit(EXIT_FAILURE);
			 }
		
			char resultBuff[1000];
			
			count = read(pd1[0], buff, 1000);
			
			if(count == -1)
			 {	
			 	perror("error: ");
			 	exit(EXIT_FAILURE);
			 }
			 
	  		 write(STDOUT_FILENO,buff,count);
	  		 
	  	}	 
	}
}


