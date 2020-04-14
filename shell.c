#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include<fcntl.h> 

#define CMD_MAX 100

//int fd[PATH_MAX];
//int fd_ctr=0;

char *getcwd(char *buf, size_t size);	//Built-in function to retrieve current working directory

char *getFilename(char *str, int i)		//Retrieve the next work in str starting from index i (delimited by ' ')
{
	int k=0;
	char *out=malloc(PATH_MAX);
	while(str[i]!='\0')
	{
		if(str[i]==' ')
			break;
		out[k++]=str[i++];
	}
	return out;
}

char* redirections(char in[])	//Check the input for 2>&1 or 1>filename or 2>filename
{
	int i=0,j=0;
	char *out=malloc(PATH_MAX);
	while(i<PATH_MAX && in[i]!='\0' && in[i+1]!='\0' && in[i+2]!='\0' && in[i+3]!='\0')
	{
		if(in[i]=='2' && in[i+1]=='>' && in[i+2]=='&' && in[i+3]=='1')	//2>&1
		{
			dup2(1,2);	//redirect2to1();
			i+=4;
		}
		else if(in[i]=='1' && in[i+1]=='>')		//1>filename
		{
			int k=0;
			char *file=malloc(PATH_MAX);
			i+=2;
			while(in[i]!='\0')
			{
				if(in[i]==' '||in[i]=='\n')
					break;
				file[k++]=in[i++];
			}
			//fd[fd_ctr++]=
			int fd=open(file,O_WRONLY|O_CREAT|O_TRUNC,0666);
			close(1);
			dup(fd);//[fd_ctr-1]);
		}
		else if(in[i]=='2' && in[i+1]=='>')		//2>filename
		{
			int k=0;
			char *file=malloc(PATH_MAX);
			i+=2;
			while(in[i]!='\0')
			{
				if(in[i]==' '||in[i]=='\n')
					break;
				file[k++]=in[i++];
			}
			//fd[fd_ctr++]=
			int fd=open(file,O_WRONLY|O_CREAT|O_TRUNC,0666);
			close(2);
			dup(fd);//[fd_ctr-1]);
		}
		else									//Only keep the remaining part of the command
			out[j++]=in[i++];
	}

	while(i<PATH_MAX && in[i]!='\0')
		out[j++]=in[i++];

	return out;
}

int parse_input(char *str, char **split)	//Parse the input and split into a list of tokens
{	
	int i=0;
	int j=0;
	int k=0;

	while(str[i]!='\0')
	{
		k=0;

		if(str[i]==' ' || str[i]=='\n')
		{
			i++;
		}
		
		if(str[i]=='|')
		{
			split[j++]=NULL;
			split[j][0]='|';
			split[j][1]='\0';
			i++;
			j++;
		}
		else if(str[i]=='<')
		{
			split[j++]=NULL;
			split[j][0]='<';
			split[j][1]='\0';
			i++;
			j++;
		}
		else if(str[i]=='>')
		{
			split[j++]=NULL;
			if(str[i+1]=='>')
			{
				split[j][0]='>';
				split[j][1]='>';
				split[j][2]='\0';
				i+=2;
			}
			else
			{
				split[j][0]='>';
				split[j][1]='\0';
				i++;
			}
			j++;
		}

		int read_something=0;

		while(str[i]!='\0' && str[i]!=' ' && str[i]!='\n' && str[i]!='|' && str[i]!='>' && str[i]!='<')
		{
			split[j][k++]=str[i++];
			split[j][k]='\0';
			read_something=1;
		}
		if(read_something)
			j++;
	}

	split[j]=NULL;

	return j+1;
}

