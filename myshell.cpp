#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring> 
#include <cerrno>  
#include <fcntl.h> 

#define SEP " "
#define NUM 1024
#define SIZE 128

std::string command_line;
std::vector<std::string> command_args;
std::string env_buffer; // for test

extern char** environ;

// 对应上层的内建命令
int ChangeDir(const std::string& new_path)
{
    if (chdir(new_path.c_str()) != 0) {
        std::cerr << "cd error: " << strerror(errno) << std::endl;
        return -1; // 调用失败
    }
    return 0; // 调用成功
}

void PutEnvInMyShell(const std::string& new_env)
{
    if (putenv(const_cast<char*>(new_env.c_str())) != 0) {
        std::cerr << "putenv error: " << strerror(errno) << std::endl;
    }
}

void parse_command()
{
    std::istringstream iss(command_line);
    std::string token;
    command_args.clear();
    while (iss >> token) {
        command_args.push_back(token);
    }
}

void execute_command()
{
    if (command_args.empty()) {
        return; // 用户输入为空
    }

    // 内建命令
    if (command_args[0] == "cd" && command_args.size() > 1) {
        ChangeDir(command_args[1]);
        return;
    }
    if (command_args[0] == "export" && command_args.size() > 1) {
        env_buffer = command_args[1];
        PutEnvInMyShell(env_buffer);
        return;
    }

    // 创建进程,执行
    size_t pipe_pos = 0;
    size_t num_pipes = 0;
    std::vector<int> pipe_fds;

    for (const auto& arg : command_args) {
        if (arg == "|") {
            num_pipes++;
        }
    }

    pipe_fds.resize(2 * num_pipes);

    for (size_t i = 0; i < num_pipes; ++i) {
        if (pipe(pipe_fds.data() + 2 * i) < 0) {
            std::cerr << "pipe error: " << strerror(errno) << std::endl;
            return;
        }
    }

    size_t start = 0;
    for (size_t i = 0; i <= num_pipes; ++i) {
        size_t end = start;
        while (end < command_args.size() && command_args[end] != "|") {
            ++end;
        }

        std::vector<std::string> segment(command_args.begin() + start, command_args.begin() + end);

        pid_t id = fork();
        if (id == 0) {
            // child
            if (i > 0) {
                dup2(pipe_fds[2 * (i - 1)], STDIN_FILENO);
            }
            if (i < num_pipes) {
                dup2(pipe_fds[2 * i + 1], STDOUT_FILENO);
            }

            for (int fd : pipe_fds) {
                close(fd);
            }

            std::vector<char*> args;
            for (auto& arg : segment) {
                args.push_back(const_cast<char*>(arg.c_str()));
            }
            args.push_back(nullptr);

            execvp(args[0], args.data());
            // std::cerr << "execvp error: " << strerror(errno) << std::endl;
            exit(1);
        } else if (id < 0) {
            std::cerr << "fork error: " << strerror(errno) << std::endl;
            return;
        }

        start = end + 1;
    }

    for (int fd : pipe_fds) {
        close(fd);
    }

    for (size_t i = 0; i <= num_pipes; ++i) {
        int status = 0;
        wait(&status);
    }
}

void run_shell()
{
    // shell 本质上就是一个死循环
    while (true) {
        // 显示提示符
        std::cout << "[张三@主机名 当前目录]# ";
        std::cout.flush();

        // 获取用户输入
        std::getline(std::cin, command_line);
        if (!command_line.empty() && command_line.back() == '\n') {
            command_line.pop_back(); // 清空\n
        }

        // "ls -a -l -i" -> "ls" "-a" "-l" "-i" 字符串切分
        parse_command();

        execute_command();
    } // end while
}

void run_script(const std::string& script_path)
{
    std::ifstream script_file(script_path);
    if (!script_file.is_open()) {
        std::cerr << "无法打开脚本文件: " << script_path << std::endl;
        return;
    }

    while (std::getline(script_file, command_line)) {
        if (command_line.empty() || command_line[0] == '#') {
            continue; // 忽略空行和注释行
        }

        if (!command_line.empty() && command_line.back() == '\n') {
            command_line.pop_back(); // 清空\n
        }

        // "ls -a -l -i" -> "ls" "-a" "-l" "-i" 字符串切分
        parse_command();

        execute_command();
    }
}

int main(int argc, char* argv[])
{
    if (argc > 1) {
        run_script(argv[1]);
    } else {
        run_shell();
    }

    return 0;
}
