#include <stdio.h>
#include <stdlib.h>

//#define path "C:\\Windows\\notepad.exe"


int bringDosHeader(unsigned char binary[]){

    if(binary[0] != 'M' || binary[1] != 'Z'){ //PE파일 아닌건 예외처리
        printf("이 파일은 PE포맷이 아닙니다.");
        return 0;
    }

    printf("이 파일의 e_magic = %X%X = %c%c\n", binary[0], binary[1], binary[0], binary[1]);
    printf("이 파일의 e_lfanew = ");
    for (int i = 57; i <= 60 ; i++) {
        printf("%02X", binary[i]);
    }

    printf("\n\n");
    return 1;
}

int main(int argc, char *argv[]){
    char *filename =  malloc(100);;  //그냥 선언만 하면 문자열 포인터는 읽기만 가능하기 때문에 scanf를 사용할 수 없음. malloc으로 공간을 만들어주어야 scanf 가능

    if(argc < 2){ //경로를 직접 입력 받거나, argument로 받거나.
        printf("파일의 경로를 입력해주세요 :");
        scanf("%s", filename);
        printf("입력받은 파일의 경로 : %s\n", filename);
    }
    else{
        filename = argv[1];
        printf("입력받은 파일의 경로 : %s\n", filename);
    }


    FILE* fp = NULL;
    int size = 0;
    unsigned char *binary_buf = NULL; //이진수를 넣어야해서 unsigned char로 선언해야함

    fp = fopen(filename, "rb"); //argv[1]를 통해 받는걸로 구현
    if (fp == NULL){
        printf("이 파일은 열 수 없습니다.\n");
        return 0;
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    binary_buf = malloc(size+1);
    fread(binary_buf, 1, size, fp);
    fclose(fp);



    bringDosHeader(binary_buf);


    system("pause");
    return 0;
}
