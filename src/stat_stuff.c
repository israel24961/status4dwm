#include "stat_stuff.h"
#include "utils.h"
//Checks whether pointer is null, and exits
#define CHK_NULL(val) \
if (val==NULL){         \
    printf("ERROR-%s:%s:%i-%s null after malloc",__FILE__,__func__,__LINE__,#val); \
    exit(-1);}      \

#define CHK_MALLOC(val,type,n) \
type* val=malloc(sizeof(type)*n);\
CHK_NULL(val)

#include "stat_stuff.h"
typedef struct
{
    char* txt;
    size_t len;//len is the number of char elements minus the last null byte
}sstr;

typedef  struct stat_stuff
{
    sstr complete_msg;
    stat_node* head;
} stat_stuff;

//TOMAYBEDO: Should this be a function
#define append_node(type,head,new_node)  type* __last=head;\
                                     while(__last->next!=NULL)\
                                        {__last=__last->next;}\
                                        __last->next=new_node;
//Frees sstr after usage and sstr.txt
stat_stuff* stat_init()
{
    CHK_MALLOC(stat,stat_stuff,1);
    stat->complete_msg.len=0;
    stat->complete_msg.txt=NULL;
    stat->head=NULL;
    return stat;
}
stat_stuff* stat_add(stat_stuff* st,double period_secs, void (*task)(stat_node*)){
    CHK_MALLOC(nst,stat_node,1);
    nst->msg.txt=NULL;
    nst->msg.len=0;
    nst->last_secs=0;

    nst->period_secs=period_secs;
    nst->task=task;
    nst->next=NULL;

    if (st->head == NULL) {
        st->head=nst;
        return st;
    }
    append_node(stat_node,st->head,nst);
    return st;
}
void stat_run(stat_stuff* st,time_t now) {
    free(st->complete_msg.txt);//Clears before next one, relies on free(NULL) not giving error
    st->complete_msg.len=0;

    stat_node* current= st->head;
    for(;current!=NULL;current=current->next,st->complete_msg.len++)//++ because of '|' character in between blocks and the last null
    {
        double cur_interval=difftime(now,current->last_secs);
        if( current->last_secs==0 || cur_interval > current->period_secs){
            current->last_secs=now;//This before execution so the task has the time it was called
            current->task(current);
        }
        if(current->msg.txt != NULL)
            st->complete_msg.len+=current->msg.len;
    }
    CHK_MALLOC(total_msg,char,st->complete_msg.len);
    bzero(total_msg,st->complete_msg.len);
    current=st->head;
    for(size_t offset=0;current!=NULL;current=current->next){
        strncpy(total_msg+offset, //offset increases after strncpy
                current->msg.txt, 
                current->msg.len);
        offset+=current->msg.len;
        if (current->next !=NULL)
            *(total_msg+(offset++))='|';
    }
    st->complete_msg.txt=total_msg;
}
//Insecure, return length also
char* stat_msg(stat_stuff* st)
{
    return st->complete_msg.txt;
}
//Malloc sprintf + malloc
void st_make_message(stat_node* st, const char* format,...)
{//Almost copypasted from man vsnprintf 
    va_list ap;
    
    va_start(ap,format);
    st->msg.len= vsnprintf(NULL,0,format,ap);
    va_end(ap);
    st->msg.txt=malloc(st->msg.len+1);
    st->msg.txt[st->msg.len]=0;
    va_start(ap,format);
    st->msg.len= vsnprintf(st->msg.txt,st->msg.len+1,format,ap);
    va_end(ap);
}




