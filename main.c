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
// ----------------------------------------
// @author Kaze
// @date 2024.12.14
// @brief The main part of the program.
// ----------------------------------------
int main(void)
{
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
        if (code <= 10)   //* To check whether the command needs a permission.
        {
            needPermission = 0;
        }
        if ((!LOGGED) && needPermission) //* To check whether the user logged in his account.
        {
            printf("Please login first!\n");
            continue;
        }
        if ((!ACCESSIBLE) && needPermission) //* To check whether the user has the permission to access.
        {
            printf("You are not accessible! Please acquire permissions.\n");
            continue;
        }

        switch (code)
        {
        case HELP:
            help();
            break;

        case START:
            start();
            break;

        case LOGIN:
            login();
            break;

        case LOGOUT:
            logout();
            break;

        case HALT:
            halt();
            return 0;
            break;

        case DATA_FILTER:
            dataFilter();
            break;

        case DATA_SORT:
            dataSort();
            break;

        case IN:
            inBound();
            break;

        case OUT:
            outBound();
            break;

        case DELETE:
            delete();
            break;

        case UPDATE:
            update();
            break;

        case SELECT:
            select();
            break;

        case USER_CREATE:
            userCreate();
            break;

        case USER_DELETE:
            userDelete();
            break;

        case USER_UPDATE:
            userUpdate();
            break;
        default:
            break;
        }
    }
}

// -------------------------------------------------
// @author Kaze
// @date 2024.12.14
// @brief Print help info on the monitor.
// -------------------------------------------------

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
    //printf("Started\n\n");
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
    sqlite3_finalize(statement);
    sqlite3_close(parcelHub);
    //printf("Halted\n\n");
}

void dataFilter()
{
    do
    {
        // TODO 获取筛选字段，并获取用户输入
        char key[20] = {0};
        printf("Input your key: ");
        scanf("%s", key);
        getchar();

        // TODO 设计一个函数，来匹配对应的字段的值，并输出结果，它的参数应该包含两部分：要匹配的字段，以及要匹配的值，以及可能的升序和降序排列
        filter(key);

    } while (chooseToContinue());
    
    //printf("Filtered\n\n");
}

void dataSort()
{
    do
    {
        // TODO 获取排序关键字段，确定升序还是降序
        char key[20] = {0};
        printf("Input your key: ");
        scanf("%s", key);
        getchar();
        // TODO 排序函数，参数应该包含两部分：字段，升序/降序
        sort(key);

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
    //FILE *parcelData = openParcel("a+");
    do
    {
        inbound();
        //writeLine(&one, parcelData);
    } while (chooseToContinue());

    //fclose(parcelData);
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
    // TODO 判断权限
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
    
    // TODO 获取用户输入，存储为一个变量
    user newUser = getUser();

    // TODO 将这个变量写入user.txt
    createUser(&newUser);
    return;
}

void userDelete()
{
    // TODO 判断权限
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

    // TODO 获取用户输入，存储为一个变量
    user toBeDeleted = getUser();

    // TODO 这个使用计算行数来完成，先计算总行数，在逐行读入解析，将匹配的行标记，然后将字符数组重新写入
    deleteUser(&toBeDeleted);
    return;
}

void userUpdate()
{
    // TODO 判断权限
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

int help()
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

    return 0;
}
