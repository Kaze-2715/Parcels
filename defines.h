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

typedef enum STATUS
{
    IN = 0,
    OUT
} STATUS;

typedef enum Command
{
    HELP = 0,
    LOGIN,
    LOGOUT,
    HALT,
    DATA_FILTER,
    DATA_SORT,
    SELECT, //* If the code is greater than SELECT, it will need permission.
    INBOUND,
    OUTBOUND,
    DELETE,
    UPDATE,
    USER_CREATE,
    USER_DELETE,
    USER_UPDATE
} cmd;

typedef struct
    {
        const char *command;
        cmd code;
    } CommandMapping;
