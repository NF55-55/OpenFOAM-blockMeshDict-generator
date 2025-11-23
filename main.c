#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "printVect.h"
#include "ones.h"
#include "scalarVectProd.h"

// ---------------Global variables---------------

const int blockDim = (8*3);
const double emptyVal = 555.555;

// convention: x-z is the ground plane while y is the vertical axis, the coordinates of a block are defined starting from the x-y face with
// lowest z value, from the bottom left corner following the right-hand rule (w.r.t. z axis), like it's done in the blockMeshDict.
// size of block coordinates: 1x(3*8) (8 vertices x 3 coordinates) -> {x1,y1,z1,x2,y2,z2,x3,y3,z3,...,x8,y8,z8}

// ---------------Functions---------------

// generates a block which is defined by its 8 vertices (output), inputs are: initial point (bottom left of x-y face with lowest z), 
// x, y, z length of edges, pointer of output
void genBlock(double *initCoords, double xLength, double yLength, double zLength, double *coords){
    // vertex 1
    *coords = *initCoords;
    *(coords + 1) = *(initCoords + 1);
    *(coords + 2) = *(initCoords + 2);
    // vertex 2
    *(coords + 3) = *(initCoords) + xLength;
    *(coords + 4) = *(initCoords + 1);
    *(coords + 5) = *(initCoords + 2);
    // vertex 3
    *(coords + 6) = *(initCoords) + xLength;
    *(coords + 7) = *(initCoords + 1) + yLength;
    *(coords + 8) = *(initCoords + 2);
    // vertex 4
    *(coords + 9) = *(initCoords);
    *(coords + 10) = *(initCoords + 1) + yLength;
    *(coords + 11) = *(initCoords + 2);
    // vertex 5
    *(coords + 12) = *initCoords;
    *(coords + 13) = *(initCoords + 1);
    *(coords + 14) = *(initCoords + 2) + zLength;
    // vertex 6
    *(coords + 15) = *(initCoords) + xLength;
    *(coords + 16) = *(initCoords + 1);
    *(coords + 17) = *(initCoords + 2) + zLength;
    // vertex 7
    *(coords + 18) = *(initCoords) + xLength;
    *(coords + 19) = *(initCoords + 1) + yLength;
    *(coords + 20) = *(initCoords + 2) + zLength;
    // vertex 8
    *(coords + 21) = *(initCoords);
    *(coords + 22) = *(initCoords + 1) + yLength;
    *(coords + 23) = *(initCoords + 2) + zLength;
}

// generates a block (set of coordinates of its vertices) starting from a point of the reference block (chosen depending on dir.)
// giving the direction (and sign, e.g.: "x" -1 -> negative dir. of x) in which we want to create the block w.r.t. the ref. block, the
// block dimensions in x, y, z are also given as well as the pointer of the output coordinates.
// the sizes given are automatically adjusted so to perfectly overlap the reference block (those lengths are overwritten)
void genBlockWoffset(double *coordsRefBlock, char dir, int sign, double xLength, double yLength, double zLength, double *coords){
    double refPoint[3];
    if (dir == 'x' && sign == 1){
        *refPoint = *(coordsRefBlock + 3);
        *(refPoint + 1) = *(coordsRefBlock + 4);
        *(refPoint + 2) = *(coordsRefBlock + 5);
        yLength = *(coordsRefBlock + 10) - *(coordsRefBlock + 1);
        zLength = *(coordsRefBlock + 14) - *(coordsRefBlock + 2);
        genBlock(refPoint, xLength, yLength, zLength, coords);
    } else if (dir == 'x' && sign == -1){
        *refPoint = *coordsRefBlock - xLength;
        *(refPoint + 1) = *(coordsRefBlock + 1);
        *(refPoint + 2) = *(coordsRefBlock + 2);
        yLength = *(coordsRefBlock + 10) - *(coordsRefBlock + 1);
        zLength = *(coordsRefBlock + 14) - *(coordsRefBlock + 2);
        genBlock(refPoint, xLength, yLength, zLength, coords);
    } else if (dir == 'y' && sign == 1){
        *refPoint = *(coordsRefBlock + 9);
        *(refPoint + 1) = *(coordsRefBlock + 10);
        *(refPoint + 2) = *(coordsRefBlock + 11);
        xLength = *(coordsRefBlock + 3) - *(coordsRefBlock);
        zLength = *(coordsRefBlock + 14) - *(coordsRefBlock + 2);
        genBlock(refPoint, xLength, yLength, zLength, coords);
    } else if (dir == 'y' && sign == -1){
        *refPoint = *(coordsRefBlock);
        *(refPoint + 1) = *(coordsRefBlock + 1) - yLength;
        *(refPoint + 2) = *(coordsRefBlock + 2);
        xLength = *(coordsRefBlock + 3) - *(coordsRefBlock);
        zLength = *(coordsRefBlock + 14) - *(coordsRefBlock + 2);
        genBlock(refPoint, xLength, yLength, zLength, coords);
    } else if (dir == 'z' && sign == 1){
        *refPoint = *(coordsRefBlock + 12);
        *(refPoint + 1) = *(coordsRefBlock + 13);
        *(refPoint + 2) = *(coordsRefBlock + 14);
        xLength = *(coordsRefBlock + 3) - *(coordsRefBlock);
        yLength = *(coordsRefBlock + 10) - *(coordsRefBlock + 1);
        genBlock(refPoint, xLength, yLength, zLength, coords);
    } else if (dir == 'z' && sign == -1){
        *refPoint = *(coordsRefBlock);
        *(refPoint + 1) = *(coordsRefBlock + 1);
        *(refPoint + 2) = *(coordsRefBlock + 2) - zLength;
        xLength = *(coordsRefBlock + 3) - *(coordsRefBlock);
        yLength = *(coordsRefBlock + 10) - *(coordsRefBlock + 1);
        genBlock(refPoint, xLength, yLength, zLength, coords);
    }
    
}

