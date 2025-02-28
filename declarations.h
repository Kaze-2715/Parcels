#ifndef DECLARATIONS_H
#define DECLARATIONS_H

//* main.c
void help();
void start();
void login();
void logout();
void halt();
void dataFilter();
void dataSort();
void dataVisual();
void inBound();
void outBound();
void delete();
void update();
void select();
void userCreate();
void userDelete();
void userUpdate();

//* data.c
void filter();
void sort();

//* parcelOprations.c
int chooseToContinue();
int getCommand();
void deleteLine(char *ID);
void updateLine(char *ID);
void selectLine(char *ID);
int inbound();
void outbound(char *ID);

#endif