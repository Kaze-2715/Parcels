#include "user.h"
#include "log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>

extern log logger;
extern sqlite3 *parcelHub;
extern sqlite3_stmt *statement;
extern int databaseStatus;
extern char *errorMessage;

int welcomeUser(void *data, int columnCount, char *columnValue[], char *columnName[]);
static int databaseError(char *systemOpration, char *databaseOpration);
//! 详见matchedName()的优化，getuser()也适用这个优化方法
user getUser()
{
    user inputUser;
    inputUser.rooted = 0;
    inputUser.login = userLogin;
    inputUser.logout = userLogout;
    inputUser.create = createUser;
    inputUser.remove = deleteUser;
    inputUser.update = updateUser;

    printf("Username: ");
    sgets(inputUser.username, 20);
    printf("Password: ");
    sgets(inputUser.password, 20);

    return inputUser;
}

void userLogin(user *user)
{
    sqlite3_stmt *statement = NULL;
    char sql[128] = "SELECT * FROM USERS ";
    char condition[64] = {0};
    snprintf(condition, 64, "WHERE username = '?' AND password = '?';", user->username, user->password);
    strcat(sql, condition);

    // databaseStatus = sqlite3_exec(parcelHub, sql, welcomeUser, "Logged in. Welcome!", &errorMessage);
    databaseStatus = sqlite3_prepare_v2(parcelHub, sql, -1, &statement, NULL);

    sqlite3_bind_text(statement, 1, user->username, -1, SQLITE_STATIC);
    sqlite3_bind_text(statement, 2, user->password, -1, SQLITE_STATIC);

    if (sqlite3_step(statement) == SQLITE_ROW)
    {
        LOGGED = 1;
        user->rooted = atoi(sqlite3_column_text(statement, 2));
        ACCESSIBLE = user->rooted;
        logger.print(INFO, "%s logged in", user->username);
    }
    else
    {
        puts("Error: No such user. Check your input or sign up for an account.");
        logger.print(ERROR, "someone tried to login with '%s', but failed", user->username);
    }

    return;

    // if (!LOGGED)
    // {
    //     puts("Error: No such user. Check your input or sign up for an account.");
    //     logger.print(ERROR, "someone tried to login with '%s', but failed", user->username);
    // }

    // if (matched(user))
    // {
    //     puts("Logged in. Welcome!");
    //     LOGGED = 1;
    //     ACCESSIBLE = user->rooted;
    //     logger.print(INFO, "%s logged in", user->username);
    // }
    // else
    // {
    //     puts("Error: No such user. Check your input or sign up for an account.");
    //     logger.print(ERROR, "someone tried to login with '%s', but failed", user->username);
    // }
    return;
}

void userLogout(user *user)
{
    if (!LOGGED)
    {
        puts("You have not logged in!");
        logger.print(ERROR, "someone tried to logout, but he never logged in");
        return;
    }
    
    LOGGED = 0;
    ACCESSIBLE = 0;
    logger.print(INFO, "logged out");
    puts("Logged out. Bye.");
    return;
}

static int matched(user *inputUser)
{
    int matched = 0;
    FILE *userData = fopen("E:/BaiduSyncdisk/03_CODE/VSCode_Workspace/Infomation_Manager/userdata.txt", "r");
    char buffer[40] = {0};
    user readUser;
    while (fgets(buffer, 40, userData) != NULL)
    {
        sscanf(buffer, "%s %s %d\n", readUser.username, readUser.password, &readUser.rooted);
        if (!strcmp(readUser.username, inputUser->username) && !strcmp(readUser.password, inputUser->password))
        {
            inputUser->rooted = readUser.rooted;
            matched = 1;
        }
        
    }
    fclose(userData);
    return matched;
}

void createUser(user *user)
{
    int accessibility;
    int Changed = 1;
    char sql[64] = "INSERT OR IGNORE INTO USERS (username, password, accessibility) VALUES (?, ?, ?);";
    sqlite3_stmt *statement = NULL;

    if (!LOGGED)
    {
        puts("You have not logged in!");
        logger.print(ERROR, "someone tried to create a user, but he didn't logged in");
        return;
    }
    if (!ACCESSIBLE)
    {
        puts("You are not authorized!");
        logger.print(ERROR, "tried to create a user without an authorization");
        return;
    }

    printf("Is it authorized? (0 for no, 1 for yes): ");
    scanf("%d", &accessibility);
    getchar();

    // if (matchedName(user))
    // {
    //     puts("User exists. Please log in");
    //     logger.print(ERROR, "tried to create a user which exists");
    //     return;
    // }

    sqlite3_prepare_v2(parcelHub, sql, -1, &statement, NULL);

    sqlite3_bind_text(statement, 1, user->username, -1, SQLITE_STATIC);
    sqlite3_bind_text(statement, 2, user->password, -1, SQLITE_STATIC);
    sqlite3_bind_int(statement, 3, accessibility);

    sqlite3_step(statement);

    Changed = sqlite3_changes(parcelHub);

    if (!Changed)
    {
        puts("User exists. Please log in");
        logger.print(ERROR, "tried to create a user which exists");
    }
    else
    {
        printf("Created\n\n");
        logger.print(INFO, "create a user '%s'", user->username);
    }
    
    // FILE *userData = fopen("E:/BaiduSyncdisk/03_CODE/VSCode_Workspace/Infomation_Manager/userdata.txt", "a");
    // fprintf(userData, "%s %s %d\n", user->username, user->password, accessibility);
    // fclose(userData);
    
    return;
}

