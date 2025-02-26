#include "defines.h"
#include "log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern log logger;

static int getOpCode(char *key);


//* 这里面的一大堆sort函数其实非常适合封装成多态函数，有机会了一定要试试！
//* 用一个函数表来加载那一大堆sort函数，在多态调用时传函数指针解决问题；
//* 就是用那个虚函数表
//* 这样来想的话，那filter一大堆函数也能封装，嗯！
void filter(char *key)
{
    //TODO 写好基础语句SELECT ... WHERE ...
    
    //TODO 动态获取字段名，组合SET子句，同时绑定参数（时间记得特判）

    //TODO 绑定参数，执行语句
}

void sort(char *key)
{
    //TODO 写好基础语句

    // TODO 动态获取字段名和升降序，组合WHERE， ORDER BY子句

    //TODO 绑定参数并执行
}

int getOpCode(char *key)
{
    int code = -1;
    if (!strcasecmp(key, "from"))
    {
        code = 1;
    }
    else if (!strcasecmp(key, "to"))
    {
        code = 2;
    }
    else if (!strcasecmp(key, "ID"))
    {
        code = 3;
    }
    else if (!strcasecmp(key, "status"))
    {
        code = 4;
    }
    else if (!strcasecmp(key, "load_time"))
    {
        code = 5;
    }
    else if (!strcasecmp(key, "unload_time"))
    {
        code = 6;
    }

    return code;
}
