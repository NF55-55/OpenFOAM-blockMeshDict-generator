#include <stdio.h>

void ones(double *initVect, int length){
    for (int i = 0; i < length; i++){
        *(initVect+i) = 1;
    }
}