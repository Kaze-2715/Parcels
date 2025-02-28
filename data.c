#include "defines.h"
#include "log.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>

extern sqlite3 *parcelHub;
extern sqlite3_stmt *statement;
extern char *errorMessage;
extern int databaseStatus;
extern log logger;

//static int getOpCode(char *key);
extern char *sgets(char *buffer, int size);
extern int is_valid_fieldname(const char *token);
extern int getTime(char *userInput, char *timeStr);

void filter()
{
    char whereClause[256] = "SELECT * FROM PARCELS ";
    int parameterCount = 0;
    char userInput[128] = "";
    puts("origin, destination, status, intime, outtime");
    printf("Enter the fields you want to apply filter(separated by ','): ");
    sgets(userInput, 128);

    if (userInput[0] != 0)
    {
        strcat(whereClause, "WHERE ");
    }

    char *token = NULL;
    char values[6][32] = {0};
    token = strtok(userInput, ",");
    while (token)
    {
        for (char *ptr = token; *ptr; ptr++)
        {
            if (isspace(*ptr) && !isspace(*(ptr + 1)))
            {
                token = ptr + 1;
            }
            else if (!isspace(*ptr) && isspace(*(ptr + 1)))
            {
                *(ptr + 1) = 0;
            }
            else
            {
                *ptr = tolower(*ptr);
            }
        }
        if (!is_valid_fieldname(token))
        {
            printf("Invalid field name\n");
            goto next;
        }
        if (parameterCount > 0)
        {
            strcat(whereClause, " AND ");
        }
        strcat(whereClause, token);
        if (!strcasecmp(token, "intime") || !strcasecmp(token, "outtime"))
        {
            char filter[32] = "";
            char timeStr[32] = "";
            printf("For %s, select a filter you want to apply from: ", token);
            printf("before, after, between\n");
            fgets(filter, 16, stdin);
            *strpbrk(filter, "\n") = 0;

            if (!strcasecmp(filter, "before"))
            {
                strcat(whereClause, "<?)");
                printf("Enter the time: ");
                sgets(filter, 32);
                getTime(filter, values[parameterCount]);
            }
            else if (!strcasecmp(filter, "after"))
            {
                strcat(whereClause, ">?)");
                printf("Enter the time: ");
                sgets(filter, 32);
                getTime(filter, values[parameterCount]);
            }
            else if (!strcasecmp(filter, "between"))
            {
                strcat(whereClause, " BETWEEN ? AND ?");
                printf("Enter the former: ");
                sgets(filter, 32);
                getTime(filter, values[parameterCount]);
                parameterCount++;

                printf("Enter the latter: ");
                sgets(filter, 32);
                getTime(filter, values[parameterCount]);
            }
        }
        else if (!strcasecmp(token, "status"))
        {
            strcat(whereClause, "=?");
            char parcelStatus[16] = "";
            getStatus:
            printf("Enter the status(Inbound or Outbound): ");
            sgets(parcelStatus, 16);
            if (!strcasecmp(parcelStatus, "inbound"))
            {
                values[parameterCount][0] = IN + 48;
            }
            else if (!strcasecmp(parcelStatus, "outbound"))
            {
                values[parameterCount][0] = OUT + 48;
            }
            else
            {
                puts("Invalid status!");
                goto getStatus;
            }            
        }        
        else
        {
            strcat(whereClause, "=?");
            printf("Value for %s: ", token);
            sgets(values[parameterCount], 32);
        }
        parameterCount++;
    next:
        token = strtok(NULL, ",");
    }
    strcat(whereClause, ";");
    sqlite3_prepare_v2(parcelHub, whereClause, -1, &statement, NULL);

    for (int i = 0; i < parameterCount; i++)
    {
        if (strlen(values[i]) == 1)
        {
            int status = values[i][0] - 48;
            sqlite3_bind_int(statement, i + 1, status);
        }
        else
        {
            sqlite3_bind_text(statement, i + 1, values[i], -1, SQLITE_STATIC);
        }
    }

    databaseStatus = sqlite3_step(statement);
    
    if (databaseStatus == SQLITE_DONE)
    {
        printf("No results\n");
        goto end;
    }
    else if (databaseStatus == SQLITE_ROW)
    {
        do
        {
            for (int i = 0; i < 6; i++)
            {
                if (i == 3)
                {
                    printf("%s ", sqlite3_column_int(statement, i) == IN ? "Inbound" : "Outbound");
                }
                else
                {
                    printf("%s ", sqlite3_column_text(statement, i));
                }
            }
            printf("\n");
        } while (sqlite3_step(statement) == SQLITE_ROW);

        printf("filtered\n");
    }
    else
    {
        printf("Database error: %s\n", sqlite3_errmsg(parcelHub));
    }
end:
    return;
}

void sort()
{
    char orderClause[256] = "SELECT * FROM PARCELS ";
    int fieldCount = 0;
    char userInput[128] = "";
    puts("\norigin, destination, status, intime, outtime");
    printf("Enter the fields you want to sort(separated by ','): ");
    sgets(userInput, 128);

    if (userInput[0] != 0)
    {
        strcat(orderClause, "ORDER BY ");
    }

    char *token = NULL;
    char sortOrder[16] = "";
    token = strtok(userInput, ",");
    while (token)
    {
        for (char *ptr = token; *ptr; ptr++)
        {
            if (isspace(*ptr) && !isspace(*(ptr + 1)))
            {
                token = ptr + 1;
            }
            else if (!isspace(*ptr) && isspace(*(ptr + 1)))
            {
                *(ptr + 1) = 0;
            }
            else
            {
                *ptr = tolower(*ptr);
            }
        }
        if (!is_valid_fieldname(token))
        {
            printf("Invalid field name!\n");
            goto next;
        }
        if (fieldCount > 0)
        {
            strcat(orderClause, ", ");//*¿Õ¸ñÒª½ô½Ó
        }

        strcat(orderClause, token);
        getOrder:
        printf("Enter the sort order for '%s'(ASC or DESC): ", token);
        sgets(sortOrder, 16);
        if (!strcasecmp(sortOrder, "ASC"))
        {
            strcat(orderClause, " ASC");
        }
        else if (!strcasecmp(sortOrder, "DESC"))
        {
            strcat(orderClause, " DESC");
        }
        else
        {
            puts("Invalid sort order!");
            goto getOrder;
        }
        
        fieldCount++;
        next:
        token = strtok(NULL, ",");
    }
    strcat(orderClause, ";");
    sqlite3_prepare_v2(parcelHub, orderClause, -1, &statement, NULL);

    databaseStatus = sqlite3_step(statement);

    if (databaseStatus == SQLITE_DONE)
    {
        printf("No results\n");
    }
    else if (databaseStatus == SQLITE_ROW)
    {
        do
        {
            printf("| ");
            for (int i = 0; i < 6; i++)
            {
                if (i == 3)
                {
                    printf(" %s |", sqlite3_column_int(statement, i) == IN ? "Inbound" : "Outbound");
                }
                else
                {
                    printf(" %s |", sqlite3_column_text(statement, i));
                }
            }
            printf("\n");
        } while (sqlite3_step(statement) == SQLITE_ROW);

        printf("sorted\n");
    }
    else
    {
        printf("Database error: %s\n", sqlite3_errmsg(parcelHub));
    }

    return;
}