#ifndef DECLARATIONS_H
#define DECLARATIONS_H

#include <stdio.h>

//* file_io.c 
int readLine(parcel *one, FILE *parcelData);
int matchedLine(node *buffer, char *ID);
void writeLine(parcel *one, FILE *parcelData);
void deleteLine(char *ID);
void updateLine(char *ID);
void selectLine(char *ID);
FILE *openParcel(char *mode);
FILE *openLog(char *mode);
node *loadLinklist(FILE *fp);

//* user_io.c
int cmd2code(char command[]);
int getCommand();
int getchoose();
void outputParcel(parcel *one);
parcel inputParcel();

//* main.c
int help();
void start();
void login();
void logout();
void stop();
void reload();
void save();
void halt();
void dataFilter();
void dataSort();
void dataVisual();
void add();
void delete();
void update();
void select();
void userCreate();
void userDelete();
void userUpdate();

//* data.c
void filter(char *key);
void sort(char *key);

#endif