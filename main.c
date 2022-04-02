#include <stdio.h>
#include <stdlib.h>

#define path "C:\\Windows\\notepad.exe"


int bringDosHeader(unsigned char binary[]){

    printf("�� ������ e_magic = %X%X = %c%c\n", binary[0], binary[1], binary[0], binary[1]);
    printf("�� ������ e_lfanew = ");
    for (int i = 57; i <= 60 ; i++) {
        printf("%02X", binary[i]);
    }

    return 0;
}

int main(int argc, char *argv[]){ //���߿��� ���� ��θ� argument�� ���� �޴°ɷ� ����.
    printf("������ ��� : %s\n", path);
    FILE* fp = NULL;
    int size = 0;
    unsigned char *binary_buf = NULL; //�������� �־���ؼ� unsigned char�� �����ؾ���

    fp = fopen(path, "rb"); //argv[1]�� ���� �޴°ɷ� ����
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
