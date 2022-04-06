#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h> //windows.h ������ �ʿ���. (2)
#include <WinNT.h> //�̰� ����Ϸ��� (1)


//C:\Windows\notepad.exe ����


IMAGE_DOS_HEADER dosHeader;
IMAGE_DOS_HEADER ntHeader;



void bringDosHeader();
void bringDosStub();
void bringNTheader();



int main(int argc, char *argv[]){
    char *filename =  malloc(100);  //�׳� ���� �ϸ� ���ڿ� �����ʹ� �б⸸ �����ϱ� ������ scanf�� ����� �� ����. malloc���� ������ ������־�� scanf ����

    if(argc < 2){ //��θ� ���� �Է� �ްų�, argument�� �ްų�.
        printf("������ ��θ� �Է����ּ��� :");
        scanf("%s", filename);
        printf("�Է¹��� ������ ��� : %s\n", filename);
    }
    else{
        filename = argv[1];
        printf("�Է¹��� ������ ��� : %s\n", filename);
    }

    FILE *fp = NULL;
    fp = fopen(filename, "rb"); //argv[1]�� ���� �޴°ɷ� ����
    if (fp == NULL){
        printf("�� ������ �� �� �����ϴ�.\n");
        return 0;
    }


    rewind(fp); //���� �����Ϳ� ����ִ� �ּҸ� ������ �� �� offset�� �ּҷ� �����Ѵ�.
    fread(&dosHeader, sizeof(IMAGE_DOS_HEADER), 1, fp); //������ �� �պ��� IMAGE_DOS_HEADER�� ũ�⸸ŭ �б�.
    bringDosHeader();
    bringDosStub(filename);


    fclose(fp);
    system("pause");
    return 0;
}

void bringDosHeader(){
    if(dosHeader.e_magic != 0x5A4D){ //PE���� �ƴѰ� ����ó��
        printf("�� ������ PE������ �ƴմϴ�.\n");
        return;
    }
    printf("\n");
    printf("�� ������ DOS HEADER �ֿ� ���\n");
    printf("e_magic : %2X %2X(%c%c)\n", dosHeader.e_magic & 0xff,(dosHeader.e_magic & 0xff00) >> 8, dosHeader.e_magic & 0xff, (dosHeader.e_magic & 0xff00) >> 8);
    printf("e_lfanew : 0x%08X\n",dosHeader.e_lfanew);
    printf("\n");
    //dosHeader.e_magic & 0xff ���� �׷��� Ȯ���غ���. WORD������ �� e_magic�� ����� ǥ���ϱ� ���� ����
    //(dosHeader.e_magic & 0xff00) >> 8
}

void bringDosStub(char filename[]) {
    FILE* fp2 = NULL;
    fp2 = fopen(filename, "rb"); //argv[1]�� ���� �޴°ɷ� ����
    int size = 0;
    unsigned char *binary = NULL; //�������� �־���ؼ� unsigned char�� �����ؾ���

    fseek(fp2, 0, SEEK_END);
    size = ftell(fp2);
    fseek(fp2, 0, SEEK_SET);

    binary = malloc(size+1);
    fread(binary, 1, size, fp2);
    fclose(fp2);


    int count = 0;
    printf("�� ������ DosStub = ��\n");
    printf("-------------------\n");
    for (int i = 60; i; i++) {
        if (binary[i] == 'P' && binary[i + 1] == 'E') {
            break;
        } else {
            if (binary[i] >= 0x20 && binary[i] <= 0x7E) { // Ư�� ���� �ƴϸ� ���
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
                printf("."); // Ư������, �׷��ȹ��� ���� ��ħǥ�� ���
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