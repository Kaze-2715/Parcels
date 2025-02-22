#ifndef USER_IO_H
#define USER_IO_H

int cmd2code(char command[]);
int getCommand();
int chooseToContinue();
void outputParcel(parcel *one);
parcel inputParcel();

#endif