#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "utils.h"
#include "tasks.h"
#include <time.h>

void add_tasks(stat_stuff* st);

int main(int argc, char** args) {
    stat_stuff* st=stat_init();
    logging_init();
    add_tasks(st);

    while(1) {
        sleep(2);
        stat_run(st, time(NULL));
        xsetroot_update(st);
        u_log(stat_msg(st));
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
