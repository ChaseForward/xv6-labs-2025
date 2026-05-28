#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int main(int argc, char* argv[]) {
    // 1. 检验参数个数是否符合要求
    if (argc != 2) {
        fprintf(2, "Usage: sleep seconds\n");
        exit(1);
    }
    // 2. 检验字符串是否合法
    const char* s = argv[1];
    int i;
    uint len = strlen(s);
    for (i = 0; i < len; ++i) {
        if (s[i] < '0' || s[i] > '9') {
            fprintf(2, "sleep: invalid number\n");
            exit(1);
        }
    }
    int num = atoi(s);
    fprintf(1, "(nothing happens for a little while)\n");
    pause(num);
    exit(0);
}