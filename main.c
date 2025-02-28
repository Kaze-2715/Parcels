#include "defines.h"
#include "declarations.h"
#include "user.h"
#include "log.h"
#include <stdio.h>
#include <sqlite3.h>

int LOGGED = 0;
int ACCESSIBLE = 0;
int Start = 0;
user currentUser;
log logger;
sqlite3 *parcelHub = NULL;
sqlite3_stmt *statement = NULL;
char *errorMessage = NULL;
int databaseStatus = SQLITE_OK;

static int databaseError(char *systemOpration, char *databaseOpration);

int main(void)
{
    void (*exec[])() = {help, login, logout, halt, dataFilter, dataSort, select, inBound, outBound, delete, update, userCreate, userDelete, userUpdate};

    start();

    while(1)
    {
        int code = -1;          //* The command code
        int needPermission = 1; //* To identify whether it needs permission.
        int need2start = 0;     //* To identify whether it needs start the system.

        code = getCommand();  

        if (code == -1)   //* To check whether the function get a valid code.
        {
            printf("Command error: No such command.\n");
            continue;
        }
        if (code > SELECT)   //* To check whether the command needs a permission.
        {
            needPermission = 0;
        }
        if ((!ACCESSIBLE) && needPermission) //* To check whether the user has the permission to access.
        {
            printf("You are not accessible! Please acquire permissions.\n");
            continue;
        }
// TODO 使用函数指针，做一个表驱动
        exec[code]();
    }
}

void start()
{
    const char *sql = NULL;
    initLog();
    databaseStatus = sqlite3_open("parcelhub.db", &parcelHub);
    if (databaseError("Starting system", "opening database"))
    {
        return;
    }

    sql = "CREATE TABLE IF NOT EXISTS PARCELS ("
          "origin TEXT NOT NULL,"
          "destination   TEXT NOT NULL,"
          "id     TEXT PRIMARY KEY NOT NULL,"
          "status INT  NOT NULL,"
          "intime TEXT NOT NULL,"
          "outtime);";
    databaseStatus = sqlite3_exec(parcelHub, sql, NULL, NULL, &errorMessage);
    if (databaseError("Starting system", "creating table 'parcels'"))
    {
        return;
    }

    sql = "CREATE TABLE IF NOT EXISTS USERS ("
          "username TEXT PRIMARY KEY NOT NULL,"
          "password TEXT NOT NULL,"
          "accessibility INT NOT NULL);"
          "INSERT OR IGNORE INTO USERS"
          "(username, password, accessibility)"
          "VALUES ('root', '123456', 1);";
    databaseStatus = sqlite3_exec(parcelHub, sql, NULL, NULL, &errorMessage);
    if (databaseError("Starting system", "creating table 'users'"))
    {
        return;
    }

    do
    {
        login();
    } while (!LOGGED);
    logger.print(INFO, "%s", "started the system");
    Start = 1;

    printf("If needed, please enter '-help' for further instructions.\n\n");

    return;
}

void login()
{
    currentUser = getUser();
    currentUser.login(&currentUser);
    return;
}

void logout()
{
    currentUser.logout(&currentUser);
    return;
}

void halt()
{
    logger.print(INFO, "stopped the system");
    logger.destroy();
    //sqlite3_finalize(statement);
    sqlite3_close(parcelHub);
}

void dataFilter()
{
    do
    {
        filter();
    } while (chooseToContinue());
}

void dataSort()
{
    do
    {
        sort();
    } while (chooseToContinue());
}

void dataVisual()
{
    printf("Visualized\n\n");
}

//* @brief Continuously doing get info and wrtie a line until the user decided to stop it.
void inBound()
{
    parcel one;
    do
    {
        inbound();
    } while (chooseToContinue());

    return;
}

void outBound()
{
    do
    {
        char ID[20] = {0};
        printf("The parcel ID to be outbounded: ");
        scanf("%s", ID);
        getchar();

        outbound(ID);
    } while (chooseToContinue());
    
}

//* @brief Continuously doing get ID and delete the line until the user dicided to stop it.
void delete()
{ 
    do
    {
        char ID[20] = {0};
        printf("The parcel ID to be deleted: ");
        scanf("%s", ID);
        getchar();

        deleteLine(ID);
        logger.print(INFO, "deleted a parcel: %s", ID);
        printf("Delete complete\n\n");
    } while (chooseToContinue());
}

//* @brief Continously doing getting ID and update the line until user's stop.
void update()
{
    do
    {
        char ID[20] = {0};
        printf("The parcel ID to be altered: ");
        scanf("%19s", ID);
        getchar();
        updateLine(ID);
        logger.print(INFO, "updated a parcel: %s", ID);
        printf("Update complete\n\n");
    } while (chooseToContinue());
}
//* @brief Same as above, but select a line.
void select()
{
    do
    {
        char ID[20] = {0};
        printf("The parcel ID to be queried: ");
        scanf("%19s", ID);
        getchar();
        selectLine(ID);
        logger.print(INFO, "selected a parcel: %s", ID);
    } while (chooseToContinue());
}

void userCreate()
{

    if (!LOGGED)
    {
        puts("Please login first!\n\n");
        return;
    }
    if (!ACCESSIBLE)
    {
        puts("You are not authorized.\n\n");
        return;
    }

    user newUser = getUser();
    createUser(&newUser);
    return;
}

void userDelete()
{
    if (!LOGGED)
    {
        puts("Please login first!\n\n");
        return;
    }
    if (!ACCESSIBLE)
    {
        puts("You are not authorized.\n\n");
        return;
    }

    user toBeDeleted = getUser();

    deleteUser(&toBeDeleted);
    return;
}

void userUpdate()
{

    if (!LOGGED)
    {
        puts("Please login first!\n\n");
        return;
    }
    if (!ACCESSIBLE)
    {
        puts("You are not authorized.\n\n");
        return;
    }
    
    user toBeUpdated = getUser();
    updateUser(&toBeUpdated);
    return; 
}

static int databaseError(char *systemOpration, char *databaseOpration)
{
    if (databaseStatus != SQLITE_OK)
    {
        printf("%s: %s\n",databaseOpration ,sqlite3_errmsg(parcelHub));
        logger.print(ERROR, "%s but failed when %s: %s",systemOpration, databaseOpration, errorMessage);
        return 1;
    }
    return 0;
}

void help()
{
    printf("-login       : Login your account.\n");
    printf("-logout      : Logout your account.\n");
    printf("-start       : Start the system and load the data.\n");
    printf("-stop        : Save all the data and exit the system.\n");
    printf("-reload      : Reload the data file to the program.\n");
    printf("-save        : Save the current data to the database file.\n");
    printf("-halt        : Exit the program without any saving operations.\n");
    printf("-inBound     : Add a parcel to the warehouse.\n");
    printf("-outBound    : Outbound a parcel from the warehouse.\n");
    printf("-delete      : Delete a parcel from warehouse(completely delete!).\n");
    printf("-update      : Update the state of a parcel.\n");
    printf("-select      : Select a specific parcel to read its information.\n");
    printf("-dataFilter  : Select all the parcels which have the same identifications.\n");
    printf("-dataSort    : Sort the data as you wish.\n");
    printf("-dataVisual  : Visualize the data as you wish.\n");
    printf("-userCreate  : Create a new user of the system.\n");
    printf("-userDelete  : Delete a user.\n");
    printf("-userUpdate  : Update the permission of a user.\n");
    return;
}
