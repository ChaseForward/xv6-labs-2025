#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[512];
int main(int argc, char* argv[]) {
    int fd, i, n;
    if(argc <= 1) {
        fprintf(2, "Usage: sixfive file.txt\n");
        exit(1);
    }
    for(i = 1; i < argc; i++) {
        if((fd = open(argv[i], O_RDONLY)) < 0) {
            fprintf(2, "sixfive: cannot open %s\n", argv[i]);
            exit(1);
        }
        char num_buf[128];
        int start, end;
        while((n = read(fd, buf, sizeof(buf))) > 0) { // 分组循环
            int j = 0;
            while (j < n) {
                if (buf[j] >= '0' && buf[j] <= '9') {
                    start = j;
                    while (j < n && buf[j] >= '0' && buf[j] <= '9') {
                        ++j;
                    }
                    if (start == 0 || buf[start - 1] == '-' || buf[start - 1] == '\r' || buf[start - 1] == '\t' || buf[start - 1] == '\n' || buf[start - 1] == '.' || buf[start - 1] == '/' || buf[start - 1] == ',') {
                        end = j;
                        int len = end - start;
                        memcpy(num_buf, buf + start, len);
                        num_buf[len] = '\0';
                        int num = atoi(num_buf);
                        if (num % 5 == 0 || num % 6 == 0) {
                            printf("%d\n", num);
                        }
                        memset(num_buf, 0, sizeof(num_buf));
                    }
                } else {
                    ++j;
                }
            }
            memset(buf, 0, sizeof(buf));    
        }
        close(fd);
    }
    return 0;
}