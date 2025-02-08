#include "log.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>

void initLog()
{
    logger.logfile = fopen("E:/BaiduSyncdisk/03_CODE/VSCode_Workspace/Infomation_Manager/log.txt", "a");
    logger.User = &currentUser;
    logger.print = printLog;
    logger.destroy = destroyLog;
    return;
}

void destroyLog()
{
    fclose(logger.logfile);
    return;
}

void printLog(Level level, const char *format, ...)
{
    time_t now;
    struct tm *currentTime;
    va_list args;
    char timeStr[24];
    char levelStr[8];
    char userinput[64];
    char logStr[128];

    time(&now);
    currentTime = localtime(&now);
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", currentTime);

    switch (level)
    {
    case INFO:
        strcpy(levelStr, "INFO");
        break;
    
    case ERROR:
        strcat(levelStr, "ERROR");
        break;

    default:
        break;
    }

    snprintf(logStr, 64, "%s %s %s -", timeStr, levelStr, currentUser.username);

    va_start(args, format);
    vsnprintf(userinput, sizeof(userinput), format, args);

    strcat(logStr, userinput);
    fprintf(logger.logfile, "%s\n", logStr);
    return;
}