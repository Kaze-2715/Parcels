#ifndef FILE_IO_H
#define FILE_IO_H

#include "defines.h"
#include <stdio.h>
int readLine(parcel *one, FILE *parcelData);
int matchedLine(node *buffer, char *ID);
void writeLine(parcel *one, FILE *parcelData);
void deleteLine(char *ID);
void updateLine(char *ID);
void selectLine(char *ID);
FILE *openParcel(char *mode);
FILE *openLog(char *mode);
node *loadLinklist(FILE *fp);
node *freeLinklist(node *list);
#endif