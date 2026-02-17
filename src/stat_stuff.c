#include "stat_stuff.h"
#include <sys/wait.h>
#include <unistd.h>
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
    struct ev_loop* loop;
} stat_stuff;

//TOMAYBEDO: Should this be a function
#define append_node(type,head,new_node)  type* __last=head;\
                                     while(__last->next!=NULL)\
                                        {__last=__last->next;}\
                                        __last->next=new_node;
stat_stuff* stat_init()
{
    CHK_MALLOC(stat,stat_stuff,1);
    stat->complete_msg.txt = NULL;
    stat->complete_msg.len = 0;
    stat->head = NULL;
    stat->loop = ev_loop_new(EVFLAG_AUTO);
    if(stat->loop == NULL) {
        printf("ERROR: Failed to create libev loop\n");
        exit(-1);
    }
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

// Update the status bar using xsetroot
void xsetroot_update(stat_stuff* st)
{
#ifdef DEBUG
    // Log update timestamp and message in debug mode
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_buffer[26];
    strftime(time_buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    fprintf(stderr, "[%s] xsetroot update: %s\n", time_buffer, stat_msg(st));
#endif

    char* arg[4];
    arg[0]="/usr/bin/xsetroot";
    arg[1]="-name";
    arg[2]=stat_msg(st);
    arg[3]=NULL;
    if( 0==fork())
    {
        if (-1 == execv(arg[0],(char**)arg))
        {
            perror("child process execv failed[%m]\n");
            exit(-1);
        }
    }
    wait(NULL);
}

// Timer callback for libev
// This callback fires whenever ANY task's timer expires, based on its period_secs.
// Each timer firing triggers: task execution → message rebuild → xsetroot update.
// With multiple tasks at different periods (e.g., 2s, 3s, 5s), xsetroot is called
// frequently to keep the status bar current.
static void timer_callback(EV_P_ ev_timer *w, int revents)
{
    (void)revents; // unused parameter
    stat_node* node = (stat_node*)w->data;
    
    // Execute the task
    node->last_secs = time(NULL);
    node->task(node);
    
    // Get the stat_stuff from the loop data
    stat_stuff* st = (stat_stuff*)ev_userdata(EV_A);
    
    // Update the complete message
    if(st->complete_msg.txt != NULL) {
        free(st->complete_msg.txt);
        st->complete_msg.txt = NULL;
    }
    st->complete_msg.len=0;

    stat_node* current= st->head;
    for(;current!=NULL;current=current->next,st->complete_msg.len++)
    {
        if(current->msg.txt != NULL)
            st->complete_msg.len+=current->msg.len;
    }
    CHK_MALLOC(total_msg,char,st->complete_msg.len);
    bzero(total_msg,st->complete_msg.len);
    current=st->head;
    for(size_t offset=0;current!=NULL;current=current->next){
        strncpy(total_msg+offset,
                current->msg.txt,
                current->msg.len);
        offset+=current->msg.len;
        if (current->next !=NULL)
            *(total_msg+(offset++))='|';
    }
    st->complete_msg.txt=total_msg;
    
    // Update the status bar (called on every timer fire)
    xsetroot_update(st);
}

// Start the libev event loop
void stat_start_loop(stat_stuff* st)
{
    // Store stat_stuff pointer in loop userdata
    ev_set_userdata(st->loop, st);
    
    // Initialize and start timer for each node
    stat_node* current = st->head;
    while(current != NULL) {
        ev_timer_init(&current->timer, timer_callback, 0.0, current->period_secs);
        current->timer.data = current;
        ev_timer_start(st->loop, &current->timer);
        current = current->next;
    }
    
    // Run the event loop
    ev_run(st->loop, 0);
}



