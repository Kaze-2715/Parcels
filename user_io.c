#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sqlite3.h>
#include "defines.h"
#include "log.h"

extern log logger;
extern sqlite3 *parcelhub;
extern sqlite3_stmt *statement;
extern char *errorMessage;
extern int databaseStatus;

int cmd2code(char command[]);
int getCommand();

int inbound()
{
    char inbound[128] = "INSERT OR IGNORE INTO PARCELS "
                        "(origin, destination, id, status, intime) VALUES "
                        "(?, ?, ?, ?, ?);";
    char timeStr[32] = {0};
    sqlite3_prepare_v2(parcelhub, inbound, -1, &statement, NULL);
    parcel one;
    time_t now;
    struct tm *currentTime;

    printf("Input the parcel infomation below\nFrom: ");
    scanf("%9s", one.from);
    getchar();
    sqlite3_bind_text(statement, 1, one.from, -1, SQLITE_STATIC);
    printf("To: ");
    scanf("%9s", one.to);
    getchar();
    sqlite3_bind_text(statement, 2, one.to, -1, SQLITE_STATIC);
    printf("ID: ");
    scanf("%19s", one.ID);
    getchar();
    sqlite3_bind_text(statement, 3, one.ID, -1, SQLITE_STATIC);

    sqlite3_bind_int(statement, 4, INBOUND);

    now = time(NULL);
    currentTime = localtime(&now);
    // one.load_time.year = currentTime->tm_year + 1900;
    // one.load_time.month = currentTime->tm_mon;
    // one.load_time.day = currentTime->tm_mday;
    // one.load_time.hour = currentTime->tm_hour;
    // one.load_time.minute = currentTime->tm_min;
    // one.load_time.second = currentTime->tm_sec;

    // one.unload_time.year = 0;
    // one.unload_time.month = 0;
    // one.unload_time.day = 0;
    // one.unload_time.hour = 0;
    // one.unload_time.minute = 0;
    // one.unload_time.second = 0;
    snprintf(timeStr, 32, "%4d-%02d-%02d %02d:%02d:%02d", currentTime->tm_year + 1900, currentTime->tm_mon, currentTime->tm_mday, currentTime->tm_hour, currentTime->tm_min, currentTime->tm_sec);
    sqlite3_bind_text(statement, 5, timeStr, -1, SQLITE_STATIC);

    sqlite3_step(statement);

    if (sqlite3_changes(parcelhub) == 1)
    {
        printf("Inbound complete\n\n");
        logger.print(INFO, "inbounded a parcel: %s", one.ID);
        return 1;
    }
    else
    {
        printf("Inbound error\n\n");
        logger.print(ERROR, "failed to inbound a parcel: %s exists", one.ID);
        return 0;
    }

    return ;
}

parcel inputParcel()
{
    parcel one;
    char status;
    int sign;

    printf("Input the parcel infomation below\nFrom: ");
    scanf("%9s", one.from);
    getchar();
    printf("To: ");
    scanf("%9s", one.to);
    getchar();
    printf("ID: ");
    scanf("%19s", one.ID);
    getchar();
    printf("Load time: ");
    scanf("%d %d %d %d %d %d", &one.load_time.year, &one.load_time.month, &one.load_time.day, &one.load_time.hour, &one.load_time.minute, &one.load_time.second);
    getchar();
    printf("Is it has been unloaded? (y/n): ");
    scanf("%c", &status);
    getchar();

    if ((status == 'y') || (status == 'Y'))
    {
        printf("Unload time: ");
        scanf("%d %d %d %d %d %d", &one.unload_time.year, &one.unload_time.month, &one.unload_time.day, &one.unload_time.hour, &one.unload_time.minute, &one.unload_time.second);
        getchar();

        one.state = 1;
    }
    else if ((status == 'N') || (status == 'n'))
    {
        one.unload_time.year = 0;
        one.unload_time.month = 0;
        one.unload_time.day = 0;
        one.unload_time.hour = 0;
        one.unload_time.minute = 0;
        one.unload_time.second = 0;

        one.state = 0;
    }
    else
    {
        printf("Error input!");
    }
    

    return one;
}

