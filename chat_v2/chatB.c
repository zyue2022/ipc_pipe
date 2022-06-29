#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    //在子进程里面pid返回值是0；
    //在父进程里面pid返回值是子进程的pid号；
    pid_t pid = fork();

    //判断是父进程还是子进程
    if (pid > 0) {
        //大于0，返回的是创建的子进程的进程号，当前是父进程

        // 1.判断管道1是否存在
        int ret1 = access("fifo1", F_OK);
        if (ret1 == -1) {
            perror("open");
            printf("管道文件fifo1不存在，进程B创建fifo1...\n");
            ret1 = mkfifo("fifo1", 0664);
            if (ret1 == -1) {
                perror("mkfifo");
                exit(0);
            }
        }
        // 2.以只读权限打开fifo1
        int fd1 = open("fifo1", O_RDONLY);
        if (fd1 == -1) {
            perror("open");
            exit(0);
        }
        printf("进程B打开fifo1成功，等待读取数据...\n");

        // 3.循环地读数据
        char buf[1024] = {0};
        while (1) {
            // 读数据
            bzero(buf, 1024);
            int ret = read(fd1, buf, 1024);
            if (ret <= 0) {
                perror("read");
                break;
            }
            printf("进程B接收到数据: %s\n", buf);
        }

        // 4.关闭管道文件
        close(fd1);

    } else if (pid == 0) {
        //等于0，当前是子进程

        // 1.判断管道2是否存在
        int ret2 = access("fifo2", F_OK);
        if (ret2 == -1) {
            perror("open");
            printf("管道文件fifo2不存在，进程B_child创建fifo1...\n");
            ret2 = mkfifo("fifo2", 0664);
            if (ret2 == -1) {
                perror("mkfifo");
                exit(0);
            }
        }

        // 2.以只写权限打开fifo2
        int fd2 = open("fifo2", O_WRONLY);
        if (fd2 == -1) {
            perror("open");
            exit(0);
        }
        printf("进程B_child打开fifo2成功，等待输入数据...\n");

        // 3.循环地写数据
        char buf[1024] = {0};
        while (1) {
            // 获取输入
            bzero(buf, 1024);
            printf("进程B_child请您输入待发送数据:\n");
            fgets(buf, 1024, stdin);
            // 写数据
            int ret = write(fd2, buf, strlen(buf));
            if (ret == -1) {
                perror("write");
                break;
            }
            printf("进程B_child发送数据: %s\n", buf);
        }

        // 4.关闭管道文件
        close(fd2);
    }

    return 0;
}