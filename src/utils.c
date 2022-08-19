#include "utils.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

char* smallprintf(const char* fmt,...)
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
char* vsmallprintf(const char* fmt,va_list ap)
{
    va_list ap2;
    va_copy(ap2, ap);
    size_t len= vsnprintf(NULL,0,fmt,ap);
    char* txt=malloc(len+1);
    txt[len]=0;
    len= vsnprintf(txt,len+1,fmt,ap2);
    va_end(ap2);
    return txt;
}
void logging_init()
{
    char * res=smallprintf("%s/.cache/status4dwm",getenv("HOME"));
    int i=mkdir(res,S_IRWXU);
#include <errno.h>
    if (0!=i)
        if (EEXIST != errno)
        {
            printf("%s",strerror(errno));
            exit(2);
        }
    char* res2=smallprintf("%s/log",res);
    free(res);
    freopen(res2,"a+",stdout);
    free(res2);
    setvbuf(stdout, NULL, _IONBF, 0);
}
#include <time.h>
void u_log(char* fmt, ...){
    va_list ap;
    time_t now=time(NULL);
    struct tm *tms=localtime(&now);
    const char* datefmt ="[%02i:%02i|%02i/%02i/%04i]";//week_day day/month-hour:minute
    char* date=smallprintf(datefmt,
            tms->tm_hour, tms->tm_min,
            tms->tm_mday, tms->tm_mon,tms->tm_year+1900);
    va_start(ap,fmt);
    char* tolog=vsmallprintf(fmt,ap); va_end(ap);
    char* result=smallprintf("%s%s",date,tolog);
    free(date); free(tolog);
    printf("%s\r\n",result);
    free(result);
}

