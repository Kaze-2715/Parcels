typedef struct Time
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
} Time;

typedef struct Parcel
{
    char from[10];
    char to[10];
    char ID[20];
    int state;
    Time unload_time;
    Time load_time;
} parcel;

typedef struct node
{
    parcel data;
    struct node *next;
} node;

typedef enum STATUS
{
    INBOUND = 0,
    OUTBOUND
} STATUS;

typedef enum Command
{
    HELP = 0,
    START,
    LOGIN,
    LOGOUT,
    STOP,
    RELOAD,
    SAVE,
    HALT,
    DATA_FILTER,
    DATA_SORT,
    DATA_VISUALIZATION, //* If the code is less than 10, it will need permission.
    IN,
    OUT,
    DELETE,
    UPDATE,
    SELECT, //* If the code is between 4 and 14, it will need to start the system.
    USER_CREATE,
    USER_DELETE,
    USER_UPDATE,
    LOCK_ACCOUNT,
    UNLOCK_ACCOUNT,
} cmd;
