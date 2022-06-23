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
void xsetroot_update(stat_stuff* st)
{
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
    st->msg.len=snprintf(NULL,0,"%fSecs:%i",st->period_secs,i);
    st->msg.txt=malloc(st->msg.len+1);
    snprintf(st->msg.txt,st->msg.len+1,"%fSecs:%i",st->period_secs,i++);
}
//-lcurl
#include <curl/curl.h>
#include <json-c/json.h>

static int CURL_GLOBAL_CALLED=0;
//Null ended *buffer, ensured
size_t get_xmr_write_data(void* buffer,size_t size, size_t nmemb,void *userp){
    size_t realsize = size * nmemb;
  struct {char* bytes;size_t size;} *mem =userp;

  char *ptr = realloc(mem->bytes, mem->size + realsize + 1);
  if(!ptr) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  mem->bytes = ptr;
  memcpy(&(mem->bytes[mem->size]), buffer, realsize);
  mem->size += realsize;
  mem->bytes[mem->size] = 0;//Null ended

  return realsize;
return size;
}
double get_xmr_curl_setup(stat_node* st) {
    if(CURL_GLOBAL_CALLED==0) {
        CURL_GLOBAL_CALLED=1;
        curl_global_init(CURL_GLOBAL_SSL);
    }
    static CURL *hnd=NULL;

    if(hnd==NULL)
    {
    hnd = curl_easy_init();
    curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
    curl_easy_setopt(hnd, CURLOPT_URL, "https://api.binance.com/api/v3/depth\?symbol=XMRUSDT&limit=1");
    curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(hnd, CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.83.1");
    curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
    curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(hnd,CURLOPT_WRITEFUNCTION,get_xmr_write_data);
    }
    struct {char* bytes;size_t size;} dmem={.bytes=malloc(0),.size=0};
    curl_easy_setopt(hnd,CURLOPT_WRITEDATA,&dmem);
    CURLcode ret= curl_easy_perform(hnd);

    //curl_easy_cleanup(hnd);
    //hnd = NULL;

    json_object *obj=json_tokener_parse(dmem.bytes);
    json_object *arr= json_object_object_get(obj,"bids");
    if(json_object_get_type(arr) != json_type_array)
        st_constmsg(st,"err: hackers attacking");
    json_object *arr_0=json_object_array_get_idx(arr,0);
    json_object *val=json_object_array_get_idx(arr_0,0);
    double i=json_object_get_double(val);
    json_object_put(obj);
    return i;
}
void get_xmr(stat_node* st){
    if (system("ping -c 1 -W 1 8.8.8.8 1>/dev/null 2>/dev/null")!=0)
    {
        st_constmsg(st,"NoInternet");
    }
    double i=get_xmr_curl_setup(st);
    free(st->msg.txt);
    const char * fmt="💵/XMR: %.2f";
    st->msg.len=snprintf(NULL,0,fmt,i);
    st->msg.txt=malloc(st->msg.len+1);
    snprintf(st->msg.txt,st->msg.len+1,fmt,i);
}