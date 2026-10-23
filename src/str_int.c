#include <stdlib.h>
#include <stdio.h>


int str_to_int(char* str) {
    return atoi(str);
}

int int_to_str(int n, char* text, int maxlen) {
    int k = snprintf(text, maxlen, "%d", n);
    if (k < 0 || k >= maxlen) 
        return -1;
    return 0;
}