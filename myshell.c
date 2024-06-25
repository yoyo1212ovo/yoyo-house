#include <stdio.h>
#include <string.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SEP " "
#define NUM 1024
#define SIZE 128

char command_line[NUM];
char *command_args[SIZE];

char env_buffer[NUM]; //for test

extern char**environ;

//对应上层的内建命令
int ChangeDir(const char * new_path)
{
    chdir(new_path);

    return 0; // 调用成功
}

void PutEnvInMyShell(char * new_env)
{
    putenv(new_env);
}

int main()
{
    //shell 本质上就是一个死循环
    while(1)
    {
        //不关心获取这些属性的接口, 搜索一下
        //1. 显示提示符
        printf("[张三@我的主机名 当前目录]# ");
        fflush(stdout);

        //2. 获取用户输入
        memset(command_line, '\0', sizeof(command_line)*sizeof(char));
        fgets(command_line, NUM, stdin); //键盘，标准输入，stdin, 获取到的是c风格的字符串, '\0'
        command_line[strlen(command_line) - 1] = '\0';// 清空\n

        //3. "ls -a -l -i" -> "ls" "-a" "-l" "-i" 字符串切分
        command_args[0] = strtok(command_line, SEP);
        int index = 1;
        // 给ls命令添加颜色
        if(strcmp(command_args[0]/*程序名*/, "ls") == 0 ) 
            command_args[index++] = (char*)"--color=auto";
        // = 是故意这么写的
        // strtok 截取成功，返回字符串其实地址
        // 截取失败，返回NULL
        while(command_args[index++] = strtok(NULL, SEP));

        //for debug
        //for(int i = 0 ; i < index; i++)
        //{
        //    printf("%d : %s\n", i, command_args[i]);
        //}
    
        // 4. TODO, 编写后面的逻辑, 内建命令
        if(strcmp(command_args[0], "cd") == 0 && command_args[1] != NULL)
        {
            ChangeDir(command_args[1]); //让调用方进行路径切换, 父进程
            continue;
        }
        if(strcmp(command_args[0], "export") == 0 && command_args[1] != NULL)
        {
            // 目前，环境变量信息在command_line,会被清空
            // 此处我们需要自己保存一下环境变量内容
            strcpy(env_buffer, command_args[1]);
            PutEnvInMyShell(env_buffer); //export myval=100, BUG?
            continue;
        }

        // 5. 创建进程,执行
        pid_t id = fork();
        if(id == 0)
        {
            //child
            // 6. 程序替换
            //exec*?
            execvp(command_args[0]/*不就是保存的是我们要执行的程序名字吗？*/, command_args);
            exit(1); //执行到这里，子进程一定替换失败
        }
        int status = 0;
        pid_t ret = waitpid(id, &status, 0);
        if(ret > 0)
        {
            printf("等待子进程成功: sig: %d, code: %d\n", status&0x7F, (status>>8)&0xFF);
        }
    }// end while
}












