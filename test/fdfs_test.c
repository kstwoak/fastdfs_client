#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>

#include "make_log.h"

#define FDFS_TEST_MODULE "test"
#define FDFS_TEST_PROC   "fdfs_test"

#define FILE_ID_LEN (256)

int main(int argc, char *argv[])
{
    int retn = 0;
    pid_t pid;

    int pfd[2];
    char fileid[FILE_ID_LEN] = {0};

    if (argc < 2) {
        printf("./a.out filename \n");
        exit(0);
    }

    // 1 创建管道
    retn = pipe(pfd);
    if (retn < 0) {
        perror("pipe error");
        exit(0);
    }
    

    // 2 创建进程
    pid = fork();
    if (pid == 0) {
        // child
        // 子

        //关闭 读管道
        close(pfd[0]);


        //将标准输出 重定向 到 管道写段
        dup2(pfd[1], STDOUT_FILENO);

        // 调用指令上传那文件
        //exec
        execlp("fdfs_upload_file", "fdfs_upload_file", "../conf/client.conf", argv[1], NULL);
        LOG(FDFS_TEST_MODULE, FDFS_TEST_PROC, "exec fdfs_upload_file error"); 

    }
    else if (pid > 0){
        // 父
        //关闭 写管道
        close(pfd[1]);

        // wait
        wait(NULL);

        // 从管道中读fileid数据
        read(pfd[0], fileid, FILE_ID_LEN);
        
        int size = strlen(fileid);
        fileid[size-1]=0;
        // 将fileid 数据写到log中
        LOG(FDFS_TEST_MODULE, FDFS_TEST_PROC, "fileid = [%s]\n", fileid);
    }

	return 0;
}
