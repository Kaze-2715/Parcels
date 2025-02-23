#include "defines.h"
#include "user_io.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sqlite3.h>

extern log logger;
extern sqlite3 *parcelhub;
extern sqlite3_stmt *statement;
extern char *errorMessage;
extern int databaseStatus;

node *loadLinklist(FILE *fp);
void freeLinklist(node *list);
int matchedLine(node *buffer, char *ID);
static int getTime(char *userInput, char *timeStr);

//* @brief To open the "parcels.txt"
//! @attention Need the whole path to the file, which is not good for deploying in other computers.
FILE *openParcel(char *mode)
{
    FILE *fp = fopen("E:/BaiduSyncdisk/03_CODE/VSCode_Workspace/Infomation_Manager/parcels.txt", mode);
    if (fp == NULL)
    {
        perror("Open parcel data");
    }
    return fp;
}

//* @brief To open the "log.txt"
//! @attention Same as above
// FILE *openLog(char *mode)
// {
//     FILE *fp = fopen("E:/BaiduSyncdisk/03_CODE/VSCode_Workspace/parcel_service/log.txt", mode);
//     if (fp == NULL)
//     {
//         perror("Open log data");
//     }
//     return fp;
// }

//* @author Kaze
//* @brief To read a line from a given txt file(FILE *fp), then return a struct variable of parcel's data.
//* @History Fixed the failed to read situation;
//*          Removed the useless status judgement;
int readLine(parcel *one, FILE *parcelData)
{
    int readState; //* Signal for if it is successfully read.
    char buffer[1024] = {0};
    fgets(buffer, 1024, parcelData);
    one->state = 0; //*Set the loading status to "Not unloaded" on default.
    readState = sscanf(buffer, "%[^,],%[^,],%[^,],%d,%d.%d.%d.%d.%d.%d,%d.%d.%d.%d.%d.%d\n", one->from, one->to, one->ID, &one->state, &one->load_time.year, &one->load_time.month, &one->load_time.day, &one->load_time.hour, &one->load_time.minute, &one->load_time.second, &one->unload_time.year, &one->unload_time.month, &one->unload_time.day, &one->unload_time.hour, &one->unload_time.minute, &one->unload_time.second);

    return (readState == 16) ? 1 : 0;
    //* Return the read state, a positive number is "succcess", 0 or negative is "failed".
}

void writeLine(parcel *one, FILE *parcelData)
{

    int sign = fprintf(parcelData, "%s,%s,%s,%d,%d.%d.%d.%d.%d.%d,%d.%d.%d.%d.%d.%d\n", one->from, one->to, one->ID, one->state, one->load_time.year, one->load_time.month, one->load_time.day, one->load_time.hour, one->load_time.minute, one->load_time.second, one->unload_time.year, one->unload_time.month, one->unload_time.day, one->unload_time.hour, one->unload_time.minute, one->unload_time.second);

    if (sign < 0) //* To check if the data is successfully written.
    {
        perror("writeLine");
    }
    // else
    // {
    //     printf("Write line complete.\n");
    // }
}

//* History: Fixed the freeing of linklist;
//*          Fixed the match line of head node;
void deleteLine(char *ID)
{
    char delete[64] = "DELETE FROM PARCELS WHERE id = ?;";

    sqlite3_prepare_v2(parcelhub, delete, -1, &statement, NULL);
    sqlite3_bind_text(statement, 1, ID, -1, SQLITE_STATIC);
    databaseStatus = sqlite3_step(statement);

    if (databaseStatus == SQLITE_DONE)
    {
        printf("Deleted parcel: %s", ID);
        logger.print(INFO, "deleted parcel: %s", ID);
    }
    else
    {
        printf("Failed to delete for database error: %s", sqlite3_errmsg(parcelhub));
        logger.print(ERROR, "failed to delete for database error: %s", sqlite3_errmsg(parcelhub));
    }

    return;

    // FILE *parcelData = openParcel("r");
    // node *list = loadLinklist(parcelData);
    // node *matched = NULL;
    // fclose(parcelData);

    // for (node *cur = list, *before = NULL; cur != NULL; before = cur, cur = cur->next)
    // {
    //     if (matchedLine(cur, ID))
    //     {
    //         if (before == NULL)
    //         {
    //             list = cur->next;
    //             matched = cur;
    //         }
    //         else
    //         {
    //             before->next = cur->next;
    //             matched = cur;
    //         }
    //     }
    // }

    // if (matched != NULL)
    // {
    //     free(matched);
    // }

    // // TODO ��������ɾ���õ�����д�����ļ�����,Ȼ������free��
    // parcelData = openParcel("w+");

    // for (node *cur = list, *next = NULL; cur != NULL;)
    // {
    //     next = cur->next;
    //     writeLine(&(cur->data), parcelData);
    //     free(cur);
    //     cur = next;
    // }

    // // TODO �ر��ļ�
    // fclose(parcelData);
}

