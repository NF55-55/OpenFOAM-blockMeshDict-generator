#include <stdio.h>

void scalarVectProd(double *vect, double scalar, double length){
    for (int i = 0; i < length; i++){
        *(vect+i) = *(vect+i) * scalar;
    }
}