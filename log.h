//* 日志系统应当包含以下信息：
//* 1.时间戳
//* 2.日志级别
//* 3.用户
//* 4.操作

#ifndef LOG_H_
#define LOG_H_
#include <stdio.h>
#include "user.h"

typedef struct Log log;

typedef enum level
{
    INFO = 0,
    ERROR
} Level;

struct Log
{
    FILE *logfile;
    user *User;
    void (*print)(Level Level, const char *format, ...);
    void (*destroy)();
};

void initLog();
void printLog(Level Level, const char *format, ...);
void destroyLog();

extern user currentUser;
extern log logger;

#endif