/*
 * mmult.c
 * LUCAS MANKER
 * 5/7/20
 * COSC 3750
 * HOMEWORK 11
 *
 * Matrix multiplication with threads
*/

#include <stdio.h>
#include <stdlib.h>

void noThreadMulti(char* file1, char* file2, char* ofile);

int main(int argc, char** argv){
    int threads;

    if(argc < 4 || argc > 5){
        printf("mmult usage: threads(optional),matrix file,matrix file,output\n");
    }
    
    else if(argc == 4){
       noThreadMulti(argv[1],argv[2],argv[3]);
    }

    else if(argc == 5){
        threads = atoi(argv[1]);
        printf("ok you want %d threads!", threads);
    }
    return 0;
}

void noThreadMulti(char* file1, char* file2, char* ofile){
    FILE* f1;
    FILE* f2;
    //FILE* outf;
    int f1row = -1;
    int f1col = -1;
    int f2row = -1;
    int f2col = -1;

    if((f1 = fopen(file1, "r"))){
        fread(&f1row,sizeof(int), 1, f1);
        fseek(f1, 4, SEEK_SET);
        fread(&f1col,sizeof(int), 1, f1);
    }
    else{
        printf("mmult: could not read file: %s\n", file1);
        return;
    }
    if((f2 = fopen(file2, "r"))){
        fread(&f2row,sizeof(int), 1, f2);
        fseek(f2, 4, SEEK_SET);
        fread(&f2col,sizeof(int), 1, f2);
    }
    else{
        printf("mmult: could not read file: %s\n", file2);
        return;
    }
    for(int i = 0; i < f1row; i++){
        for(int j = 0; j < f2col; j++){
            double entry = 0;
            for(int k = 0; k < f1col; k++){
                double read1;
                double read2;
                int f1Off = (i*f1col + k)*8 + 8;
                int f2Off = (k*f2col + j)*8 + 8;
                fseek(f1, f1Off, SEEK_SET);
                fseek(f2, f2Off, SEEK_SET);
                fread(&read1, sizeof(double), 1, f1);
                fread(&read2, sizeof(double), 1, f2);
                entry += (read1*read2);
            }
            printf("%f ", entry);
        }
        printf("\n");
    }
    return;
}





