int chooseToContinue()
{
    char choose;
    printf("End or not? (y/n) ");
    scanf("%c", &choose);
    char ch = getchar();
    if ((choose == 'y') || (choose == 'Y'))
    {
        return 0;
    }
    else
    {
        if ((choose == 'n') || (choose == 'N'))
        {
            return 1;
        }
        else
        {
            printf("Check your input!\n");
            return 0;
        }
    }
}

void outputParcel(parcel *one)
{
    printf("%s,%s,%s,%d,%d.%d.%d.%d.%d.%d,", one->from, one->to, one->ID, one->state, one->load_time.year, one->load_time.month, one->load_time.day, one->load_time.hour, one->load_time.minute, one->load_time.second);

    if (one->state) //*To check if this one has been fetched. If fetched, read in the time, or set manually.
    {
        printf("%d.%d.%d.%d.%d.%d\n", one->unload_time.year, one->unload_time.month, one->unload_time.day, one->unload_time.hour, one->unload_time.minute, one->unload_time.second);
    }
    else
    {
        printf("Not unloaded.\n");
    }
}

// -------------------------------------------------
// @author Kaze
// @date 2024.12.14
// @brief Switch the command text to command code
// -------------------------------------------------
int cmd2code(char command[])
{
    cmd cmd_code = -1;

    if (!strcasecmp(command, "-help\n"))
    {
        cmd_code = HELP;
        return cmd_code;
    }
    if (!strcasecmp(command, "-inbound\n"))
    {
        cmd_code = IN;
        return cmd_code;
    }
    if (!strcasecmp(command, "-outbound\n"))
    {
        cmd_code = OUT;
        return cmd_code;
    }
    if (!strcasecmp(command, "-delete\n"))
    {
        cmd_code = DELETE;
        return cmd_code;
    }
    if (!strcasecmp(command, "-update\n"))
    {
        cmd_code = UPDATE;
        return cmd_code;
    }
    if (!strcasecmp(command, "-select\n"))
    {
        cmd_code = SELECT;
        return cmd_code;
    }
    if (!strcasecmp(command, "-dataFilter\n"))
    {
        cmd_code = DATA_FILTER;
        return cmd_code;
    }
    if (!strcasecmp(command, "-dataSort\n"))
    {
        cmd_code = DATA_SORT;
        return cmd_code;
    }
    if (!strcasecmp(command, "-dataVisual\n"))
    {
        cmd_code = DATA_VISUALIZATION;
        return cmd_code;
    }
    if (!strcasecmp(command, "-login\n"))
    {
        cmd_code = LOGIN;
        return cmd_code;
    }
    if (!strcasecmp(command, "-logout\n"))
    {
        cmd_code = LOGOUT;
        return cmd_code;
    }
    if (!strcasecmp(command, "-start\n"))
    {
        cmd_code = START;
        return cmd_code;
    }
    if (!strcasecmp(command, "-stop\n"))
    {
        cmd_code = STOP;
        return cmd_code;
    }
    if (!strcasecmp(command, "-reload\n"))
    {
        cmd_code = RELOAD;
        return cmd_code;
    }
    if (!strcasecmp(command, "-save\n"))
    {
        cmd_code = SAVE;
        return cmd_code;
    }
    if (!strcasecmp(command, "-halt\n"))
    {
        cmd_code = HALT;
        return cmd_code;
    }
    if (!strcasecmp(command, "-userCreate\n"))
    {
        cmd_code = USER_CREATE;
        return cmd_code;
    }
    if (!strcasecmp(command, "-userDelete\n"))
    {
        cmd_code = USER_DELETE;
        return cmd_code;
    }
    if (!strcasecmp(command, "-userUpdate\n"))
    {
        cmd_code = USER_UPDATE;
        return cmd_code;
    }
    if (!strcasecmp(command, "-lockAccount\n"))
    {
        cmd_code = LOCK_ACCOUNT;
        return cmd_code;
    }
    if (!strcasecmp(command, "-unlockAccount\n"))
    {
        cmd_code = UNLOCK_ACCOUNT;
        return cmd_code;
    }

    return cmd_code;
}

// -----------------------------------------------
// @author Kaze
// @date 2024.12.14
// @brief To get the user input command text and switch it to the code.
// -----------------------------------------------
int getCommand()
{
    char command[15] = {0};
    fgets(command, 14, stdin);
    int code = cmd2code(command);
    return code;
}
