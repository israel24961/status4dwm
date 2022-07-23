#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "tasks.h"

int main(int argc, char** args)
{
    stat_stuff* st=stat_init();
    stat_add(st,3.0f,get_cpu_usage);
    stat_add(st,5.0f,get_xmr);
    stat_add(st,60.0f,get_date_hour);
    stat_add(st,2.0f,get_power);
    stat_add(st,5.0f,get_temperature);
    while(1) {
        sleep(2);
        stat_run(st,time(NULL));
        //printf("%s\n", stat_msg(st)); //quick debug
        xsetroot_update(st);
    }
    return 0;
}