void deleteUser(user *User)
{
    int changed = -1;
    sqlite3_stmt *statement = NULL;
    char sql[64] = "DELETE FROM USERS WHERE username = ? AND password = ?;";

    sqlite3_prepare_v2(parcelHub, sql, -1, &statement, NULL);

    sqlite3_bind_text(statement, 1, User->username, -1, SQLITE_STATIC);
    sqlite3_bind_text(statement, 2, User->password, -1, SQLITE_STATIC);

    if (sqlite3_step(statement) == SQLITE_DONE)
    {
        logger.print(INFO, "deleted a user '%s'", User->username);
        printf("Deleted\n\n");
    }
    else
    {
        puts("User does not exists. Check your input.");
        logger.print(ERROR, "tried to delete a user which doesn't exist or input wrong password");
    }
    
    // if (!matchedName(User))
    // {
    //     puts("User does not exists. Check your input.");
    //     logger.print(ERROR, "tried to delete a user which doesn't exist");
    //     return;
    // }

    // FILE *userData = fopen("E:/BaiduSyncdisk/03_CODE/VSCode_Workspace/Infomation_Manager/userdata.txt", "r");
    // char ch;
    // int userCount = 0;
    // while ((ch = fgetc(userData)) != EOF)
    // {
    //     if (ch == '\n')
    //     {
    //         userCount++;
    //     }

    // }
    // user *users = malloc(userCount * sizeof(user));
    // char buffer[100];
    // user readUser;
    // rewind(userData);
    // for (int i = 0; i < userCount; i++)
    // {
    //     fscanf(userData, "%s %s %d\n", users[i].username, users[i].password, &users[i].rooted);
    //     if (!strcmp(users[i].username, User->username))
    //     {
    //         users[i].username[0] = '\0';
    //     }

    // }
    // fclose(userData);
    // userData = fopen("E:/BaiduSyncdisk/03_CODE/VSCode_Workspace/Infomation_Manager/userdata.txt", "w");
    // for (int i = 0; i < userCount; i++)
    // {
    //     if (users[i].username[0] == '\0')
    //     {
    //         continue;
    //     }
    //     fprintf(userData, "%s %s %d\n", users[i].username, users[i].password, users[i].rooted);
    // }
    return;
}

