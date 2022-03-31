#include <stdio.h>
#include <stdlib.h>

#define path "C:\\Windows\\notepad.exe"

int main(int argc, char *argv[]){ //나중에는 파일 경로를 argument를 통해 받는걸로 변경.
    FILE* fp = NULL;
    int size = 0;
    char *binary_buf = NULL;

    fp = fopen(path, "rb"); //argv[1]
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


    printf("%c", binary_buf[0]);
    printf("%c", binary_buf[1]);


    return 0;
}
