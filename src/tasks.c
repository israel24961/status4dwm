#define _DEFAULT_SOURCE
#include "tasks.h"
#include <sys/wait.h>
#include <unistd.h>
void task_2secs(stat_node* st){
    static int j=0;
    free(st->msg.txt);
    st->msg.len=snprintf(NULL,0,"%fSecs:%i",st->period_secs,j);
    st->msg.txt=malloc(st->msg.len+1);
    snprintf(st->msg.txt,st->msg.len+1,"%fSecs:%i",st->period_secs,j++);
}
#define st_constmsg(st,str) st->msg.txt=str;        \
                            st->msg.len=sizeof(str);
void get_cpu_usage(stat_node* st)
{
    char *cd_emoji= "\U0001F4BF";
    char *dvd_emoji="\U0001F4C0";
    char* format="%s%.2f,%.2f,%.2f";
    double avgs[3];
    if(getloadavg(avgs,3)<0)
    {
        st_constmsg(st,"error load avg");
        return;
    }
    char *c_emoji= st->msg.txt != NULL ?
        st->msg.txt[2]==-110?
                dvd_emoji:
                cd_emoji:
        cd_emoji;

    st->msg.len=snprintf(NULL,0,format,c_emoji,avgs[0],avgs[1],avgs[2]);
    void* tmp=NULL;
    if(NULL==(tmp=realloc(st->msg.txt,st->msg.len+1))) {
        st_constmsg(st,"error realloc");
        return;
    }else {
        st->msg.txt=tmp;
        snprintf(st->msg.txt,st->msg.len+1, format,c_emoji,avgs[0],avgs[1],avgs[2]);
    }
    return;
}
void task_5secs(stat_node* st){
    static int i=0;
    free(st->msg.txt);
    st_make_message(st,"%fSecs:%i",st->period_secs,i);
}
//-lcurl
void get_xmr(stat_node* st){
    st_constmsg(st,"hi");
}
