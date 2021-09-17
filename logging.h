//================================================================
//Author: Sanskar Chand <sanskarchand@gmail.com>
//Date  : 2021-09-17
//================================================================
#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>
#include <string.h>
#include <time.h>

/*
 * A simple header-only logging library
 */

#define LOG_SEP_CHAR     '#'
#define LOG_DEF_FILENAME "chitralok.log"


typedef enum {
    DEBUG = 0,
    INFO, 
    WARNING,
    ERROR,
    FATAL
} LogType;

const char* logtype_map[] = {
    [DEBUG] = "Debug",
    [INFO] = "Info",
    [WARNING]  = "Warning",
    [ERROR] = "Error",
    [FATAL] = "Fatal"
};


FILE *_stream;

//function sigs
void _log_init_msg();


void log_init(FILE *stream) {
    if (NULL != stream) {
        _stream = stream;
        return;
    }

    //if NULL is provided, we use the default file
    _stream = fopen(LOG_DEF_FILENAME, "a");

    _log_init_msg();

}

void log_close() {
    fclose(_stream);
}

//using functions instead of macros
void _log_init_msg()
{
    time_t ltime;
    ltime = time(NULL);
    char *timestamp = asctime(localtime(&ltime)); //returned string is statically allocated
    timestamp[strlen(timestamp)-1] = 0;


    //print a separating line
    char seps[65];
    memset(seps, LOG_SEP_CHAR, 64);
    seps[64] = 0;
    
    fprintf(_stream, "%s\n", seps); 
    fprintf(_stream, "%s\t%s\t%s\n", timestamp, logtype_map[INFO], "Started new log");
}

void _log(LogType mode, const char *msg)
{
    time_t ltime;
    ltime = time(NULL);
    char *timestamp = asctime(localtime(&ltime)); 
    timestamp[strlen(timestamp)-1] = 0;

    fprintf(_stream, "%s\t%s\t%s\n", timestamp, logtype_map[mode], msg);
}

void logging_info(const char *msg)
{
    _log(INFO, msg);
}

#endif //LOGGING_H
