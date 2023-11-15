#include "utils.h"
#include "globals.h"

char* strdup(const char* src){
    int length = strlen(src)+1;
    char* dst=malloc(length * sizeof(char));
    if(dst != NULL){
        memcpy(dst, src, length);
    }
    return dst;
}
