#ifndef stat_stuff_h
#define stat_stuff_h

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef  struct  stat_stuff stat_stuff;
typedef struct stat_node stat_node;
typedef struct stat_node
{
    struct {char* txt;size_t len;} msg;
    double period_secs;//seconds
    void (*task)(stat_node*);

    time_t last_secs;//Last time
    stat_node* next;
} stat_node;
stat_stuff* stat_init();
stat_stuff* stat_add(stat_stuff*,double,void (*)(stat_node*));
void stat_run(stat_stuff* st,time_t now);
char* stat_msg(stat_stuff* st);

#endif
