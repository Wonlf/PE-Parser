#include <stdio.h>
#include <stdlib.h>

//C:\Windows\notepad.exe ����
// #define path "C:\\Windows\\notepad.exe"

int e_lfanew;

int bringDosHeader(unsigned char binary[]){
    if(binary[0] != 'M' || binary[1] != 'Z'){ //PE���� �ƴѰ� ����ó��
        printf("�� ������ PE������ �ƴմϴ�.");
        return 0;
    }
    printf("�� ������ e_magic = %X%X = %c%c\n", binary[0], binary[1], binary[0], binary[1]);
    printf("�� ������ e_lfanew = ");
    for (int i = 57; i <= 60 ; i++) {
        printf("%02X", binary[i]);
    }
    e_lfanew = binary[60];
    printf("\n");
    return 1;
}

int bringDosStub(unsigned char binary[]){
    int count = 0;
    printf("�� ������ DosStub = ��\n");
    printf("-------------------\n");
    for (int i = 60; i; i++) {
        if(binary[i] == 'P' && binary[i + 1] == 'E'){
            break;
        }
        else{
            if (binary[i] >= 0x20 && binary[i] <= 0x7E){ // Ư�� ���� �ƴϸ� ���
                if (count == 16){
                    count = 0;
                    printf(" |");
                    printf("\n");
                }
                if(count == 0){
                    count += 1;
                    printf("| ");
                }
                printf("%c", binary[i]);
                count += 1;
            }
            else{
                if (count == 16){
                    count = 0;
                    printf(" |");
                    printf("\n");
                }
                if(count == 0){
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
    return 1;
}


int bringNTheader(unsigned char binary[]){

    printf("�� ������ Signature = ");
    printf("%c%c\n", binary[e_lfanew],binary[e_lfanew + 1]);

    return 1;
}




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


    FILE* fp = NULL;
    int size = 0;
    unsigned char *binary_buf = NULL; //�������� �־���ؼ� unsigned char�� �����ؾ���

    fp = fopen(filename, "rb"); //argv[1]�� ���� �޴°ɷ� ����
    if (fp == NULL){
        printf("�� ������ �� �� �����ϴ�.\n");
        return 0;
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    binary_buf = malloc(size+1);
    fread(binary_buf, 1, size, fp);
    fclose(fp);



    bringDosHeader(binary_buf);

    bringDosStub(binary_buf);

    bringNTheader(binary_buf);

    system("pause");
    return 0;
}
