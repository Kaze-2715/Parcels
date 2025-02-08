// #include "defines.h"
#include "file_io.h"
#include "user_io.h"
#include "log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern log logger;

static int timeLaterThan(time floor, time cur);
static int timeEarlierThan(time ceil, time cur);
static int getOpCode(char *key);
void sortFrom(node *list, int rule);
void sortTo(node *list, int rule);
void sortLoadTime(node *list, int rule);
void sortUnloadTime(node *list, int rule);

//* 这里面的一大堆sort函数其实非常适合封装成多态函数，有机会了一定要试试！
//* 用一个函数表来加载那一大堆sort函数，在多态调用时传函数指针解决问题；
//* 就是用那个虚函数表
//* 这样来想的话，那filter一大堆函数也能封装，嗯！
void filter(char *key)
{
    // TODO 先加载链表
    FILE *parcelData = openParcel("r");
    node *list = loadLinklist(parcelData);
    fclose(parcelData);

    typedef enum Criteria
    {
        FROM = 1,
        TO,
        ID,
        STATUS,
        LOADTIME,
        UNLOADTIME
    } Criteria;

    typedef enum timeFilter
    {
        BETWEEN = 1,
        LATER,
        EARLIER
    } timefilter;

    char input[20] = {0};
    int matchCount = 0;
    int criteria = 0;
    int mode = 0;
    time floor;
    time ceil;

    criteria = getOpCode(key);

    switch (criteria)
    {
    case FROM:

        printf("Criteria: ");
        scanf("%s", input);
        getchar();
        for (node *ptr = list; ptr != NULL; ptr = ptr->next)
        {
            if (!strcasecmp(ptr->data.from, input))
            {
                if (matchCount == 0)
                {
                    printf("Matched!\n");
                }
                outputParcel(&(ptr->data));
                matchCount++;
            }
        }
        //* 给后面输出日志用
        strcpy(input, "FROM");
        break;

    case TO:
        printf("Criteria: ");
        scanf("%s", input);
        getchar();
        for (node *ptr = list; ptr != NULL; ptr = ptr->next)
        {
            if (!strcasecmp(ptr->data.to, input))
            {
                if (matchCount == 0)
                {
                    printf("Matched!\n");
                }
                outputParcel(&(ptr->data));
                matchCount++;
            }
        }
        strcpy(input, "TO");
        break;

    case ID:
        printf("Criteria: ");
        scanf("%s", input);
        getchar();
        for (node *ptr = list; ptr != NULL; ptr = ptr->next)
        {
            if (!strcasecmp(ptr->data.ID, input))
            {
                if (matchCount == 0)
                {
                    printf("Matched!\n");
                }
                outputParcel(&(ptr->data));
                matchCount++;
            }
        }
        strcpy(input, "ID");
        break;

    case STATUS:
        int status;
        printf("Criteria: ");
        scanf("%d", &status);
        getchar();
        //* It‘s 0 or 1.
        for (node *ptr = list; ptr != NULL; ptr = ptr->next)
        {
            if (ptr->data.state == status)
            {
                if (matchCount == 0)
                {
                    printf("Matched!\n");
                }
                outputParcel(&(ptr->data));
                matchCount++;
            }
        }
        strcpy(input, "STATUS");
        break;
    // TODO 为时间筛选设计单独的判断标准输入，以及评判条件，5和6基本相同，可以复制
    case LOADTIME:
        int Timerfilter = 0;
        printf("Select the filter mode, 1 for between, 2 for later, 3 for earlier\n");
        scanf("%d", &Timerfilter);
        getchar();

        strcpy(input, "LOADTIME by ");
        switch (Timerfilter)
        {
        case BETWEEN:
            printf("Earliest time: ");
            scanf("%d %d %d %d %d %d", &floor.year, &floor.month, &floor.day, &floor.hour, &floor.minute, &floor.second);
            getchar();
            printf("Latest time: ");
            scanf("%d %d %d %d %d %d", &ceil.year, &ceil.month, &ceil.day, &ceil.hour, &ceil.minute, &ceil.second);
            getchar();

            for (node *ptr = list; ptr != NULL; ptr = ptr->next)
            {
                if ((timeLaterThan(floor, ptr->data.load_time)) && (timeEarlierThan(ceil, ptr->data.load_time)))
                {
                    if (matchCount == 0)
                    {
                        printf("Matched!\n");
                    }
                    outputParcel(&(ptr->data));
                    matchCount++;
                }
            }
            strcat(input, "BETWEEN");
            break;

        case LATER:
            printf("Earliest time: ");
            scanf("%d %d %d %d %d %d", &floor.year, &floor.month, &floor.day, &floor.hour, &floor.minute, &floor.second);
            getchar();
            for (node *ptr = list; ptr != NULL; ptr = ptr->next)
            {
                if (timeLaterThan(floor, ptr->data.load_time))
                {
                    if (matchCount == 0)
                    {
                        printf("Matched!\n");
                    }
                    outputParcel(&(ptr->data));
                    matchCount++;
                }
            }
            strcat(input, "LATER");
            break;

        case EARLIER:
            printf("Latest time: ");
            scanf("%d %d %d %d %d %d", &ceil.year, &ceil.month, &ceil.day, &ceil.hour, &ceil.minute, &ceil.second);
            getchar();

            for (node *ptr = list; ptr != NULL; ptr = ptr->next)
            {
                if (timeEarlierThan(ceil, ptr->data.load_time))
                {
                    if (matchCount == 0)
                    {
                        printf("Matched!\n");
                    }
                    outputParcel(&(ptr->data));
                    matchCount++;
                }
            }
            strcat(input, "EARLIER");
            break;

        default:
            break;
        }
        break;

    case UNLOADTIME:
        int Timefilter = 0;
        printf("Select the filter mode, 1 for between, 2 for later, 3 for earlier\n");
        scanf("%d", &Timefilter);
        getchar();

        strcpy(input, "UNLOADTIME by ");

        switch (Timefilter)
        {
        case BETWEEN:
            printf("Earliest time: ");
            scanf("%d %d %d %d %d %d", &floor.year, &floor.month, &floor.day, &floor.hour, &floor.minute, &floor.second);
            getchar();
            printf("Latest time: ");
            scanf("%d %d %d %d %d %d", &ceil.year, &ceil.month, &ceil.day, &ceil.hour, &ceil.minute, &ceil.second);
            getchar();

            for (node *ptr = list; ptr != NULL; ptr = ptr->next)
            {
                if ((timeLaterThan(floor, ptr->data.unload_time)) && (timeEarlierThan(ceil, ptr->data.unload_time)))
                {
                    if (matchCount == 0)
                    {
                        printf("Matched!\n");
                    }
                    outputParcel(&(ptr->data));
                    matchCount++;
                }
            }
            strcat(input, "BETWEEN");
            break;

        case LATER:
            printf("Earliest time: ");
            scanf("%d %d %d %d %d %d", &floor.year, &floor.month, &floor.day, &floor.hour, &floor.minute, &floor.second);
            getchar();
            for (node *ptr = list; ptr != NULL; ptr = ptr->next)
            {
                if (timeLaterThan(floor, ptr->data.unload_time))
                {
                    if (matchCount == 0)
                    {
                        printf("Matched!\n");
                    }
                    outputParcel(&(ptr->data));
                    matchCount++;
                }
            }
            strcat(input, "LATER");
            break;

        case EARLIER:
            printf("Latest time: ");
            scanf("%d %d %d %d %d %d", &ceil.year, &ceil.month, &ceil.day, &ceil.hour, &ceil.minute, &ceil.second);
            getchar();

            for (node *ptr = list; ptr != NULL; ptr = ptr->next)
            {
                if (timeEarlierThan(ceil, ptr->data.unload_time))
                {
                    if (matchCount == 0)
                    {
                        printf("Matched!\n");
                    }
                    outputParcel(&(ptr->data));
                    matchCount++;
                }
            }
            strcat(input, "EARLIER");
            break;

        default:
            break;
        }

    default:
        printf("Invalid key, check your input!\n");
        strcpy(input, "INVALID KEY");
        break;
    }
    logger.print(INFO, "filtered data with %s", input);

    // TODO 释放链表
    freeLinklist(list);
}