//* Fixed the ending condition of the 'while' loop;
node *loadLinklist(FILE *fp)
{
    parcel *buffer = malloc(sizeof(parcel));
    node *list = NULL;
    node *ptr = NULL;
    while (readLine(buffer, fp) != EOF)
    {
        node *new = malloc(sizeof(node));
        if (new == NULL)
        {
            perror("Linklist");
        }

        new->data = *buffer;
        new->next = list;
        list = new;
    }
    return list;
}

//* Fixed the retval of strcmp();
int matchedLine(node *buffer, char *ID)
{
    int match = 0;
    if (!strcmp(buffer->data.ID, ID))
    {
        match = 1;
    }

    return match;
}

void updateLine(char *ID)
{
    //* ---------������----------
    char *query = "SELECT * FROM PARCELS WHERE id = ?;";
    char setClause[256] = "UPDATE PARCELS SET ";
    char *fields[6] = {"origin", "destination", "status", "intime", "outtime"};
    char userInput[64] = "";
    char newValue[5][64] = {0};
    char *separation = ",";
    char *token = NULL;

    //* ------------------------

    // TODO ��֤�ֶ�

    // TODO ��ȡʱ������

    // TODO ��ѯID����ʾ��ǰ����
    sqlite3_prepare_v2(parcelhub, query, -1, statement, NULL);
    sqlite3_bind_text(statement, 1, ID, -1, SQLITE_STATIC);
    databaseStatus = sqlite3_step(statement);

    if (databaseStatus == SQLITE_ROW)
    {
        printf("Is this exactly the parcel you want to update?\n");
        printf("%s | %s | %s | %d | %s | %s\n",
               sqlite3_column_text(statement, 0),
               sqlite3_column_text(statement, 1),
               sqlite3_column_text(statement, 2),
               sqlite3_column_int(statement, 3),
               sqlite3_column_text(statement, 4),
               sqlite3_column_text(statement, 5));
    }
    else if (databaseStatus == SQLITE_DONE)
    {
        printf("No such parcel, check your input\n");
        return;
    }
    
    // TODO �ռ�������Ϣ��ͬʱ��ȡ����ֶ�
    int parameterCount = 0;
    printf("Enter the field you want to update, separated by ',': ");
    sgets(userInput, 64);
    token = strtok(userInput, separation);
    while (token)
    {
        //TODO תСд
        for (char* ptr = userInput; *ptr; ptr++)
        {
            *ptr = tolower(*ptr);
        }
        //TODO �жϺϷ���
        if (!is_valid_fieldname(token))
        {
            printf("Invalid field name");
            return;
        }
        
        //TODO �ַ���ƴ���ֶ�ֵ�� =? ��
        if (parameterCount > 0)
        {
            strcat(setClause, ", ");
        }
        strcat(setClause, token);
        strcat(setClause, "=?");

        //TODO �ռ�����ֵ
        if (!strcasecmp(token, "intime") || !strcasecmp(token, "outtime"))
        {
            char timeStr[32] = "";
            printf("Enter the new %s: ", token);
            sgets(timeStr, 64);
            getTime(timeStr, newValue[parameterCount]);
        }
        else
        {
            printf("Enter the new value of %s: ", token);
            sgets(newValue[parameterCount], 64);
        }
        token = strtok(NULL, separation);
        parameterCount++;
    }
    // TODO ����set�Ӿ�
    strcat(setClause, ";");
    sqlite3_prepare_v2(parcelhub, setClause, -1, statement, NULL);
    
    for (int i = 0; i < parameterCount; i++)
    {
        if (strlen(newValue[i]) == 1)
        {
            int status = newValue[i][0] - 48;
            sqlite3_bind_int(statement, i + 1, status);
            continue;
        }
        else
        {
            sqlite3_bind_text(statement, i + 1, newValue[i], -1, SQLITE_STATIC);
        }
    }
    
    

    // TODO ׼������

    // TODO �󶨲���

    // TODO ִ�и���
    databaseStatus = sqlite3_step(statement);
    if (databaseStatus != SQLITE_OK)
    {
        printf("Database error: %s\n", sqlite3_errmsg(parcelhub));
        return;
    }
    else
    {
        printf("Update success\n");
    }

    return;

    // //* Open the file and load the linkist.
    // FILE *parcelData = openParcel("r");
    // node *list = loadLinklist(parcelData);
    // fclose(parcelData);
    // int matchCount = 0;

    // //* Go through the linklist to match the target item.
    // for (node *cur = list, *before = NULL; cur != NULL; before = cur, cur = cur->next)
    // {
    //     if (matchedLine(cur, ID))
    //     {
    //         printf("Matched!\n");
    //         cur->data = inputParcel();
    //         matchCount++;
    //     }
    // }
    // //* Check if there is matched items, if not, tell the user.
    // if (matchCount == 0)
    // {
    //     printf("Not matched! Check your input.\n");
    // }

    // //* Free the linklist.
    // parcelData = openParcel("w+");

    // for (node *cur = list, *next = NULL; cur != NULL;)
    // {
    //     next = cur->next;
    //     writeLine(&(cur->data), parcelData);
    //     free(cur);
    //     cur = next;
    // }

    // fclose(parcelData);
}

