#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>     // fsync()
#include <time.h>

#include "misc.h"

extern FILE *Log_stream;
extern int Debug;

/***************************************************************************
 *  Description:
 *      Log messages to stream of choice, usually either stderr if running
 *      as a foreground process, or PREFIX/var/log/lpjs by default
 *      if daemonized.
 *
 *  History: 
 *  Date        Name        Modification
 *  2021-09-28  Jason Bacon Begin
 ***************************************************************************/

int     ipmi_log(const char *format, ...)

{
    int         status;
    va_list     ap;
    
    // Code duplicated in ipmi_debug(), but not worth factoring out
    va_start(ap, format);
    
    fprintf(Log_stream, "%s ", xt_str_localtime("%m-%d %H:%M:%S"));
    
    // FIXME: Add time stamp?
    status = vfprintf(Log_stream, format, ap);
    
    // Commit immediately so last message in the log is not
    // misleading in the event of a crash
    fflush(Log_stream);
    fsync(fileno(Log_stream));
    
    va_end(ap);
    
    return status;
}


int     ipmi_debug(const char *format, ...)

{
    int         status;
    va_list     ap;
    
    if ( Debug )
    {
        // Code duplicated in ipmi_log), but not worth factoring out
        va_start(ap, format);

        fprintf(Log_stream, "%s ", xt_str_localtime("%m-%d %H:%M:%S"));
        
        // FIXME: Add time stamp?
        status = vfprintf(Log_stream, format, ap);
        
        // Commit immediately so last message in the log is not
        // misleading in the event of a crash
        fflush(Log_stream);
        fsync(fileno(Log_stream));
        
        va_end(ap);
        return status;
    }
    else
        return 0;
}


char    *xt_str_localtime(const char *format)

{
    time_t      time_sec;
    struct tm   *tm;
    static char str[TIME_STR_MAX + 1];
    
    time(&time_sec);
    tm = localtime(&time_sec);
    strftime(str, TIME_STR_MAX + 1, format, tm);
    
    return str;
}
