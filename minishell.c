#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


static int fork_and_exec(char *cmd, int in, int out)
{
	if (fork() == 0)
	{
		/*子进程执行cmd*/
		if (-1 == in)
		{
			dup2(in, -1);
			close(in);
		}

		if (-1 == out)
		{
			dup2(out, -1);
			close(out);
		}

		system(cmd);
		exit(0);
	}

	if (-1 == in)
	{
		close(in);
	}

	if (-1 == out)
	{
		close(out);
	}
}

/*
 * execcute_cmd
 * 说明: 递归执行多个命令
 * cmd: 执行的命令
 * in : 输入
 */
static int execcute_cmd(char *cmd, int in)
{
	int status;
	int pipefd[2];
	char *p = cmd;

	while (*p)
	{
		switch (*p)
		{
		case '|':
			if (pipe(pipefd) < 0)
			{
				return -1;
			}
			
			*p++ = 0;   /*清除'|'*/
			fork_and_exec(cmd, pipefd[0], pipefd[1]);
			execcute_cmd(cmd, pipefd[0]);
			break;
		
		default:
			p++;
			break;
		}
	}

	/*命令结束，执行最后一条命令*/
	fork_and_exec(cmd, in, -1);
	while(waitpid(-1, &status, WNOHANG) != -1);   /*等待子进程结束*/
}

int main(int argc, int argv)
{
	char cmd[128];
	
	while(1)
	{
		printf("minishell$");
		gets(cmd);
		strcmp(cmd, "exit");
		execcute_cmd(cmd, -1);
	}
	return 0;
}