// checks if the cooordinate (x,y,z) are contained in the ref. vector coordsRef = {x1,y1,z1,x2,y2,z2,...,x8,y8,z8}, the output is
// boolean true or false
bool checkDuplicatedVertex(double x, double y, double z, double *coordsRef, int lengthRef){
    bool isDuplicate = false;
    for (int i = 0; i < (lengthRef/3); i++){
        if (x == *(coordsRef + 3*i)){
            if (y == *(coordsRef + 3*i + 1)){
                if (z == *(coordsRef + 3*i + 2)){
                    isDuplicate = true;
                    break;
                }
            }
        }
    }
    return isDuplicate;
}

// appends array2 at the end of array1 excluding the duplicated coordinates, the number of new vertices is always 8 even if there
// are case in which they are 4,2,1 or 0 (repetition of a block), a function will later be used to clean the "empty spots" of 
// coordsTot
void cat(double *coordsB1, int lengthB1, double *coordsB2, double *coordsTot){
    bool isDuplicate;
    int i;
    for (i = 0; i < lengthB1; i++) *(coordsTot + i) = *(coordsB1 + i);
    for (int j = 0; j < 8; j++){
        isDuplicate = checkDuplicatedVertex(*(coordsB2+3*j), *(coordsB2+3*j+1), *(coordsB2+3*j+2), coordsB1, lengthB1);
        if (!isDuplicate){
            *(coordsTot + i) = *(coordsB2 + 3*j);
            i++;
            *(coordsTot + i) = *(coordsB2 + 3*j + 1);
            i++;
            *(coordsTot + i) = *(coordsB2 + 3*j + 2);
            i++;
        }
    }
}

// counts and returns the number of consecutive non-empty elements of the array starting from indx 0, empty element are recognized with
// the global constant "emptyVal"
int countNonEmptyElem(double *fullVect, int length){
    int lengthClean;
    for (lengthClean = 0; lengthClean < length; lengthClean++){
        if (*(fullVect+lengthClean)==emptyVal) break;
    }
    return lengthClean;
}

// it returns an array cleanVect which is the "clean" version of fullVect, meaning without empty elements
void delEmptyElem(double *fullVect, double *cleanVect, int lengthClean){
    for (int i = 0; i < lengthClean; i++) *(cleanVect + i) = *(fullVect + i);
}

// concatenates the old array of coordinates with a new block + cleans the new array from empty elements
void catAndClean(double **coordsTot1, double **coordsTot2, int *lengthClean, double *coordsNew, int *count){
    *coordsTot1 = realloc(*coordsTot1, (*lengthClean + 8*3) * (int)sizeof(double)); //double coordsTot3[lengthClean + 8*3];
    if (*coordsTot1 == NULL) {
        perror("realloc failed for coordsTot1");
        exit(EXIT_FAILURE);
    }
    int size1 = *lengthClean + 8*3;
    ones(*coordsTot1, size1);
    scalarVectProd(*coordsTot1, emptyVal, size1);
    cat(*coordsTot2, *lengthClean, coordsNew, *coordsTot1);
    *lengthClean = countNonEmptyElem(*coordsTot1, size1);
    *coordsTot2 = realloc(*coordsTot2, *lengthClean * (int)sizeof(double));
    if (*coordsTot2 == NULL) {
        perror("realloc failed for coordsTot2");
        exit(EXIT_FAILURE);
    }
    delEmptyElem(*coordsTot1,*coordsTot2,*lengthClean);
    (*count)++;
}

