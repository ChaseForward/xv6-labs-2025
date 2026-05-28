#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void find(char* dir, char* target, int exec_flag, int exec_start, int argc, char* argv[]) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // 打开目录
    if((fd = open(dir, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot open %s\n", dir);
        return;
    }

    // 获取目录状态
    if(fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", dir);
        close(fd);
        return;
    }

    if(st.type != T_DIR) {
        close(fd);
        return;
    }

    // 路径拼接准备
    if(strlen(dir) + 1 + DIRSIZ + 1 > sizeof(buf)) {
        printf("find: path too long\n");
        close(fd);
        return;
    }
    strcpy(buf, dir);
    p = buf + strlen(buf);
    *p++ = '/';

    // 遍历目录项
    while(read(fd, &de, sizeof(de)) == sizeof(de)) {
        // 跳过未使用的目录项和 .、..
        if(de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
            continue;

        // 拼接完整路径
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;

        // 获取文件状态
        if(stat(buf, &st) < 0) {
            printf("find: cannot stat %s\n", buf);
            continue;
        }

        // 如果是目录，递归查找
        if(st.type == T_DIR) {
            find(buf, target, exec_flag, exec_start, argc, argv);
        } 
        // 如果是文件，检查是否匹配
        else if(st.type == T_FILE) {
            if(strcmp(de.name, target) == 0) {
                if (exec_flag) {
                    int num_args = argc - exec_start;
                    char* cmd_args[num_args + 2];

                    for (int i = 0; i < num_args; ++i) {
                        cmd_args[i] = argv[exec_start + i];
                    }
                    cmd_args[num_args] = buf;
                    cmd_args[num_args + 1] = 0;
                    if(fork() == 0) {
                        exec(cmd_args[0], cmd_args);
                        fprintf(2, "exec %s failed\n", cmd_args[0]);
                        exit(1);
                    } else {
                        // 父进程：等待子进程完成
                        wait(0);
                    }
                } else {
                    printf("%s\n", buf);
                }
            }
        }
    }

    close(fd);
}


int main(int argc, char *argv[]) {
    int i;
    int exec_flag = 0;
    int exec_start = 0; // --exec 参数的位置

    if (argc < 3) {
        fprintf(2, "usage: find <directory> <filename> [-exec command...]\n");
        exit(1);  // 非零退出码表示错误
    }
    // 检查是否有 -exec 参数
    for(i = 3; i < argc; i++) {
        if(strcmp(argv[i], "-exec") == 0) {
            exec_flag = 1;
            exec_start = i + 1; // 命令
            break;
        }
    }
    find(argv[1], argv[2], exec_flag, exec_start, argc, argv);
    exit(0);
}