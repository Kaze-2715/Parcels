typedef enum Object
{
    PARCEL = 0,
    LOG
} object;

typedef struct Time
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
} time;

typedef struct Parcel
{
    char from[10];
    char to[10];
    char ID[20];
    int state;
    time unload_time;
    time load_time;
} parcel;

typedef struct Log
{
    char user[10];
    char operation[15];
    object obj;
    time log_time;
} log;

typedef struct node
{
    parcel data;
    struct node *next;
} node;