void updateUser(user *inputUser)
{
    //char ch;
    //int userCount = 0;
    //char buffer[100];
    //user readUser;
    
    typedef enum keys
    {
        USERNAME = 0,
        PASSWORD = 1,
        ROOT = 2
    } key;
    char query[64] = "SELECT * FROM USERS WHERE username = ?;";
    char update[64] = "UPDATE USERS SET ? = ? WHERE username = ?;";
    char criteria[16] = {0};

    sqlite3_prepare_v2(parcelHub, query, -1, &statement, NULL);

    sqlite3_bind_text(statement, 1, inputUser->username, -1, SQLITE_STATIC);

    if (sqlite3_step(statement) == SQLITE_ROW && !strcmp(sqlite3_column_text(statement, 1), inputUser->password))
    {
        sqlite3_prepare_v2(parcelHub, update, -1, &statement, NULL);
        sqlite3_bind_text(statement, 3, inputUser->username, -1, SQLITE_STATIC);

        printf("Enter the key to update: ");
        sgets(criteria, 16);

        if (!strcasecmp(criteria, "username"))
        {
            sqlite3_bind_text(statement, 1, "username", -1, SQLITE_STATIC);

            char newUsername[16];
            printf("Enter the new username: ");
            sgets(newUsername, 16);
            sqlite3_bind_text(statement, 2, newUsername, -1, SQLITE_STATIC);

            if (sqlite3_step(statement) == SQLITE_ROW)
            {
                printf("Changed username from %s into %s\n", inputUser->username, sqlite3_column_text(statement, 0));
            }
            else
            {
                printf("Database error: %s, nothing updated\n", sqlite3_errmsg(parcelHub));
            }
        }
        else if (!strcasecmp(criteria, "password"))
        {
            sqlite3_bind_text(statement, 1, "password", -1, SQLITE_STATIC);

            char newPassword[20];
            printf("Enter the new password: ");
            sgets(newPassword, 20);
            sqlite3_bind_text(statement, 2, newPassword, -1, SQLITE_STATIC);

            if (sqlite3_step(statement) == SQLITE_ROW)
            {
                printf("Changed password from %s into %s\n", inputUser->password, sqlite3_column_text(statement, 1));
            }
            else
            {
                printf("Database error: %s, nothing updated\n", sqlite3_errmsg(parcelHub));
            }
        }
        else if (!strcasecmp(criteria, "accessbility"))
        {
            int newAccessbility = -1;

            sqlite3_bind_text(statement, 1, "accessibility", -1, SQLITE_STATIC);
            printf("Enter the new accessibility: ");
            scanf("%d", &newAccessbility);
            getchar();
            sqlite3_bind_int(statement, 2, newAccessbility);

            if (sqlite3_step(statement) == SQLITE_ROW)
            {
                printf("Changed accessbility from %d into %d\n", inputUser->rooted, sqlite3_column_int(statement, 1));
            }
            else
            {
                printf("Database error: %s, nothing updated\n", sqlite3_errmsg(parcelHub));
            }
        }
    }
    else
    {
        puts("Error username or password input");
    }
    // if (!matched(inputUser))
    // {
    //     puts("User does not exists. Check your input.");
    //     logger.print(ERROR, "tried to update a user '%s' which doesn't exist", inputUser->username);
    //     return;
    // }

    // FILE *userData = fopen("E:/BaiduSyncdisk/03_CODE/VSCode_Workspace/Infomation_Manager/userdata.txt", "r");

    // while ((ch = fgetc(userData)) != EOF)
    // {
    //     if (ch == '\n')
    //     {
    //         userCount++;
    //     }
    // }
    // rewind(userData);
    // user *users = malloc(userCount * sizeof(user));
    // for (int i = 0; i < userCount; i++)
    // {
    //     fscanf(userData, "%s %s %d\n", users[i].username, users[i].password, &users[i].rooted);
    // }
    // fclose(userData);

    // userData = fopen("E:/BaiduSyncdisk/03_CODE/VSCode_Workspace/Infomation_Manager/userdata.txt", "w");
    // //* 先不做这个权限的更改了，目前来看权限是创建用户的时候就定好的
    // // puts("Enter the key you want to update: (password or root)");
    // // sgets(buffer, 10);
    // // int target = 0;
    // // if (!strcasecmp(buffer, "password"))
    // // {
    // //     target = PASSWORD;
    // // }
    // // else if (!strcasecmp(buffer, "root"))
    // // {
    // //     target = ROOT;
    // // }

    // for (int i = 0; i < userCount; i++)
    // {
    //     if (!strcmp(users[i].username, inputUser->username))
    //     {
    //         // switch (target)
    //         // {
    //         // case PASSWORD:

    //         //     break;

    //         // case ROOT:

    //         //     break;

    //         // default:
    //         //     break;
    //         // }

    //         char password[20] = {0};
    //         do
    //         {
    //             printf("Enter your original password: ");
    //             sgets(password, 20);
    //         } while (strcmp(password, inputUser->password));

    //         printf("Enter new password: ");
    //         sgets(password, 20);
    //         char newPassword[20] = {0};
    //         do
    //         {
    //             printf("Enter new password again: ");
    //             sgets(newPassword, 20);
    //         } while (strcmp(password, newPassword));

    //         strcpy(users[i].password, password);
    //         puts("Change applied.");
    //     }
    //     fprintf(userData, "%s %s %d\n", users[i].username, users[i].password, users[i].rooted);
    // }
    // printf("Updated\n\n\n");
    // logger.print(INFO, "updated a user '%s'", inputUser->username);
    // fclose(userData);
    return;
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

//! matchedName()与matched()这两个函数仅相差判断条件，可以尝试用宏定义的方法将它们合并简化，附加条件来调用代码，写这个代码是因为有的操作需要匹配用户，而有的操作需要只匹配用户名
int matchedName(user *inputUser)
{
    int matched = 0;
    FILE *userData = fopen("E:/BaiduSyncdisk/03_CODE/VSCode_Workspace/Infomation_Manager/userdata.txt", "r");
    char buffer[40] = {0};
    user readUser;
    while (fgets(buffer, 40, userData) != NULL)
    {
        sscanf(buffer, "%s %s %d\n", readUser.username, readUser.password, &readUser.rooted);
        if (!strcmp(readUser.username, inputUser->username)/* && !strcmp(readUser.password, inputUser->password)*/)
        {
            inputUser->rooted = readUser.rooted;
            matched = 1;
        }
        }
    fclose(userData);
    return matched;
}

int welcomeUser(void *data, int columnCount, char *columnValue[], char* columnName[])
{
    typedef enum columns
    {
        USERNAME = 0,
        PASSWORD,
        AUTHORIZATION
    } columns;
    char welcome[64] = {0};
    strcpy(welcome, (const char *)data);
    strcat(welcome, columnValue[USERNAME]);
    printf("%s\n", welcome);

    LOGGED = 1;
    ACCESSIBLE = columnValue[AUTHORIZATION] - 48;
    logger.print(INFO, "%s logged in", columnValue[USERNAME]);
    return 0;
}

static int databaseError(char *systemOpration, char *databaseOpration)
{
    if (databaseStatus != SQLITE_OK)
    {
        printf("%s: %s\n", databaseOpration, sqlite3_errmsg(parcelHub));
        logger.print(ERROR, "%s but failed when %s: %s", systemOpration, databaseOpration, errorMessage);
        return 1;
    }
    return 0;
}