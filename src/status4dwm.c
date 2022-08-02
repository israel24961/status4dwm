#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "tasks.h"

void add_tasks(stat_stuff* st);

int main(int argc, char** args)
{
    stat_stuff* st=stat_init();
    add_tasks(st);

    while(1) {
        sleep(2);
        stat_run(st,time(NULL));
        //printf("%s\n", stat_msg(st)); //quick debug
        xsetroot_update(st);
    }
    return 0;
}

void add_tasks(stat_stuff* st)
{
    stat_add(st,3.0f,get_cpu_usage);
    stat_add(st,5.0f,get_xmr);
    stat_add(st,60.0f,get_date_hour);
    stat_add(st,2.0f,get_power);
    stat_add(st,5.0f,get_temperature);
}
