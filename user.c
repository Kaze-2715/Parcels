#include "user.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
    if (matched(user))
    {
        puts("Logged in. Welcome!");
        LOGGED = 1;
        ACCESSIBLE = user->rooted;
    }
    else
    {
        puts("Error: No such user. Check your input or sign up for an account.");
    }
    return;
}

void userLogout(user *user)
{
    if (!LOGGED)
    {
        puts("You have not logged in!");
        return;
    }
    
    LOGGED = 0;
    ACCESSIBLE = 0;
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

    if (!LOGGED)
    {
        puts("You have not logged in!");
        return;
    }
    if (!ACCESSIBLE)
    {
        puts("You are not authorized!");
        return;
    }
    
    if (matchedName(user))
    {
        puts("User exists. Please log in");
        return;
    }

    printf("Is it authorized? (0 for no, 1 for yes): ");
    scanf("%d", &accessibility);
    getchar();
    FILE *userData = fopen("E:/BaiduSyncdisk/03_CODE/VSCode_Workspace/Infomation_Manager/userdata.txt", "a");
    fprintf(userData, "%s %s %d\n", user->username, user->password, accessibility);
    fclose(userData);
    printf("Created\n\n\n");
    return;
}

void deleteUser(user *User)
{
    if (!matchedName(User))
    {
        puts("User does not exists. Check your input.");
        return;
    }
    
    FILE *userData = fopen("E:/BaiduSyncdisk/03_CODE/VSCode_Workspace/Infomation_Manager/userdata.txt", "r");
    char ch;
    int userCount = 0;
    while ((ch = fgetc(userData)) != EOF)
    {
        if (ch == '\n')
        {
            userCount++;
        }
        
    }
    user *users = malloc(userCount * sizeof(user));
    char buffer[100];
    user readUser;
    rewind(userData);
    for (int i = 0; i < userCount; i++)
    {
        fscanf(userData, "%s %s %d\n", users[i].username, users[i].password, &users[i].rooted);
        if (!strcmp(users[i].username, User->username))
        {
            users[i].username[0] = '\0';
        }
        
    }
    fclose(userData);
    userData = fopen("E:/BaiduSyncdisk/03_CODE/VSCode_Workspace/Infomation_Manager/userdata.txt", "w");
    for (int i = 0; i < userCount; i++)
    {
        if (users[i].username[0] == '\0')
        {
            continue;
        }
        fprintf(userData, "%s %s %d\n", users[i].username, users[i].password, users[i].rooted);
    }
    printf("Deleted\n\n\n");
    return;
}

void updateUser(user *inputUser)
{
    char ch;
    int userCount = 0;
    char buffer[100];
    user readUser;
    typedef enum keys
    {
        PASSWORD = 1,
        ROOT = 2
    } key;

    if (!matched(inputUser))
    {
        puts("User does not exists. Check your input.");
        return;
    }

    FILE *userData = fopen("E:/BaiduSyncdisk/03_CODE/VSCode_Workspace/Infomation_Manager/userdata.txt", "r");
    
    while ((ch = fgetc(userData)) != EOF)
    {
        if (ch == '\n')
        {
            userCount++;
        }
    }
    rewind(userData);
    user *users = malloc(userCount * sizeof(user));
    for (int i = 0; i < userCount; i++)
    {
        fscanf(userData, "%s %s %d\n", users[i].username, users[i].password, &users[i].rooted);
    }
    fclose(userData);

    userData = fopen("E:/BaiduSyncdisk/03_CODE/VSCode_Workspace/Infomation_Manager/userdata.txt", "w");
    //* 先不做这个权限的更改了，目前来看权限是创建用户的时候就定好的
    // puts("Enter the key you want to update: (password or root)");
    // sgets(buffer, 10);
    // int target = 0;
    // if (!strcasecmp(buffer, "password"))
    // {
    //     target = PASSWORD;
    // }
    // else if (!strcasecmp(buffer, "root"))
    // {
    //     target = ROOT;
    // }

    for (int i = 0; i < userCount; i++)
    {
        if (!strcmp(users[i].username, inputUser->username))
        {
            // switch (target)
            // {
            // case PASSWORD:

                
            //     break;

            // case ROOT:

            //     break;

            // default:
            //     break;
            // }

            char password[20] = {0};
            do
            {
                printf("Enter your original password: ");
                sgets(password, 20);
            } while (strcmp(password, inputUser->password));

            printf("Enter new password: ");
            sgets(password, 20);
            char newPassword[20] = {0};
            do
            {
                printf("Enter new password again: ");
                sgets(newPassword, 20);
            } while (strcmp(password, newPassword));

            strcpy(users[i].password, password);
            puts("Change applied.");
        }
        fprintf(userData, "%s %s %d\n", users[i].username, users[i].password, users[i].rooted);
    }
    printf("Updated\n\n\n");
    fclose(userData);
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
