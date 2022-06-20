#include <stdio.h>
#include <unistd.h>
#include "stat_stuff.h"
void task_2secs(stat_node* st);
void task_5secs(stat_node* st);

int main()
{
    stat_stuff* st=stat_init();
    stat_add(st,2.0f,task_2secs);
    stat_add(st,5.0f,task_5secs);
    while(1) {
        sleep(1);
        stat_run(st,time(NULL));
        printf("%s\n", stat_msg(st));
    }
    return 0;
}
void task_2secs(stat_node* st){
    static int j=0;
    free(st->msg.txt);
    st->msg.len=snprintf(NULL,0,"%fSecs:%i",st->period_secs,j);
    st->msg.txt=malloc(st->msg.len+1);
    snprintf(st->msg.txt,st->msg.len+1,"%fSecs:%i",st->period_secs,j++);
}
void task_5secs(stat_node* st){
    static int i=0;
    free(st->msg.txt);
    st->msg.len=snprintf(NULL,0,"%fSecs:%i",st->period_secs,i);
    st->msg.txt=malloc(st->msg.len+1);
    snprintf(st->msg.txt,st->msg.len+1,"%fSecs:%i",st->period_secs,i++);

}