void sort(char *key)
{
    typedef enum Criteria
    {
        FROM = 1,
        TO,
        ID,
        STATUS,
        LOADTIME,
        UNLOADTIME
    } Criteria;

    typedef enum Rule
    {
        DESCENDING = -1,
        ASCENDING = 1
    } Rule;

    //* 排序的关键字段有来，去，时间，状态（key）
    //* 排序的规则有升序和降序
    int rule = 0;
    int code = -1;
    char criteiriaStr[12];
    char ruleStr[12];

    printf("Input the rule of sorting, -1 is descending, 1 is ascending: ");
    scanf("%d", &rule);
    getchar();

    if (!((rule == DESCENDING) || (rule == ASCENDING)))
    {
        do
        {
            printf("Error rule input! Input again: ");
            scanf("%d", &rule);
            getchar();
        } while (!((rule == DESCENDING) || (rule == ASCENDING)));
    }
    if (rule == DESCENDING)
    {
        strcpy(ruleStr, "DESCENDING");
    }
    else
    {
        strcpy(ruleStr, "ASCENDING");
    }

    // TODO 加载链表
    FILE *parcelData = openParcel("r");
    node *list = loadLinklist(parcelData);
    fclose(parcelData);

    // TODO 翻译字段为操作码，确定子函数
    code = getOpCode(key);

    if (code == -1)
    {
        do
        {
            printf("Error key input! Input key again: ");
            scanf("%s", key);
            code = getOpCode(key);
        } while (code == -1);
    }

    switch (code)
    {
    case FROM:
        sortFrom(list, rule);
        strcpy(criteiriaStr, "FROM");
        break;

    case TO:
        sortTo(list, rule);
        strcpy(criteiriaStr, "TO");
        break;

    case LOADTIME:
        sortLoadTime(list, rule);
        strcpy(criteiriaStr, "LOADTIME");
        break;

    case UNLOADTIME:
        sortUnloadTime(list, rule);
        strcpy(criteiriaStr, "UNLOADTIME");
        break;

    default:
        strcpy(criteiriaStr, "INVALIDKEY");
        break;
    }

    logger.print(INFO, "sorted the data with '%s' by '%s'", criteiriaStr, ruleStr);
    // TODO 卸载链表
    freeLinklist(list);
}

