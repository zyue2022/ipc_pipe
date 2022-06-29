#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    // 1.判断管道1是否存在
    int ret1 = access("fifo1", F_OK);
    if (ret1 == -1) {
        perror("open");
        printf("管道文件fifo1不存在，创建fifo1...\n");
        ret1 = mkfifo("fifo1", 0664);
        if (ret1 == -1) {
            perror("mkfifo");
            exit(0);
        }
    }

    // 2.判断管道2是否存在
    int ret2 = access("fifo2", F_OK);
    if (ret2 == -1) {
        perror("open");
        printf("管道文件fifo2不存在，创建fifo2...\n");
        ret2 = mkfifo("fifo2", 0664);
        if (ret2 == -1) {
            perror("mkfifo");
            exit(0);
        }
    }

    // 3.以只读权限打开fifo1
    int fd1 = open("fifo1", O_RDONLY);
    if (fd1 == -1) {
        perror("open");
        exit(0);
    }
    printf("进程B打开fifo1成功，等待读取数据...\n");

    // 4.以只写权限打开fifo2
    int fd2 = open("fifo2", O_WRONLY);
    if (fd2 == -1) {
        perror("open");
        exit(0);
    }
    printf("进程B打开fifo2成功，等待输入数据...\n");
    
    // 5.循环地读写数据
    char buf[1024] = {0};
    while (1) {
        // 读数据
        bzero(buf, 1024);
        int ret = read(fd1, buf, 1024);
        if (ret <= 0) {
            perror("read");
            exit(0);
        }
        printf("接收数据: %s\n", buf);
        // 获取输入
        bzero(buf, 1024);
        printf("请输入待发送数据：");
        fgets(buf, 1024, stdin);
        // 写数据
        ret = write(fd2, buf, strlen(buf));
        if (ret == -1) {
            perror("write");
            exit(0);
        }
        printf("发送数据: %s\n", buf);
    }
    
    // 6.关闭管道文件
    close(fd1);
    close(fd2);

    return 0;
}