void selectLine(char *ID)
{
    char *select = "SELECT * FROM PARCELS WHERE id = ?;";
    sqlite3_prepare_v2(parcelhub, select, -1, &statement, NULL);
    sqlite3_bind_text(statement, 1, ID, -1, SQLITE_STATIC);
    databaseStatus = sqlite3_step(statement);

    if (databaseStatus == SQLITE_ROW)
    {
        printf("Found parcel: %s\n\n", ID);
        logger.print(INFO, "query for parcel: %s", ID);
    }
    else if (databaseStatus == SQLITE_DONE)
    {
        printf("No such parcel: %s\n\n", ID);
        logger.print(INFO, "query for parcel: %s which not exists", ID);
    }
    else
    {
        printf("Database error: %s", sqlite3_errmsg(parcelhub));
        logger.print(ERROR, "met an database error: %s", sqlite3_errmsg(parcelhub));
    }

    return;
    // //* Open the file and load the linkist.
    // FILE *parcelData = openParcel("r");
    // node *list = loadLinklist(parcelData);
    // fclose(parcelData);
    // int matchCount = 0;

    // //* Go through the linklist to match the target item.
    // for (node *ptr = list; ptr != NULL; ptr = ptr->next)
    // {
    //     if (matchedLine(ptr, ID))
    //     {
    //         printf("Matched!\n");
    //         outputParcel(&(ptr->data));
    //         matchCount++;
    //     }
    // }
    // //* Check if there is matched items, if not, tell the user.
    // if (matchCount == 0)
    // {
    //     printf("Not matched! Check your input.\n");
    // }

    // //* Free the linklist.
    // freeLinklist(list);
}

void freeLinklist(node *list)
{
    //* Free the linklist.
    for (node *cur = list, *next = NULL; cur != NULL;)
    {
        next = cur->next;
        free(cur);
        cur = next;
    }
}

