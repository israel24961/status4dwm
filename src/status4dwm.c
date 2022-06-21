#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "tasks.h"

int main()
{
    stat_stuff* st=stat_init();
    stat_add(st,2.0f,task_2secs);
    stat_add(st,5.0f,task_5secs);
    stat_add(st,3.0f,get_cpu_usage);
    stat_add(st,3.0f,get_xmr);
    while(1) {
        sleep(1);
        stat_run(st,time(NULL));
        //printf("%s\n", stat_msg(st)); //quick debug
        xsetroot_update(st);
    }
    return 0;
}
