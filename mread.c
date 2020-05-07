#include<stdio.h>
#include<stdlib.h>

int main(int argc, char** argv){
    FILE * f;
    int row;
    int col;
    f = fopen(argv[1], "r");
    fread(&row, sizeof(int), 1, f);
    fseek(f, 4, SEEK_SET);
    fread(&col, sizeof(int), 1, f);
    printf("rows: %d, cols: %d\n", row, col);
    for(int i = 0; i < (row * col); i++){
        double entry;
        int offset = (i * 8) + 8;
        fseek(f, offset, SEEK_SET);
        fread(&entry, sizeof(double), 1, f);
        printf("%f ", entry);
        if(!((i+1)%row)){
            printf("\n");
        }
    }
    printf("\n");
}