void outbound(char *ID)
{
    time_t now;
    struct tm *currentTime;

    char query[32] = "SELECT * FROM PARCELS WHERE id = ?;";
    char update[32] = "UPDATE PARCELS SET outtime = ?;";
    char timeStr[32] = {0};

    sqlite3_prepare_v2(parcelhub, query, -1, &statement, NULL);
    sqlite3_bind_text(statement, 1, ID, -1, SQLITE_STATIC);
    databaseStatus = sqlite3_step(statement);

    if (databaseStatus == SQLITE_ROW)
    {
        now = time(NULL);
        currentTime = localtime(&now);
        snprintf(timeStr, 32, "%4d-%02d-%02d %02d:%02d:%02d", currentTime->tm_year + 1900, currentTime->tm_mon, currentTime->tm_mday, currentTime->tm_hour, currentTime->tm_min, currentTime->tm_sec);

        sqlite3_prepare_v2(parcelhub, update, -1, &statement, NULL);
        sqlite3_bind_text(statement, 1, timeStr, -1, SQLITE_STATIC);
        sqlite3_step(statement);

        if (sqlite3_changes(parcelhub) == 1)
        {
            printf("Outbounded parcel: %s\n\n", ID);
            logger.print(INFO, "outbounded parcel: %s", ID);
        }
        else
        {
            printf("Failed to outbounded for error: %s\n\n", sqlite3_errmsg(parcelhub));
            logger.print(ERROR, "failed to outbounded a parcel for error: %s", sqlite3_errmsg(parcelhub));
        }
    }
    else if (databaseStatus == SQLITE_DONE)
    {
        printf("No such parcel: %s\n", ID);
        printf("Check your input\n\n");
    }
    else
    {
        printf("Database error: %s\n\n", sqlite3_errmsg(parcelhub));
    }

    return;

    // //* Open the file and load the linkist.
    // FILE *parcelData = openParcel("r");
    // node *list = loadLinklist(parcelData);
    // fclose(parcelData);
    // int matchCount = 0;

    // //* Go through the linklist to match the target item.
    // for (node *cur = list, *before = NULL; cur != NULL; before = cur, cur = cur->next)
    // {
    //     if (matchedLine(cur, ID))
    //     {
    //         now = time(NULL);
    //         currentTime = localtime(&now);
    //         cur->data.state = OUTBOUND;
    //         cur->data.unload_time.year = currentTime->tm_year + 1900;
    //         cur->data.unload_time.month = currentTime->tm_mon;
    //         cur->data.unload_time.day = currentTime->tm_mday;
    //         cur->data.unload_time.hour = currentTime->tm_hour;
    //         cur->data.unload_time.minute = currentTime->tm_min;
    //         cur->data.unload_time.second = currentTime->tm_sec;
    //         matchCount++;
    //         printf("Outbounded!\n");
    //     }
    // }
    // //* Check if there is matched items, if not, tell the user.
    // if (matchCount == 0)
    // {
    //     printf("Not matched! Check your input.\n");
    // }

    // //* Free the linklist.
    // parcelData = openParcel("w+");

    // for (node *cur = list, *next = NULL; cur != NULL;)
    // {
    //     next = cur->next;
    //     writeLine(&(cur->data), parcelData);
    //     free(cur);
    //     cur = next;
    // }

    // fclose(parcelData);
}

int getTime(char *userInput, char *timeStr)
{
    char *time = strtok(userInput, "- .:");
    int tokenCount = 0;
    while (time)
    {
        char formatted[5] = "";
        if (strlen(time) == 1)
        {
            sprintf(formatted, "0%s", time);
        }
        else
        {
            strcpy(formatted, time);
        }
        strcat(timeStr, formatted);
        switch (tokenCount)
        {
        case 0:
        case 1:
            strcat(timeStr, "-");
            break;
        case 2:
            strcat(timeStr, " ");
            break;
        case 3:
        case 4:
            strcat(timeStr, ":");
            break;
        default:
            break;
        }
        time = strtok(NULL, "- .");
        tokenCount++;
    }
    return 0;
}