#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include"linkedlist.h"
#include<signal.h>

int sh_cd(char** args);
int sh_exit();
int sh_bg(char** args);
int sh_bglist();
int sh_google(char** args);
int sh_yahoo(char** args);
int sh_kill(char **args);

char* builtin_str[]={
	"cd",
	"exit",
    "bg",
    "bglist",
	"google",
	"yahoo",
    "kill"
};

int (*builtin_func[]) (char**)={
	&sh_cd,
	&sh_exit,
    &sh_bg,
    &sh_bglist,
	&sh_google,
	&sh_yahoo,
    &sh_kill
};
char *sh_read_line()
{
	char*line=NULL;
	ssize_t bufsize =0;
	if(getline(&line,&bufsize,stdin)==-1)
	{
		if(feof(stdin))
			exit(EXIT_SUCCESS);
		else
		{
			perror("myshell-sh: getline\n");
			exit(EXIT_FAILURE);
		}
	}
	return line;
} 

#define SH_TOKEN_BUFSIZE 64
#define SH_TOK_DELIM " \t\r\n"

char** sh_split_line(char* line)
{
	int bufsize=SH_TOKEN_BUFSIZE,position=0;
	char ** tokens=malloc(bufsize * sizeof(char*));
	char *token;

	if(!token)
	{
		fprintf(stderr,"myshell-sh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	//strtok :- Library Function 
	token=strtok(line,SH_TOK_DELIM);
	while(token!=NULL)
	{
		tokens[position]=token;
		token=strtok(NULL,SH_TOK_DELIM);

		position++;
	}
	tokens[position]=NULL;
	return tokens;
}

int sh_launch(char **args)
{
	pid_t pid;
	int status;

	pid=fork();
	if(pid==0)
	{
		if(execvp(args[0],args)==-1)
		{
			printf("Invalid Command\n");
		}
		exit(EXIT_FAILURE);
	}
	else if(pid<0)
	{
		perror("myshell-sh");
	}
	else
	{
		do
		{
			waitpid(pid,&status,WUNTRACED);
		}
		while(!WIFEXITED(status)&&!WIFSIGNALED(status));
	}
	return 1;
}

int sh_execute(char ** args)
{
	int i;
	if(args[0]==NULL)
	{
		return 1;
	}

	for(int i=0;i<7;i++)
	{
		if(strcmp(args[0],builtin_str[i])==0)
			return (*builtin_func[i])(args);
	}

	return sh_launch(args);

}

void broadcastTermination(int pid,int status){
    if (WIFEXITED(status)){
        printf("exited, status=%d\n", WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status)){
        printf("killed by signal %d\n", WTERMSIG(status));
    }
    else if(WIFSTOPPED(status))
    {
        printf("stopped by signal %d\n", WSTOPSIG(status));
    }
    else if(WIFCONTINUED(status))
    {
        printf("continued\n");
    }
    delete_from_list(pid);
}

static void signalHandler(int sig){
    int pid;
    int status;
    pid=waitpid(-1,&status,WNOHANG);
    broadcastTermination(pid,status);
}


int sh_cd(char** args)
{
	if(args[1]==NULL)
	{
		fprintf(stderr,"sh: expected argument to \"cd\"\n");
	}
	else
	{
		if(chdir(args[1])!=0)
		{
			perror("Invalid Call");
		}
	}
	return 1;
}

int sh_exit(char **args)
{
	return 0;
}

process* headProcess=NULL;
int sh_bg(char **args)
{
//args -- bg echo "hello"
    ++args;
//args -- echo "hello"
    char *firstCmd= args[0];//echo
    int childpid=fork();
    if (childpid>=0)
    {
        if (childpid==0)
        {
            if (execvp(firstCmd,args)<0)
            {
                perror("Error on execvp\n");
                exit(0);
            }

        }
        else{
                if (headProcess==NULL)
                {
                    headProcess=create_list(childpid,firstCmd);
                }
                else{
                    add_to_list(childpid,firstCmd,true);
                }
        }
    }
    else{
        perror("fork() error");
    }
    return 1;
}
int sh_bglist(char **args)
{
    print_list();
    return 1;
}
int sh_kill(char **args)
{
    char* pidCmd=args[1];
    if(!pidCmd)//kill 
    {
        printf("Please specify a pid\n");
    }
    else
    {
        int pid=atoi(pidCmd);
        process* target= search_in_list(pid, &headProcess);
        if (target!=NULL)
        {
            if (kill(target->pid,SIGTERM)>=0)
            {
                delete_from_list(pid);
            }
            else{
                perror("Could not kill pid specified\n");
            }
        }
        else{
            printf("Specify a pid which is present in the list.\nType \"bglist\" to see active processes\n");
        }
    }
    return 1;
}
int sh_google(char** args){
	if(args[1]==NULL){
		char line1[]="xdg-open https://www.google.com";
		char **args1=sh_split_line(line1);
		return sh_launch(args1);
	}
	if(args[2]!=NULL){
		perror("only 1 arguement search allowed\n");
		return 1;
	}
	char command[1000]="xdg-open https://www.google.com/search?q=";
	char* search_query=args[1];
	strcat(command,search_query);
	char **args2=sh_split_line(command);
	return sh_launch(args2);
}
int sh_yahoo(char** args){
	if(args[1]==NULL){
		char line1[]="xdg-open https://www.youtube.com";
		char **args1=sh_split_line(line1);
		return sh_launch(args1);
	}
	if(args[2]!=NULL){
		perror("only 1 arguement search allowed\n");
		return 1;
	}
	char command[1000]="xdg-open https://www.yahoo.com/results?search_query=";
	char* search_query=args[1];
	strcat(command,search_query);
	char **args2=sh_split_line(command);
	return sh_launch(args2);
}
int main(int argc,char **argv)
{
	char *line;
	char **args;
	int status;
    signal(SIGCHLD,signalHandler);
	do{
		
		printf("myshell > ");
		line=sh_read_line();
		args=sh_split_line(line);
		status=sh_execute(args);

		free(line);
		free(args);
	}
	while(status);

	return 0;
}
