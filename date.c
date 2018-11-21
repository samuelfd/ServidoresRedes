#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    char data[50];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    data = itoa((tm.tm_year + 1900) (tm.tm_mon + 1) (tm.tm_mday) (tm.tm_hour) (tm.tm_min) (tm.tm_sec),data,10);

    return 0;
}