int main()
{
	char *args[CMD_MAX][PATH_MAX];
	int cmd_ctr=-1;

	char cwd[PATH_MAX];

	int p[PATH_MAX][2];
	int pipe_ctr=0;

	while(1)
	{
		getcwd(cwd, sizeof(cwd));
		
		printf("\033[1;31m");
		printf("shell@dushyant");
		
		printf("\033[1;34m");
		printf(":");

		printf("\033[1;32m");
		printf("%s",cwd);

		printf("\033[1;34m");
		printf("$ ");

		printf("\033[0;33m");



		char inp[PATH_MAX];
		fgets(inp,PATH_MAX,stdin);

		if(inp[0]=='e' && inp[1]=='x' && inp[2]=='i' && inp[3]=='t')	//EXIT
		{	
			printf("Good Bye!\n");

			exit(0);
		}

		

		/*int i=0;
		while(i<ctr)
		{
			if(cmd[i]==NULL)
				printf("%p\n",cmd[i++]);
			else
				printf("%s\n", cmd[i++]);
		}*/

		//printf("parse_input done\n");

		int pid=fork();
		if(pid==0)
		{

			char *cmd[CMD_MAX];		//list to store output of parse_input
			for(int i=0; i<CMD_MAX; i++)
				cmd[i]=malloc(PATH_MAX);

			char *out=redirections(inp);	//apply redirection commands if present and remove from the input string

			int ctr=parse_input(out,cmd);	//split the input string into list of tokens, ctr stores the number of elements in this list

			cmd_ctr=-1;
			//execv(cmd[0],cmd);
			int i=0;
			while(i<ctr)
			{
				cmd_ctr++;
				int j=0;
				do
				{
					args[cmd_ctr][j++]=cmd[i++];
					args[cmd_ctr][j]=NULL;
				}
				while(cmd[i]!=NULL && i<ctr && j<PATH_MAX);		//Fetch the next set of args until a NULL is encountered

				/*int j_ctr=0;
				while(j_ctr<=j)
				{
					if(args[cmd_ctr][j_ctr]==NULL)
						printf("%p\n",args[cmd_ctr][j_ctr++]);
					else
						printf("%s\n", args[cmd_ctr][j_ctr++]);
				}*/

				i++;

				if(i<ctr)	//if more commands and operations
				{
					if(cmd[i][0]=='|')	//Pipes
					{
						pipe(p[pipe_ctr++]);
						
						int pid1=fork();
						if(pid1==0)		//Child process for write side of current pipe
						{
							close(1);
							dup(p[pipe_ctr-1][1]);
							close(p[pipe_ctr-1][0]);

							//printf("Child for %s\n",args[cmd_ctr][0]);

							execv(args[cmd_ctr][0],args[cmd_ctr]);	//execute the command
						}
						else 			//Parent process keeps the read part ready
						{
							close(0);
							dup(p[pipe_ctr-1][0]);
							close(p[pipe_ctr-1][1]);
						}
					}

					else if(cmd[i][0]=='>' && cmd[i][1]=='>')	//append to a file
					{
						i++;
						int pid1=fork();
						if(pid1==0)		//Child process opens the file to be written on
						{
							close(1);
							//fd[fd_ctr++]=
							int fd=open(cmd[i],O_WRONLY|O_APPEND,0666);
							dup(fd);//[fd_ctr-1]);

							execv(args[cmd_ctr][0],args[cmd_ctr]);	//execute the command
						}
						else
							wait(NULL);	//wait for the child to exit
					}

					else if(cmd[i][0]=='>')						//overwrite a file
					{
						i++;
						int pid1=fork();
						if(pid1==0)		//Child process opens the file to be written on
						{
							close(1);
							//fd[fd_ctr++]=
							int fd=open(cmd[i],O_WRONLY|O_CREAT|O_TRUNC,0666);
							dup(fd);//[fd_ctr-1]);

							execv(args[cmd_ctr][0],args[cmd_ctr]);	//execute the command
						}
						else
							wait(NULL);	//wait for the child to exit
					}
					else if(cmd[i][0]=='<')						//read from a file
					{
						i++;
						int pid1=fork();
						if(pid1==0)		//Child process opens the file to be read from
						{
							close(0);
							//fd[fd_ctr++]=
							int fd=open(cmd[i],O_RDONLY,0666);
							dup(fd);//[fd_ctr-1]);

							execv(args[cmd_ctr][0],args[cmd_ctr]);	//execute the command
						}
						else
							wait(NULL);	//wait for the child to exit
					}

					i++;
				}
				else
				{
					execv(args[cmd_ctr][0],args[cmd_ctr]);
				}
			}
			exit(0);
		}
		else if(pid<0)
			printf("Failed to fork\n");
		else
			wait(NULL);		//parent wait for child to exit
	



		//printf("%s\n", inp);
		//printf("Iteration %d ends\n",iter++ );
		//printf("%s\n",inp);
	}



	return 0;
}