// creates the blockMeshDict template and adds the vertices coordinates as well as the creation of blocks
void blockMeshDict(FILE *f, double *coordsTot2, int lengthClean, double converToMeters, int nBlocks){
    fprintf(f, "/*--------------------------------*- C++ -*----------------------------------*\n");
    fprintf(f, "  =========                 |\n");
    fprintf(f, "  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox\n");
    fprintf(f, "   \\    /   O peration     | Website:  https://openfoam.org\n");
    fprintf(f, "    \\  /    A nd           | Version:  13\n");
    fprintf(f, "     \\/     M anipulation  |\n");
    fprintf(f, "*---------------------------------------------------------------------------*/\n");
    fprintf(f, "FoamFile\n{\n    format      ascii;\n    class       dictionary;\n    object      blockMeshDict;\n}\n");
    fprintf(f, "// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //\n");
    fprintf(f, "\n");
    fprintf(f, "convertToMeters %f;\n", converToMeters);
    fprintf(f, "\n");
    fprintf(f, "vertices\n(\n");
    for (int i = 0; i < (lengthClean/3); i++){
        fprintf(f, "	(%f %f %f)\n", *(coordsTot2+3*i), *(coordsTot2+3*i+1), *(coordsTot2+3*i+2));
    }
    fprintf(f, ");\n");
    fprintf(f, "\n");
    fprintf(f, "blocks\n(\n");
    for (int i = 0; i < nBlocks; i++){
        fprintf(f, "	hex ()  (1 1 1) simpleGrading (1 1 1) // block %d\n",(i+1));
    }
    fprintf(f, ");\n");
    fprintf(f, "\n");
    fprintf(f, "boundary\n(\n");
    fprintf(f, "    inlet\n    {\n        type patch;\n        faces\n        (\n        	()\n        );\n    }\n");
    fprintf(f, "\n");
    fprintf(f, "    outlet\n    {\n        type patch;\n        faces\n        (\n        	()\n        );\n    }\n");
    fprintf(f, "\n");
    fprintf(f, "    walls\n    {\n        type wall;\n        faces\n        (\n        	()\n        );\n    }\n");
    fprintf(f, "\n");
    fprintf(f, ");\n");
    fprintf(f, "\n");
    fprintf(f, "// ************************************************************************* //");
}


// ---------------Main---------------

int main(void){
    // generation of blocks (vertices coordinates)

    double convertToMeters = 1; // like the one in blockMeshDict

    // some useful dimensions
    double h = 0.04;
    double L = 3;
    double H = h/0.2;
    double l = 4*H;
    double W = 2*H;

    double coords1[blockDim];
    double pos0[3] = {0,0,0};
    genBlock(pos0,l,h,W,coords1);
    double coords2[blockDim];
    genBlockWoffset(coords1, 'x', 1, L, 0, 0, coords2);
    double coords3[blockDim];
    genBlockWoffset(coords2, 'y', -1, 0, (H-h), 0, coords3);
    
    int nBlocks = 3;

    // creating array of all vertices to visualize and create blockMeshDict
    // - only for the first 2 blocks:
    int numelB1 = 8*3;
    double *coordsTot1 = malloc((numelB1 + 8*3) * (int)sizeof(double)); //double coordsTot1[numelB1 + 8*3];
    if (coordsTot1 == NULL) {
        perror("malloc failed for coordsTot1");
        exit(EXIT_FAILURE);
    }
    int size1 = numelB1 + 8*3;
    ones(coordsTot1, size1);
    scalarVectProd(coordsTot1, emptyVal, size1);
    cat(coords1, numelB1, coords2, coordsTot1);
    int lengthClean = countNonEmptyElem(coordsTot1, size1);
    double *coordsTot2 = malloc(lengthClean * (int)sizeof(double));//double coordsTot2[lengthClean];
    if (coordsTot2 == NULL) {
        perror("malloc failed for coordsTot2");
        exit(EXIT_FAILURE);
    }
    delEmptyElem(coordsTot1,coordsTot2,lengthClean);

    // - for the remaining blocks:

    int countB = 2;

    catAndClean(&coordsTot1, &coordsTot2, &lengthClean, coords3, &countB);

    

    if (countB != nBlocks) printf("!Warning: make sure to concatenate all blocks, concatenated ones != created ones.");

    // post-processing
    printf("Total number of vertices: %d.\n", (lengthClean/3));

    FILE *f = fopen("outputCoords.csv", "w"); // for python visualization
    for (int i = 0; i < (lengthClean/3); i++){
        fprintf(f, "%f,%f,%f\n", *(coordsTot2+3*i), *(coordsTot2+3*i+1), *(coordsTot2+3*i+2));
    }
    fclose(f);

    FILE *g = fopen("BlockMeshDict", "w"); // for blockMeshDict implementation
    blockMeshDict(g,coordsTot2,lengthClean,convertToMeters,nBlocks);
    fclose(g);

    free(coordsTot1);
    free(coordsTot2);

    // executing the python script to visualize the domain
    system("python ./visulizeDomain.py"); // make sure that the script is in the same folder as this code

    return 0;
}