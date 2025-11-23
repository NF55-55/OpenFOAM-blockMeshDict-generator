#include <stdio.h>

void printVect(double *vect, int length){
    printf("[");
    for (int i = 0; i < length; i++){
        if (i < (length-1)){
            printf("%f ", *(vect+i));
        } else {
            printf("%f]", *(vect+i));
        }
        
    }
}