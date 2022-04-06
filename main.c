#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h> //windows.h 파일이 필요함. (2)
#include <WinNT.h> //이걸 사용하려면 (1)


//C:\Windows\notepad.exe 예제


IMAGE_DOS_HEADER dosHeader;
IMAGE_DOS_HEADER ntHeader;



void bringDosHeader();
void bringDosStub();
void bringNTheader();



int main(int argc, char *argv[]){
    char *filename =  malloc(100);  //그냥 선언만 하면 문자열 포인터는 읽기만 가능하기 때문에 scanf를 사용할 수 없음. malloc으로 공간을 만들어주어야 scanf 가능

    if(argc < 2){ //경로를 직접 입력 받거나, argument로 받거나.
        printf("파일의 경로를 입력해주세요 :");
        scanf("%s", filename);
        printf("입력받은 파일의 경로 : %s\n", filename);
    }
    else{
        filename = argv[1];
        printf("입력받은 파일의 경로 : %s\n", filename);
    }

    FILE *fp = NULL;
    fp = fopen(filename, "rb"); //argv[1]를 통해 받는걸로 구현
    if (fp == NULL){
        printf("이 파일은 열 수 없습니다.\n");
        return 0;
    }


    rewind(fp); //파일 포인터에 들어있는 주소를 파일의 맨 앞 offset의 주소로 변경한다.
    fread(&dosHeader, sizeof(IMAGE_DOS_HEADER), 1, fp); //파일의 맨 앞부터 IMAGE_DOS_HEADER의 크기만큼 읽기.
    bringDosHeader();
    bringDosStub(filename);


    fclose(fp);
    system("pause");
    return 0;
}

void bringDosHeader(){
    if(dosHeader.e_magic != 0x5A4D){ //PE파일 아닌건 예외처리
        printf("이 파일은 PE포맷이 아닙니다.\n");
        return;
    }
    printf("\n");
    printf("이 파일의 DOS HEADER 주요 멤버\n");
    printf("e_magic : %2X %2X(%c%c)\n", dosHeader.e_magic & 0xff,(dosHeader.e_magic & 0xff00) >> 8, dosHeader.e_magic & 0xff, (dosHeader.e_magic & 0xff00) >> 8);
    printf("e_lfanew : 0x%08X\n",dosHeader.e_lfanew);
    printf("\n");
    //dosHeader.e_magic & 0xff 직접 그려서 확인해보자. WORD형으로 된 e_magic을 양수로 표현하기 위해 같음
    //(dosHeader.e_magic & 0xff00) >> 8
}

void bringDosStub(char filename[]) {
    FILE* fp2 = NULL;
    fp2 = fopen(filename, "rb"); //argv[1]를 통해 받는걸로 구현
    int size = 0;
    unsigned char *binary = NULL; //이진수를 넣어야해서 unsigned char로 선언해야함

    fseek(fp2, 0, SEEK_END);
    size = ftell(fp2);
    fseek(fp2, 0, SEEK_SET);

    binary = malloc(size+1);
    fread(binary, 1, size, fp2);
    fclose(fp2);


    int count = 0;
    printf("이 파일의 DosStub = ↓\n");
    printf("-------------------\n");
    for (int i = 60; i; i++) {
        if (binary[i] == 'P' && binary[i + 1] == 'E') {
            break;
        } else {
            if (binary[i] >= 0x20 && binary[i] <= 0x7E) { // 특수 문자 아니면 출력
                if (count == 16) {
                    count = 0;
                    printf(" |");
                    printf("\n");
                }
                if (count == 0) {
                    count += 1;
                    printf("| ");
                }
                printf("%c", binary[i]);
                count += 1;
            } else {
                if (count == 16) {
                    count = 0;
                    printf(" |");
                    printf("\n");
                }
                if (count == 0) {
                    count += 1;
                    printf("| ");
                }
                printf("."); // 특수문자, 그래픽문자 등은 마침표로 출력
                count += 1;
            }

        }
    }
    printf("|\n");
    printf("-------------------\n");
    printf("\n");
}

void bringNTheader(){
    //do somting
}