// TODO 完成这两个时间比较函数，当条件为真时返回值应该是正数
int timeLaterThan(time floor, time cur)
{
    if (cur.year > floor.year)
    {
        return 1;
    }
    else
    {
        if (cur.year < floor.year)
        {
            return 0;
        }
    }
    //* 现在 year 是相等的
    if (cur.month > floor.month)
    {
        return 1;
    }
    else
    {
        if (cur.month < floor.month)
        {
            return 0;
        }
    }
    //* 现在month是相等的
    if (cur.day > floor.day)
    {
        return 1;
    }
    else
    {
        if (cur.day < floor.day)
        {
            return 0;
        }
    }
    //* 现在day是相等的
    if (cur.hour > floor.hour)
    {
        return 1;
    }
    else
    {
        if (cur.hour < floor.hour)
        {
            return 0;
        }
    }
    //* 现在hour是相等的
    if (cur.minute > floor.minute)
    {
        return 1;
    }
    else
    {
        if (cur.minute < floor.minute)
        {
            return 0;
        }
    }
    //* 现在minute是相等的
    if (cur.second > floor.second)
    {
        return 1;
    }
    else
    {
        if (cur.second < floor.second)
        {
            return 0;
        }
    }
    //* 现在second是相等的

    return 1;
}

int timeEarlierThan(time ceil, time cur)
{
    if (cur.year < ceil.year)
    {
        return 1;
    }
    else
    {
        if (cur.year > ceil.year)
        {
            return 0;
        }
    }
    //* 现在 year 是相等的
    if (cur.month < ceil.month)
    {
        return 1;
    }
    else
    {
        if (cur.month > ceil.month)
        {
            return 0;
        }
    }
    //* 现在month是相等的
    if (cur.day < ceil.day)
    {
        return 1;
    }
    else
    {
        if (cur.day > ceil.day)
        {
            return 0;
        }
    }
    //* 现在day是相等的
    if (cur.hour < ceil.hour)
    {
        return 1;
    }
    else
    {
        if (cur.hour > ceil.hour)
        {
            return 0;
        }
    }
    //* 现在hour是相等的
    if (cur.minute < ceil.minute)
    {
        return 1;
    }
    else
    {
        if (cur.minute > ceil.minute)
        {
            return 0;
        }
    }
    //* 现在minute是相等的
    if (cur.second < ceil.second)
    {
        return 1;
    }
    else
    {
        if (cur.second > ceil.second)
        {
            return 0;
        }
    }
    //* 现在second是相等的

    return 1;
}

