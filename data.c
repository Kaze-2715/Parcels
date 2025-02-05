// #include "defines.h"
#include "file_io.h"
#include "user_io.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int timeLaterThan(time floor, time cur);
int timeEarlierThan(time ceil, time cur);
int getOpCode(char *key);
void sortFrom(node *list, int rule);
void sortTo(node *list, int rule);
void sortLoadTime(node *list, int rule);
void sortUnloadTime(node *list, int rule);

//* �������һ���sort������ʵ�ǳ��ʺϷ�װ�ɶ�̬�������л�����һ��Ҫ���ԣ�
//* ��һ����������������һ���sort�������ڶ�̬����ʱ������ָ�������⣻
//* �������Ǹ��麯����
//* ��������Ļ�����filterһ��Ѻ���Ҳ�ܷ�װ���ţ�
void filter(char *key)
{
    // TODO �ȼ�������
    FILE *parcelData = openParcel("r");
    node *list = loadLinklist(parcelData);
    fclose(parcelData);

    char input[20] = {0};
    int matchCount = 0;
    int code = 0;
    int mode = 0;
    time floor;
    time ceil;

    code = getOpCode(key);

    switch (code)
    {
    case 1:

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
        break;

    case 2:
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
        break;

    case 3:
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
        break;

    case 4:
        int status;
        printf("Criteria: ");
        scanf("%d", &status);
        getchar();
        //* It��s 0 or 1.
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
        break;
    // TODO Ϊʱ��ɸѡ��Ƶ������жϱ�׼���룬�Լ�����������5��6������ͬ�����Ը���
    case 5:
        mode = 0;
        printf("Select the filter mode, 1 for between, 2 for later, 3 for earlier\n");
        scanf("%d", &mode);
        getchar();

        switch (mode)
        {
        case 1:
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

            break;

        case 2:
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

            break;

        case 3:
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

            break;

        default:
            break;
        }
        break;

    case 6:
        mode = 0;
        printf("Select the filter mode, 1 for between, 2 for later, 3 for earlier\n");
        scanf("%d", &mode);
        getchar();

        switch (mode)
        {
        case 1:
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

            break;

        case 2:
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

            break;

        case 3:
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

            break;

        default:
            break;
        }

    default:
        printf("Invalid key, check your input!\n");
        break;
    }

    // TODO �ͷ�����
    freeLinklist(list);
}

void sort(char *key)
{
    //* ����Ĺؼ��ֶ�������ȥ��ʱ�䣬״̬��key��
    //* ����Ĺ���������ͽ���
    int rule = 0;
    int code = -1;

    printf("Input the rule of sorting, -1 is descending, 1 is ascending: ");
    scanf("%d", &rule);
    getchar();

    if (!((rule == -1) || (rule == 1)))
    {
        do
        {
            printf("Error rule input! Input again: ");
            scanf("%d", &rule);
            getchar();
        } while (!((rule == -1) || (rule == 1)));
    }

    // TODO ��������
    FILE *parcelData = openParcel("r");
    node *list = loadLinklist(parcelData);
    fclose(parcelData);

    // TODO �����ֶ�Ϊ�����룬ȷ���Ӻ���
    code = getOpCode(key);

    if (code == -1)
    {
        do
        {
            printf("Error key input! Input key again: ");
            scanf("%s", key);
        } while (code == -1);
    }

    switch (code)
    {
    case 1:
        sortFrom(list, rule);
        break;

    case 2:
        sortTo(list, rule);
        break;

    case 5:
        sortLoadTime(list, rule);
        break;

    case 6:
        sortUnloadTime(list, rule);
        break;

    default:
        break;
    }

    // TODO ж������
    freeLinklist(list);
}

// TODO ���������ʱ��ȽϺ�����������Ϊ��ʱ����ֵӦ��������
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
    //* ���� year ����ȵ�
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
    //* ����month����ȵ�
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
    //* ����day����ȵ�
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
    //* ����hour����ȵ�
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
    //* ����minute����ȵ�
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
    //* ����second����ȵ�

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
    //* ���� year ����ȵ�
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
    //* ����month����ȵ�
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
    //* ����day����ȵ�
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
    //* ����hour����ȵ�
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
    //* ����minute����ȵ�
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
    //* ����second����ȵ�

    return 1;
}

int getOpCode(char *key)
{
    int code = -1;
    // TODO ���ж�Ҫɸѡ�ĸ��ֶΣ�Ȼ��strcasecmp���ߵ�����С�жϱȽϱȽϼ���
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
    //TODO ʹ��ѡ��������ʵ������
    //*���ѭ��ʵ�ֱ�����������ڵ�
    //*�ڲ�ѭ��ʵ�ֽ������бȽ�
    //*��Ҫ��ָ�������1.��ʶ��ǰȷ����λ 2.��ʶѡ���λ
    for (node *cur = list; cur->next != NULL; cur = cur ->next)
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

    //TODO ���������
    for (node *ptr = list; ptr != NULL; ptr = ptr->next)
    {
        outputParcel(&(ptr->data));
    }

    //TODO �ͷ�����
    //freeLinklist(list);
}

void sortTo(node *list, int rule)
{
    // TODO ʹ��ѡ��������ʵ������
    //*���ѭ��ʵ�ֱ�����������ڵ�
    //*�ڲ�ѭ��ʵ�ֽ������бȽ�
    //*��Ҫ��ָ�������1.��ʶ��ǰȷ����λ 2.��ʶѡ���λ
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

    // TODO ���������
    for (node *ptr = list; ptr != NULL; ptr = ptr->next)
    {
        outputParcel(&(ptr->data));
    }

    // TODO �ͷ�����
    freeLinklist(list);
}

void sortLoadTime(node *list, int rule)
{
    // TODO ʹ��ѡ��������ʵ������
    //*���ѭ��ʵ�ֱ�����������ڵ�
    //*�ڲ�ѭ��ʵ�ֽ������бȽ�
    //*��Ҫ��ָ�������1.��ʶ��ǰȷ����λ 2.��ʶѡ���λ
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

    // TODO ���������
    for (node *ptr = list; ptr != NULL; ptr = ptr->next)
    {
        outputParcel(&(ptr->data));
    }

    // TODO �ͷ�����
    freeLinklist(list);
}

// void sortLoadTime(node *list, int rule)
// {
//     // TODO ʹ��ѡ��������ʵ������
//     //*���ѭ��ʵ�ֱ�����������ڵ�
//     //*�ڲ�ѭ��ʵ�ֽ������бȽ�
//     //*��Ҫ��ָ�������1.��ʶ��ǰȷ����λ 2.��ʶѡ���λ
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

//     // TODO ���������
//     for (node *ptr = list; ptr != NULL; ptr = ptr->next)
//     {
//         outputParcel(&(ptr->data));
//     }

//     // TODO �ͷ�����
//     freeLinklist(list);
// }

void sortUnloadTime(node *list, int rule)
{
    // TODO ʹ��ѡ��������ʵ������
    //*���ѭ��ʵ�ֱ�����������ڵ�
    //*�ڲ�ѭ��ʵ�ֽ������бȽ�
    //*��Ҫ��ָ�������1.��ʶ��ǰȷ����λ 2.��ʶѡ���λ
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

    // TODO ���������
    for (node *ptr = list; ptr != NULL; ptr = ptr->next)
    {
        outputParcel(&(ptr->data));
    }

    // TODO �ͷ�����
    freeLinklist(list);
}