#include "defines.h"
#include "log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern log logger;

static int getOpCode(char *key);


//* �������һ���sort������ʵ�ǳ��ʺϷ�װ�ɶ�̬�������л�����һ��Ҫ���ԣ�
//* ��һ����������������һ���sort�������ڶ�̬����ʱ������ָ�������⣻
//* �������Ǹ��麯����
//* ��������Ļ�����filterһ��Ѻ���Ҳ�ܷ�װ���ţ�
void filter(char *key)
{
    //TODO д�û������SELECT ... WHERE ...
    
    //TODO ��̬��ȡ�ֶ��������SET�Ӿ䣬ͬʱ�󶨲�����ʱ��ǵ����У�

    //TODO �󶨲�����ִ�����
}

void sort(char *key)
{
    //TODO д�û������

    // TODO ��̬��ȡ�ֶ��������������WHERE�� ORDER BY�Ӿ�

    //TODO �󶨲�����ִ��
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
