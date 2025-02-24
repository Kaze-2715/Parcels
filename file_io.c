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
extern sqlite3 *parcelHub;
extern sqlite3_stmt *statement;
extern char *errorMessage;
extern int databaseStatus;


static int getTime(char *userInput, char *timeStr);
static int is_valid_fieldname(const char *token);
static char *sgets(char *buffer, int size);

void deleteLine(char *ID)
{
    char delete[64] = "DELETE FROM PARCELS WHERE id = ?;";

    sqlite3_prepare_v2(parcelHub, delete, -1, &statement, NULL);
    sqlite3_bind_text(statement, 1, ID, -1, SQLITE_STATIC);
    databaseStatus = sqlite3_step(statement);

    if (databaseStatus == SQLITE_DONE)
    {
        printf("Deleted parcel: %s", ID);
        logger.print(INFO, "deleted parcel: %s", ID);
    }
    else
    {
        printf("Failed to delete for database error: %s", sqlite3_errmsg(parcelHub));
        logger.print(ERROR, "failed to delete for database error: %s", sqlite3_errmsg(parcelHub));
    }

    return;
}

void updateLine(char *ID)
{
    //* ---------±äÁ¿Çø----------
    char *query = "SELECT * FROM PARCELS WHERE id = ?;";
    char setClause[256] = "UPDATE PARCELS SET ";
    char userInput[64] = "";
    char newValue[5][64] = {0};
    char *separation = ",";
    char *token = NULL;

    sqlite3_prepare_v2(parcelHub, query, -1, &statement, NULL);
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

    int parameterCount = 0;
    printf("Enter the field you want to update, separated by ',': ");
    sgets(userInput, 64);
    token = strtok(userInput, separation);
    while (token)
    {

        for (char* ptr = token; *ptr; ptr++)
        {
            if (isspace(*ptr))
            {
                token = ptr + 1;
            }
            *ptr = tolower(*ptr);
        }

        if (!is_valid_fieldname(token))
        {
            printf("Invalid field name");
            return;
        }
        

        if (parameterCount > 0)
        {
            strcat(setClause, ", ");
        }
        strcat(setClause, token);
        strcat(setClause, "=?");


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

    strcat(setClause, " WHERE id = ?;");
    sqlite3_prepare_v2(parcelHub, setClause, -1, &statement, NULL);
    
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

    sqlite3_bind_text(statement, parameterCount + 1, ID, -1, SQLITE_STATIC);






    databaseStatus = sqlite3_step(statement);
    if (databaseStatus != SQLITE_DONE)
    {
        printf("Database error: %s\n", sqlite3_errmsg(parcelHub));
        return;
    }
    else
    {
        printf("Update success\n");
    }

    return;
}

void selectLine(char *ID)
{
    char *select = "SELECT * FROM PARCELS WHERE id = ?;";
    sqlite3_prepare_v2(parcelHub, select, -1, &statement, NULL);
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
        printf("Database error: %s", sqlite3_errmsg(parcelHub));
        logger.print(ERROR, "met an database error: %s", sqlite3_errmsg(parcelHub));
    }

    return;
}

void outbound(char *ID)
{
    time_t now;
    struct tm *currentTime;

    char query[64] = "SELECT * FROM PARCELS WHERE id = ?;";
    char update[64] = "UPDATE PARCELS SET outtime = ? WHERE id = ?;";
    char timeStr[32] = {0};

    sqlite3_prepare_v2(parcelHub, query, -1, &statement, NULL);
    sqlite3_bind_text(statement, 1, ID, -1, SQLITE_STATIC);
    databaseStatus = sqlite3_step(statement);

    if (databaseStatus == SQLITE_ROW)
    {
        now = time(NULL);
        currentTime = localtime(&now);
        snprintf(timeStr, 32, "%4d-%02d-%02d %02d:%02d:%02d", currentTime->tm_year + 1900, currentTime->tm_mon + 1, currentTime->tm_mday, currentTime->tm_hour, currentTime->tm_min, currentTime->tm_sec);

        sqlite3_prepare_v2(parcelHub, update, -1, &statement, NULL);
        sqlite3_bind_text(statement, 1, timeStr, -1, SQLITE_STATIC);
        sqlite3_bind_text(statement, 2, ID, -1, SQLITE_STATIC);
        sqlite3_step(statement);

        if (sqlite3_changes(parcelHub) == 1)
        {
            printf("Outbounded parcel: %s\n\n", ID);
            logger.print(INFO, "outbounded parcel: %s", ID);
        }
        else
        {
            printf("Failed to outbounded for error: %s\n\n", sqlite3_errmsg(parcelHub));
            logger.print(ERROR, "failed to outbounded a parcel for error: %s", sqlite3_errmsg(parcelHub));
        }
    }
    else if (databaseStatus == SQLITE_DONE)
    {
        printf("No such parcel: %s\n", ID);
        printf("Check your input\n\n");
    }
    else
    {
        printf("Database error: %s\n\n", sqlite3_errmsg(parcelHub));
    }

    return;
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

int is_valid_fieldname(const char *token)
{
    int flag = 0;
    char *fields[5] = {"origin", "destination", "status", "intime", "outtime"};

    for (int i = 0; i < 5; i++)
    {
        if (!strcasecmp(token, fields[i]))
        {
            flag = 1;
        }
        
    }

    return flag;
}

static char *sgets(char *buffer, int size)
{
    if (buffer == NULL)
    {
        puts("Buffer is NULL!");
        return NULL;
    }
    if (size < 1)
    {
        puts("Size should greater than 1!");
        buffer[0] = '\0';
        return NULL;
    }

    char *newLine;

    if (fgets(buffer, size, stdin) == NULL)
    {
        perror("Failed to get.");
        return NULL;
    }

    newLine = strrchr(buffer, '\n');
    if (newLine == NULL)
    {
        char ch;
        while ((ch = getchar()) != '\n' && ch != EOF)
        {
            continue;
        }
        buffer[0] = '\0';
        puts("Input is too long!\n");
        return NULL;
    }
    else
    {
        *newLine = '\0';
    }

    return buffer;
}