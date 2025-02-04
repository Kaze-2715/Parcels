#include "defines.h"
#include "user_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

node *loadLinklist(FILE *fp);
void freeLinklist(node *list);
int matchedLine(node *buffer, char *ID);

//* @brief To open the "parcels.txt"
//! @attention Need the whole path to the file, which is not good for deploying in other computers.
FILE *openParcel(char *mode)
{
    FILE *fp = fopen("E:/BaiduSyncdisk/03_CODE/VSCode_Workspace/parcel_service/parcels.txt", mode);
    if (fp == NULL)
    {
        perror("Open parcel data");
    }
    return fp;
}

//* @brief To open the "log.txt"
//! @attention Same as above
FILE *openLog(char *mode)
{
    FILE *fp = fopen("E:/BaiduSyncdisk/03_CODE/VSCode_Workspace/parcel_service/log.txt", mode);
    if (fp == NULL)
    {
        perror("Open log data");
    }
    return fp;
}

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

    return readState; //* Return the read state, a positive number is "succcess", 0 or negative is "failed".
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
    FILE *parcelData = openParcel("r");
    node *list = loadLinklist(parcelData);
    node *matched = NULL;
    fclose(parcelData);

    for (node *cur = list, *before = NULL; cur != NULL; before = cur, cur = cur->next)
    {
        if (matchedLine(cur, ID))
        {
            if (before == NULL)
            {
                list = cur->next;
                matched = cur;
            }
            else
            {
                before->next = cur->next;
                matched = cur;
            }
        }
    }

    if (matched != NULL)
    {
        free(matched);
    }

    // TODO 接下来将删除好的链表写进新文件当中,然后将链表free掉
    parcelData = openParcel("w+");

    for (node *cur = list, *next = NULL; cur != NULL;)
    {
        next = cur->next;
        writeLine(&(cur->data), parcelData);
        free(cur);
        cur = next;
    }

    // TODO 关闭文件
    fclose(parcelData);
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
    //* Open the file and load the linkist.
    FILE *parcelData = openParcel("r");
    node *list = loadLinklist(parcelData);
    fclose(parcelData);
    int matchCount = 0;

    //* Go through the linklist to match the target item.
    for (node *cur = list, *before = NULL; cur != NULL; before = cur, cur = cur->next)
    {
        if (matchedLine(cur, ID))
        {
            printf("Matched!\n");
            cur->data = inputParcel();
            matchCount++;
        }
    }
    //* Check if there is matched items, if not, tell the user.
    if (matchCount == 0)
    {
        printf("Not matched! Check your input.\n");
    }

    //* Free the linklist.
    parcelData = openParcel("w+");

    for (node *cur = list, *next = NULL; cur != NULL;)
    {
        next = cur->next;
        writeLine(&(cur->data), parcelData);
        free(cur);
        cur = next;
    }

    fclose(parcelData);
}

void selectLine(char *ID)
{
    //* Open the file and load the linkist.
    FILE *parcelData = openParcel("r");
    node *list = loadLinklist(parcelData);
    fclose(parcelData);
    int matchCount = 0;

    //* Go through the linklist to match the target item.
    for (node *ptr = list; ptr != NULL; ptr = ptr->next)
    {
        if (matchedLine(ptr, ID))
        {
            printf("Matched!\n");
            outputParcel(&(ptr->data));
            matchCount++;
        }
    }
    //* Check if there is matched items, if not, tell the user.
    if (matchCount == 0)
    {
        printf("Not matched! Check your input.\n");
    }

    //* Free the linklist.
    freeLinklist(list);
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