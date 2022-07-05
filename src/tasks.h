#ifndef tasks_h
#define tasks_h

#include "stdlib.h"
#include <sys/wait.h>
#include <unistd.h>
#include <stdarg.h>
#include "stat_stuff.h"
void task_2secs(stat_node* st);
void task_5secs(stat_node* st);
void xsetroot_update(stat_stuff* st);
void get_cpu_usage(stat_node* st);
void get_xmr(stat_node* st);
void get_date_hour(stat_node* st);
void get_power(stat_node* st);

#endif