int getOpCode(char *key)
{
    int code = -1;
    // TODO 先判断要筛选哪个字段，然后strcasecmp或者单纯大小判断比较比较即可
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

void sortFrom(node *list, int rule)
{
    // TODO 使用选择排序来实现排序
    //*外层循环实现遍历所有链表节点
    //*内层循环实现进行所有比较
    //*需要的指针变量：1.标识当前确定的位 2.标识选择的位
    for (node *cur = list; cur->next != NULL; cur = cur->next)
    {
        node *max = cur->next;
        for (node *select = cur->next; select != NULL; select = select->next)
        {
            if ((rule * strcasecmp(max->data.from, select->data.from)) > 0)
            {
                max = select;
            }
        }
        parcel tmp = cur->data;
        cur->data = max->data;
        max->data = tmp;
    }

    // TODO 输出排序结果
    for (node *ptr = list; ptr != NULL; ptr = ptr->next)
    {
        outputParcel(&(ptr->data));
    }

    // TODO 释放链表
    // freeLinklist(list);
}

void sortTo(node *list, int rule)
{
    // TODO 使用选择排序来实现排序
    //*外层循环实现遍历所有链表节点
    //*内层循环实现进行所有比较
    //*需要的指针变量：1.标识当前确定的位 2.标识选择的位
    for (node *cur = list; cur->next != NULL; cur = cur->next)
    {
        node *max = cur->next;
        for (node *select = cur->next; select != NULL; select = select->next)
        {
            if ((rule * strcasecmp(max->data.to, select->data.to)) > 0)
            {
                max = select;
            }
        }
        parcel tmp = cur->data;
        cur->data = max->data;
        max->data = tmp;
    }

    // TODO 输出排序结果
    for (node *ptr = list; ptr != NULL; ptr = ptr->next)
    {
        outputParcel(&(ptr->data));
    }

    // TODO 释放链表
    // freeLinklist(list);
}

void sortLoadTime(node *list, int rule)
{
    // TODO 使用选择排序来实现排序
    //*外层循环实现遍历所有链表节点
    //*内层循环实现进行所有比较
    //*需要的指针变量：1.标识当前确定的位 2.标识选择的位
    switch (rule)
    {
    case 1:
        for (node *cur = list; cur->next != NULL; cur = cur->next)
        {
            node *min = cur->next;
            for (node *select = cur->next; select != NULL; select = select->next)
            {
                if (timeEarlierThan(min->data.load_time, select->data.load_time))
                {
                    min = select;
                }
            }
            parcel tmp = cur->data;
            cur->data = min->data;
            min->data = tmp;
        }
        break;

    case -1:
        for (node *cur = list; cur->next != NULL; cur = cur->next)
        {
            node *max = cur->next;
            for (node *select = cur->next; select != NULL; select = select->next)
            {
                if (timeLaterThan(max->data.load_time, select->data.load_time))
                {
                    max = select;
                }
            }
            parcel tmp = cur->data;
            cur->data = max->data;
            max->data = tmp;
        }
        break;

    default:
        break;
    }

    // TODO 输出排序结果
    for (node *ptr = list; ptr != NULL; ptr = ptr->next)
    {
        outputParcel(&(ptr->data));
    }

    // TODO 释放链表
    // freeLinklist(list);
}

// void sortLoadTime(node *list, int rule)
// {
//     // TODO 使用选择排序来实现排序
//     //*外层循环实现遍历所有链表节点
//     //*内层循环实现进行所有比较
//     //*需要的指针变量：1.标识当前确定的位 2.标识选择的位
//     switch (rule)
//     {
//     case 1:
//         for (node *cur = list; cur->next != NULL; cur = cur->next)
//         {
//             node *min = cur->next;
//             for (node *select = cur->next; select != NULL; select = select->next)
//             {
//                 if (timeEarlierThan(min->data.load_time, select->data.load_time))
//                 {
//                     min = select;
//                 }
//             }
//             parcel tmp = cur->data;
//             cur->data = min->data;
//             min->data = tmp;
//         }
//         break;

//     case -1:
//         for (node *cur = list; cur->next != NULL; cur = cur->next)
//         {
//             node *max = cur->next;
//             for (node *select = cur->next; select != NULL; select = select->next)
//             {
//                 if (timeLaterThan(max->data.load_time, select->data.load_time))
//                 {
//                     max = select;
//                 }
//             }
//             parcel tmp = cur->data;
//             cur->data = max->data;
//             max->data = tmp;
//         }
//         break;

//     default:
//         break;
//     }

//     // TODO 输出排序结果
//     for (node *ptr = list; ptr != NULL; ptr = ptr->next)
//     {
//         outputParcel(&(ptr->data));
//     }

//     // TODO 释放链表
//     freeLinklist(list);
// }

void sortUnloadTime(node *list, int rule)
{
    // TODO 使用选择排序来实现排序
    //*外层循环实现遍历所有链表节点
    //*内层循环实现进行所有比较
    //*需要的指针变量：1.标识当前确定的位 2.标识选择的位
    switch (rule)
    {
    case 1:
        for (node *cur = list; cur->next != NULL; cur = cur->next)
        {
            node *min = cur->next;
            for (node *select = cur->next; select != NULL; select = select->next)
            {
                if (timeEarlierThan(min->data.unload_time, select->data.unload_time))
                {
                    min = select;
                }
            }
            parcel tmp = cur->data;
            cur->data = min->data;
            min->data = tmp;
        }
        break;

    case -1:
        for (node *cur = list; cur->next != NULL; cur = cur->next)
        {
            node *max = cur->next;
            for (node *select = cur->next; select != NULL; select = select->next)
            {
                if (timeLaterThan(max->data.unload_time, select->data.unload_time))
                {
                    max = select;
                }
            }
            parcel tmp = cur->data;
            cur->data = max->data;
            max->data = tmp;
        }
        break;

    default:
        break;
    }

    // TODO 输出排序结果
    for (node *ptr = list; ptr != NULL; ptr = ptr->next)
    {
        if (ptr->data.unload_time.year)
        {
            continue;
        }

        outputParcel(&(ptr->data));
    }

    // TODO 释放链表
    // freeLinklist(list);
}