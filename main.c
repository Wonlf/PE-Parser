#include <stdio.h>
#include <stdlib.h>

#define path "C:\\Windows\\notepad.exe"


int bringDosHeader(unsigned char binary[]){

    printf("이 파일의 e_magic = %X%X = %c%c\n", binary[0], binary[1], binary[0], binary[1]);
    printf("이 파일의 e_lfanew = ");
    for (int i = 57; i <= 60 ; i++) {
        printf("%02X", binary[i]);
    }

    return 0;
}

int main(int argc, char *argv[]){ //나중에는 파일 경로를 argument를 통해 받는걸로 변경.
    printf("파일의 경로 : %s\n", path);
    FILE* fp = NULL;
    int size = 0;
    unsigned char *binary_buf = NULL; //이진수를 넣어야해서 unsigned char로 선언해야함

    fp = fopen(path, "rb"); //argv[1]를 통해 받는걸로 구현
    if (fp == NULL){
        printf("File Open error\n");
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    binary_buf = malloc(size+1);
    fread(binary_buf, 1, size, fp);
    fclose(fp);

    bringDosHeader(binary_buf);


    return 0;
}
