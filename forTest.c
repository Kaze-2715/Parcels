#include "defines.h"
#include <stdio.h>

int main(void)
{
    FILE *fp = fopen("E:/BaiduSyncdisk/03_CODE/VSCode_Workspace/parcel_service/parcels.txt", "r");

    if (fp == NULL)
    {
        perror("parcel.txt");
    }
    
    parcel one;
    one.state = 0;
    fscanf(fp, "%[^,],%[^,],%[^,],%d,%d.%d.%d.%d.%d.%d,", one.from, one.to, one.ID, &one.state, &one.load_time.year, &one.load_time.month, &one.load_time.day, &one.load_time.hour, &one.load_time.minute, &one.load_time.second);

    if (one.state) //*To check if this one has been fetched. If fetched, read in the time, or set manually.
    {
        fscanf(fp, "%d.%d.%d.%d.%d.%d\n", &one.unload_time.year, &one.unload_time.month, &one.unload_time.day, &one.unload_time.hour, &one.unload_time.minute, &one.unload_time.second);
    }
    else
    {
        one.unload_time.year = 0;
        one.unload_time.month = 0;
        one.unload_time.day = 0;
        one.unload_time.hour = 0;
        one.unload_time.minute = 0;
        one.unload_time.second = 0;
    }

    fclose(fp);

    printf("%d.%d.%d.%d.%d.%d\n", one.unload_time.year, one.unload_time.month, one.unload_time.day, one.unload_time.hour, one.unload_time.minute, one.unload_time.second);
    // return one;
}
