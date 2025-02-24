#ifndef USER_H_
#define USER_H_
typedef struct user user;

struct user
{
    char username[20];
    char password[20];
    int rooted;
    void (*login)(user *user);
    void (*logout)(user *user);
    void (*create)(user *user);
    void (*remove)(user *user);
    void (*update)(user *user);
};

extern int LOGGED, ACCESSIBLE;

void userLogin(user *user);

void userLogout(user *logout);

user getUser();

void createUser(user *user);

void deleteUser(user *user);

void updateUser(user *user);

#endif