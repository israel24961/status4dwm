#include "tasks.h"


void task_2secs(stat_node* st){
    static int j=0;
    free(st->msg.txt);
    st->msg.len=snprintf(NULL,0,"%fSecs:%i",st->period_secs,j);
    st->msg.txt=malloc(st->msg.len+1);
    snprintf(st->msg.txt,st->msg.len+1,"%fSecs:%i",st->period_secs,j++);
}
void xsetroot_update(stat_stuff* st)
{
    char const* arg[4];
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
char* file2str(char* filepath){
    FILE* fload=fopen(filepath,"r");
    if (NULL==fload)
    {
        return 0;
    }
    fseek(fload,0,SEEK_END);
    size_t len=ftell(fload);
    fseek(fload,0,SEEK_SET);
    char *str=malloc(len+1);
    bzero(str,len+1);

    fread(str,len,1,fload);
    fclose(fload);
    return str;
}
long file2l(char* filepath){
    char* str= file2str(filepath);
    long r=strtol(str,NULL,10);
    free(str);
    return r;
}
double file2double(char* filepath){
    char* str= file2str(filepath);
    double r=strtod(str,NULL);
    free(str);
    return r;
}
#define st_constmsg(st,str) st->msg.txt=malloc(sizeof(str));\
                            memcpy(st->msg.txt,str,sizeof(str));\
                            st->msg.len=sizeof(str)-1;
void get_cpu_usage(stat_node* st)
{
    static FILE* fload=NULL;
    fload=NULL==fload?fopen("/proc/loadavg","r"):fload;
    if (NULL==fload)
    {
        free(st->msg.txt);
        st_constmsg(st,"NO POSIX");
        return;
    }
    rewind(fload);
    double avgs[3];
    char *buffer=malloc(32);buffer[31]=0;
    fread(buffer,1,31,fload);
    for(struct {int i;char* nxt;} s={0} ;s.i<3;s.i++){
        s.nxt= (s.nxt==NULL) ? buffer :s.nxt;
        if(s.nxt != NULL)
            avgs[s.i]=strtod(s.nxt,&s.nxt);
    }
    free(buffer);
    const char *cd_emoji= "\U0001F4BF";
    const char *dvd_emoji="\U0001F4C0";
    const char* format="%s%.2f,%.2f,%.2f";
    char const *c_emoji= st->msg.txt != NULL ?
        st->msg.txt[2]==-110?
                dvd_emoji:
                cd_emoji:
        cd_emoji;

    st->msg.len=snprintf(NULL,0,format,c_emoji,avgs[0],avgs[1],avgs[2]);
    void* tmp=NULL;
    if(NULL==(tmp=realloc(st->msg.txt,st->msg.len+1))) {
        free(st->msg.txt);
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

    if (hnd == NULL){
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
    struct {char* bytes;size_t size;} dmem=
    {.bytes=malloc(0),.size=0};
    curl_easy_setopt(hnd,CURLOPT_WRITEDATA,&dmem);
    CURLcode ret= curl_easy_perform(hnd);
    if (ret)
    {
        st_constmsg(st,"📶 ❌");
        return INT_MAX;
    }

    //curl_easy_cleanup(hnd);
    //hnd = NULL;

    json_object *obj=json_tokener_parse(dmem.bytes);
    free(dmem.bytes);
    json_object *arr= json_object_object_get(obj,"bids");
    if(json_object_get_type(arr) != json_type_array)
    {
        st_constmsg(st,"err: hackers attacking");
        return INT_MAX;
    }
    json_object *arr_0=json_object_array_get_idx(arr,0);
    json_object *val=json_object_array_get_idx(arr_0,0);
    double i=json_object_get_double(val);
    json_object_put(obj);
    return i;
}
void get_xmr(stat_node* st){
    free(st->msg.txt);
    if (system("ping -c 1 -W 1 8.8.8.8 1>/dev/null 2>/dev/null")!=0)
    {
        st_constmsg(st,"📶 ❌");
        return ;
    }
    double i=get_xmr_curl_setup(st);
    if (i ==INT_MAX)
    {
        st_constmsg(st,"📶 ❌");
        return ;
    }

    const char * fmt="XMR:%.2f💵" ;
    st_make_message(st,fmt,i);

}
const char* week_str(int day)
{
    const char* week[]={"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};
    return week[day-1];
}
const char* month_str(int month)
{
    const char* months[]={
        "Jan","Feb","Mar","Apr",
        "May","Jun","Jul","Aug",
        "Sep","Oct","Nov","Dec"};
    return months[month];
}

void get_date_hour(stat_node* st){
    free(st->msg.txt);
    struct tm *tms=localtime(&st->last_secs);
    const char* fmt ="%02i:%02i%s%02i/%s";//week_day day/month-hour:minute
    st_make_message(st,fmt,
            tms->tm_hour, tms->tm_min,
            week_str(tms->tm_wday), tms->tm_mday, month_str(tms->tm_mon));
}
char* smallprintf(char* fmt,...)
{
    va_list ap;
    va_start(ap,fmt);
    size_t len= vsnprintf(NULL,0,fmt,ap);
    va_end(ap);
    char* txt=malloc(len+1);
    txt[len]=0;
    va_start(ap,fmt);
    len= vsnprintf(txt,len+1,fmt,ap);
    va_end(ap);
    return txt;
}
////////////////BATTERY////////////////
#include <dirent.h>
enum power_status{
    p_s_NOBATTERY,
    p_s_batUnknown,
    p_s_batCharging,
    p_s_batDischarging,
    p_s_batNotcharging,
    p_s_batFull
};
typedef struct {double percentage; enum power_status status;} chk_power;
chk_power* check_power()
{
    chk_power* r= malloc(sizeof(chk_power));
    r->status=p_s_NOBATTERY;
    const char *path_folder="/sys/class/power_supply";
    DIR *target_d;
    target_d=opendir(path_folder);
    if (target_d == NULL){
        r->status=p_s_NOBATTERY;
        r->percentage=0;
        return r;
    }
    struct dirent *esp_d;
    char* fname=NULL;
    while(NULL != (esp_d=readdir(target_d))){
        int isBat=!strncmp(esp_d->d_name,"BAT",3);
        if (isBat) {
            fname=smallprintf("%s/%s",path_folder,esp_d->d_name);
            break;
        }
    }
    closedir(target_d);
    if (fname==NULL) {//No battery modules
        r->status=p_s_NOBATTERY;
        r->percentage=100.0;
        return r;
    }
    char *present=smallprintf("%s/%s",fname,"present");
    double c_pre=file2l(present);
    free(present);
    if(c_pre!=1) {
        r->status=p_s_NOBATTERY;
        r->percentage=0;
        return r; }
    //A battery present

    char *charge_full=smallprintf("%s/%s",fname,"charge_full");
    char *charge_now= smallprintf("%s/%s",fname,"charge_now");
    char *status=     smallprintf("%s/%s",fname,"status");
    free(fname);
    long c_full=file2l(charge_full);
    free(charge_full);
    long c_now=file2l(charge_now);
    free(charge_now);
    r->percentage=c_now/(double)c_full;

    char* c_st=file2str(status);
    free(status);
    switch(*c_st){
        case 'U':
            r->status=p_s_batUnknown;
            break;
        case 'C':
            r->status=p_s_batCharging;
            break;
        case 'D':
            r->status=p_s_batDischarging;
            break;
        case 'N':
            r->status=p_s_batNotcharging;
            break;
        case 'F':
            r->status=p_s_batFull;
            break;
        default:
            r->status=p_s_batUnknown;
            break;
    }
    free(c_st);

    return r;
}
void get_power(stat_node* st){
    free(st->msg.txt);
    chk_power* p= check_power();
    switch(p->status){
        case p_s_NOBATTERY:
            st_make_message(st,"%s","🔌");
            break;
        case p_s_batUnknown:
            st_make_message(st,"%s","🔌¿🔋?");
            break;
        case p_s_batCharging:
            st_make_message(st,"%i%s",(int)p->percentage*100,"🔌🔋");
            break;
        case p_s_batDischarging:
            st_make_message(st,"%i%s",(int)(p->percentage*100),p->percentage < 0.4 ?"🪫":"🔋");
            break;
        case p_s_batNotcharging:
            st_make_message(st,"%i%s",(int)(p->percentage*100),"🔌🔋🤯");
            break;
        case p_s_batFull:
            st_make_message(st,"%i%s",(int)(p->percentage*100),"🔋");
            break;
    }
    free(p);

}

