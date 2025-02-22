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

static int databaseError(char *systemOpration, char *databaseOpration);

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
    snprintf(condition, 64, "WHERE username = ? AND password = ?;", user->username, user->password);
    strcat(sql, condition);

    databaseStatus = sqlite3_prepare_v2(parcelHub, sql, -1, &statement, NULL);

    sqlite3_bind_text(statement, 1, user->username, -1, SQLITE_STATIC);
    sqlite3_bind_text(statement, 2, user->password, -1, SQLITE_STATIC);

    if (sqlite3_step(statement) == SQLITE_ROW)
    {
        LOGGED = 1;
        user->rooted = atoi(sqlite3_column_text(statement, 2));
        ACCESSIBLE = user->rooted;
        printf("Welcome! %s\n\n", user->username);
        logger.print(INFO, "%s logged in", user->username);
    }
    else
    {
        puts("Error: No such user. Check your input or sign up for an account.");
        logger.print(ERROR, "someone tried to login with '%s', but failed", user->username);
    }

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
    char sql[128] = "INSERT OR IGNORE INTO USERS (username, password, accessibility) VALUES (?, ?, ?);";
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

    return;
}

void updateUser(user *inputUser)
{
    typedef enum keys
    {
        USERNAME = 0,
        PASSWORD = 1,
        ROOT = 2
    } key;
    char query[64] = "SELECT * FROM USERS WHERE username = ?;";
    char criteria[16] = {0};

    sqlite3_prepare_v2(parcelHub, query, -1, &statement, NULL);

    sqlite3_bind_text(statement, 1, inputUser->username, -1, SQLITE_STATIC);

    if (sqlite3_step(statement) == SQLITE_ROW && !strcmp(sqlite3_column_text(statement, 1), inputUser->password))
    {
        printf("Enter the key to update: ");
        sgets(criteria, 16);

        if (!strcasecmp(criteria, "username"))
        {
            char update[64] = "UPDATE USERS SET username = ? WHERE username = ?;";
            sqlite3_prepare_v2(parcelHub, update, -1, &statement, NULL);
            sqlite3_bind_text(statement, 2, inputUser->username, -1, SQLITE_STATIC);

            char newUsername[16];
            printf("Enter the new username: ");
            sgets(newUsername, 16);
            sqlite3_bind_text(statement, 1, newUsername, -1, SQLITE_STATIC);

            if (sqlite3_step(statement) == SQLITE_DONE)
            {
                printf("Changed username from %s into %s\n", inputUser->username, newUsername);
            }
            else
            {
                printf("Database error: %s, nothing updated\n", sqlite3_errmsg(parcelHub));
            }
        }
        else if (!strcasecmp(criteria, "password"))
        {
            char update[64] = "UPDATE USERS SET password = ? WHERE username = ?;";
            sqlite3_prepare_v2(parcelHub, update, -1, &statement, NULL);
            sqlite3_bind_text(statement, 2, inputUser->username, -1, SQLITE_STATIC);

            char newPassword[20];
            printf("Enter the new password: ");
            sgets(newPassword, 20);
            sqlite3_bind_text(statement, 1, newPassword, -1, SQLITE_STATIC);

            if (sqlite3_step(statement) == SQLITE_DONE)
            {
                printf("Changed password from %s into %s\n", inputUser->password, newPassword);
            }
            else
            {
                printf("Database error: %s, nothing updated\n", sqlite3_errmsg(parcelHub));
            }
        }
        else if (!strcasecmp(criteria, "accessibility"))
        {
            int newAccessbility = -1;

            char update[64] = "UPDATE USERS SET accessibility = ? WHERE username = ?;";
            sqlite3_prepare_v2(parcelHub, update, -1, &statement, NULL);
            sqlite3_bind_text(statement, 2, inputUser->username, -1, SQLITE_STATIC);

            printf("Enter the new accessibility: ");
            scanf("%d", &newAccessbility);
            getchar();
            sqlite3_bind_int(statement, 1, newAccessbility);

            if (sqlite3_step(statement) == SQLITE_DONE)
            {
                printf("Changed accessbility from %d into %d\n", inputUser->rooted, newAccessbility);
            }
            else
            {
                printf("Database error: %s, nothing updated\n", sqlite3_errmsg(parcelHub));
            }
        }
        else
        {
            puts("Error criteria input");
        }
    }
    else
    {
        puts("Error username or password input");
    }

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