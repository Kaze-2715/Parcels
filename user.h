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

// TODO login
void userLogin(user *user);
// TODO logout
void userLogout(user *logout);
// TODO getuser
user getUser();

// TODO create
int createUser(user *user);

// TODO delete
int deleteUser(user *user);
#endif

// TODO matchUser
static int matched(user *user);

// TODO updateUser
int updateUser(user *user);

static char *sgets(char *buffer, int size);