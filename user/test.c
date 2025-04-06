#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int n = 0;
    if (argc >= 2) n = atoi(argv[1]);

    printf("HI TO KERNEL %d\n", n);
    int out = sysinfo(n);
    printf("Number out: %d\n", out);
    exit(0);
}