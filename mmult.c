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
#include <time.h>
#include <pthread.h>

void noThreadMulti(char* file1, char* file2, char* ofile);
void threadMulti(char* file1, char* file2, char* ofile, int threads);
void* calculate(void* x);

struct arg_struct{
    int i;
    int j;
    int f1col;
    int f2col;
    char* file1;
    char* file2;
};

int main(int argc, char** argv){
    int threads;

    if(argc < 4 || argc > 5){
        printf("mmult usage: threads(optional),matrix file,matrix file,output\n");
    }
    
    else if(argc == 4){
       clock_t t;
       t = clock();
       noThreadMulti(argv[1],argv[2],argv[3]);
       t = clock() - t;
       double time_take = ((double)t)/CLOCKS_PER_SEC;
       printf("Worker threads:  0\n");
       printf("\nTotal time:  %f seconds.\n\n", time_take);
    }

    else if(argc == 5){
        threads = atoi(argv[1]);
        clock_t t;
        t = clock();
        threadMulti(argv[2], argv[3], argv[4], atoi(argv[1]));
        t = clock() - t;
        double time_take = ((double)t)/CLOCKS_PER_SEC;
        printf("Worker threads:  %d\n", threads);
        printf("\nTotal time:  %f seconds.\n\n", time_take);
    }
    return 0;
}

void noThreadMulti(char* file1, char* file2, char* ofile){
    FILE* f1;
    FILE* f2;
    FILE* outf;
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
    if(f1col != f2row){
        printf("mmult: rows of %s and columns of %s don't match.\n",
                file1, file2);
        return;
    }
    outf = fopen(ofile, "w");
    fwrite(&f1row, 1, sizeof(int), outf);
    fwrite(&f2col, 1, sizeof(int), outf);
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
            fwrite(&entry, 1, sizeof(double), outf);
        }
    }
    printf("\nMatrix sizes:\n");
    printf("    M:%d\n", f1row);
    printf("    N:%d\n", f1col);
    printf("    P:%d\n", f2col);
    return;
}


void threadMulti(char* file1, char* file2, char* ofile, int threads){
    FILE* f1;
    FILE* f2;
    FILE* outf;
    int f1row = -1;
    int f1col = -1;
    int f2row = -1;
    int f2col = -1;
    pthread_t thread_list[threads];

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
    if(f1col != f2row){
        printf("mmult: rows of %s and columns of %s don't match.\n",
                file1, file2);
        return;
    }
    fclose(f1);
    fclose(f2);
    outf = fopen(ofile, "w");
    fwrite(&f1row, 1, sizeof(int), outf);
    fwrite(&f2col, 1, sizeof(int), outf);
    for(int i = 0; i < f1row; i++){
        for(int j = 0; j < f2col; j++){
            void* entry;
            for(int l = 0; l < threads; l++){
                struct arg_struct args;
                args.i = i;
                args.j = j;
                args.file1 = file1;
                args.file2 = file2;
                args.f1col = f1col;
                args.f2col = f2col;
                pthread_create(&thread_list[l], NULL,calculate,&args);
            }
            for(int l = 0; l < threads; l++){
                pthread_join(thread_list[l],&entry);
                fwrite(entry, 1, sizeof(double), outf);
            }
        }
    }
    printf("\nMatrix sizes:\n");
    printf("    M:%d\n", f1row);
    printf("    N:%d\n", f1col);
    printf("    P:%d\n", f2col);
    return;
}


void* calculate(void* x){
    struct arg_struct *args = x;
    int i = args->i;
    int j = args->j;
    int f1col = args->f1col;
    int f2col = args->f2col;
    char* file1 = args->file1;
    char* file2 = args->file2;
    FILE* f1;
    FILE* f2;
    f1 = fopen(file1, "r");
    f2 = fopen(file2, "r");
    double entry = 0;
    for(int k = 0; k < args->f1col; k++){
        int f1Off = (i*f1col + k)*8 + 8;
        int f2Off = (k*f2col + j)*8 + 8;
        double read1;
        double read2;
        fseek(f1, f1Off, SEEK_SET);
        fseek(f2, f2Off, SEEK_SET);
        fread(&read1, sizeof(double), 1, f1);
        fread(&read2, sizeof(double), 1, f2);
        entry += (read1*read2);
    }
    double* ret = malloc(sizeof(double));
    *ret = entry;
    void* retV;
    retV = (void *) ret;
    fclose(f1);
    fclose(f2);
    return retV;
}



















