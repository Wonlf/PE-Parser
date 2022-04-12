#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h> //windows.h ������ �ʿ���. (2)
#include <WinNT.h> //�̰� ����Ϸ��� (1)


//C:\Windows\notepad.exe ����

//struct.word & 0xff �� ������ �ϴ� ������ WORD���� �� 8��Ʈ�� ����� �ؼ� ���
//(struct.word & 0xff00) >> 8 ���� ������ ������ �� 8��Ʈ�� ����� �ؼ� ����ϴ°ǵ�, 8��ŭ ���������� ��Ʈ���� �ϴ� ������ �ڿ� ��Ʈ�� 0�� ���� �Ǵϱ� ���ڸ��� �����


IMAGE_DOS_HEADER dosHeader;
IMAGE_NT_HEADERS ntHeader;



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

    fseek(fp, dosHeader.e_lfanew, SEEK_SET); //���� �������� ��ġ�� dos_header�� ������ �ű�.
    fread(&ntHeader, sizeof(IMAGE_NT_HEADERS), 1, fp); //dos_header�� ������ IMAGE_NT_HEADERS�� ũ�⸸ŭ �б�.
    bringDosStub(filename); //nt_header�� �ҷ������� �����ؾ� PE���� Ȯ�� ����
    bringNTheader();


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
    printf("e_magic : %2X %2X (%c%c)\n", dosHeader.e_magic & 0xff, (dosHeader.e_magic & 0xff00) >> 8, dosHeader.e_magic & 0xff, (dosHeader.e_magic & 0xff00) >> 8);
    printf("e_lfanew : 0x%08X\n",dosHeader.e_lfanew);
    printf("\n");
}

void bringDosStub(char filename[]) {
    FILE* fp2 = NULL;
    fp2 = fopen(filename, "rb"); //argv[1]�� ���� �޴°ɷ� ����
    int size = 0;
    unsigned char *binary = NULL; //unsigned char�� ����ϴ� ������ ���̳ʸ��� �ޱ� ����

    fseek(fp2, 0, SEEK_END);
    size = ftell(fp2);
    fseek(fp2, 0, SEEK_SET);

    binary = malloc(size+1);
    fread(binary, 1, size, fp2);
    fclose(fp2);


    int count = 0;
    int line_count = 34;
    printf("�� ������ DosStub ��\n");
    for (int i = 0; i < line_count + 3; ++i) {
        printf("-");
    }
    printf("\n");
    for (int i = 60; i; i++) {
        if (binary[i] == 'P' && binary[i + 1] == 'E') {
            break;
        } else {
            if (binary[i] >= 0x20 && binary[i] <= 0x7E) { // Ư�� ���� �ƴϸ� ���
                if (count == line_count) {
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
                if (count == line_count) {

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
    for (int i = 0; i < line_count + 3; ++i) {
        printf("-");
    }
    printf("\n\n");
}

void bringOptionalHeader(){
    printf("Optional Magic: 0x%x", ntHeader.OptionalHeader.Magic); //32bit = 0x10B, 64bit = 0x20B
    switch (ntHeader.OptionalHeader.Magic){
        case 0x10b:
            printf(" = (IMAGE_OPTIONAL_HEADER32) : 32��Ʈ\n");
            break;
        case 0x20b:
            printf(" = (IMAGE_OPTIONAL_HEADER64) : 64��Ʈ\n");
            break;
    }
    printf("Address of Entry Point: 0x%x\n", ntHeader.OptionalHeader.AddressOfEntryPoint); //���α׷� Entry Point�� RVA
    printf("Image Base: 0x%x\n", ntHeader.OptionalHeader.ImageBase); //�޸��� �ε��Ǵ� ���� �ּ�, ���� EP�� VA(���� �ּ�)�� AddressOfEntryPoint+ImageBase
    printf("Section Alignment: 0x%x\n", ntHeader.OptionalHeader.SectionAlignment); //�޸𸮿��� ������ �ּ� ����
    printf("File Alignment: 0x%x\n", ntHeader.OptionalHeader.FileAlignment); //���Ͽ��� ������ �ּ� ����
    printf("Size of Image: 0x%x\n", ntHeader.OptionalHeader.SizeOfImage); //�޸𸮿� �ö� ���� �� PE ������ ũ��
    printf("Size of Headers: 0x%x\n", ntHeader.OptionalHeader.SizeOfHeaders); //�޸��� �ö� ���� �� PE ����� ũ��
    printf("Subsystem: 0x%x", ntHeader.OptionalHeader.Subsystem); //�� ���� ������ ������ �� �� ����.
    switch (ntHeader.OptionalHeader.Subsystem){
        case 0x1:
            printf(" (Driver file)\n");
            break;
        case 0x2:
            printf(" (Graphic User Interface file)\n");
            break;
        case 0x3:
            printf(" (Console User Interface file)\n");
            break;
    }
    printf("Number of RVA sizes: 0x%x\n", ntHeader.OptionalHeader.NumberOfRvaAndSizes); //������ ��� DataDirectory �迭�� ����
    printf("\n\n");
}

void bringNTheader(){
    printf("�� ������ NT HEADER �ֿ� ���\n");
    printf("Signature: %x %x (%c%c)\n", ntHeader.Signature & 0xff, (ntHeader.Signature & 0xff00) >> 8, ntHeader.Signature & 0xff, (ntHeader.Signature & 0xff00) >> 8);
    printf("MACHINE: 0x%x",ntHeader.FileHeader.Machine); //������� CPU�� Ư�� ���� �������
    switch(ntHeader.FileHeader.Machine){
        case 0:
            printf("UNKNOWN");
            break;
        case 0x014c:
            printf(" (IA32)\n");
            break;
        case 0x2000:
            printf(" (IA64)\n");
            break;
        case 0x8664:
            printf(" (AMD64)\n");
            break;
        case 0x01c0:
            printf(" (ARM)\n");
            break;
        default:
            printf("Another someting else");

        //case�� �� ������ ����ϴ� ������ ū MACHINE�� ���
        //#define IMAGE_FILE_MACHINE_UNKNOWN           0
        //#define IMAGE_FILE_MACHINE_I386              0x014c  // Intel 386.
        //#define IMAGE_FILE_MACHINE_R3000             0x0162  // MIPS little-endian, 0x160 big-endian
        //#define IMAGE_FILE_MACHINE_R4000             0x0166  // MIPS little-endian
        //#define IMAGE_FILE_MACHINE_R10000            0x0168  // MIPS little-endian
        //#define IMAGE_FILE_MACHINE_WCEMIPSV2         0x0169  // MIPS little-endian WCE v2
        //#define IMAGE_FILE_MACHINE_ALPHA             0x0184  // Alpha_AXP
        //#define IMAGE_FILE_MACHINE_POWERPC           0x01F0  // IBM PowerPC Little-Endian
        //#define IMAGE_FILE_MACHINE_SH3               0x01a2  // SH3 little-endian
        //#define IMAGE_FILE_MACHINE_SH3E              0x01a4  // SH3E little-endian
        //#define IMAGE_FILE_MACHINE_SH4               0x01a6  // SH4 little-endian
        //#define IMAGE_FILE_MACHINE_ARM               0x01c0  // ARM Little-Endian
        //#define IMAGE_FILE_MACHINE_THUMB             0x01c2  // ?
        //#define IMAGE_FILE_MACHINE_IA64              0x0200  // Intel 64
        //#define IMAGE_FILE_MACHINE_MIPS16            0x0266  // MIPS
        //#define IMAGE_FILE_MACHINE_MIPSFPU           0x0366  // MIPS
        //#define IMAGE_FILE_MACHINE_MIPSFPU16         0x0466  // MIPS
        //#define IMAGE_FILE_MACHINE_ALPHA64           0x0284  // ALPHA64
        //#define IMAGE_FILE_MACHINE_AXP64             IMAGE_FILE_MACHINE_ALPHA64
    }
    printf("Number of Sections: 0x%x = (��ü ������ ����) : %x��\n", ntHeader.FileHeader.NumberOfSections, ntHeader.FileHeader.NumberOfSections); //������ �ִ� ������ ��ü ����
    printf("Size of OptionalHeader: 0x%x\n",ntHeader.FileHeader.SizeOfOptionalHeader); //NT header�� IMAGE_OPTIONAL_HEADER(n) �� ũ��
    printf("File Characteristics: 0x%x\n", ntHeader.FileHeader.Characteristics); //������ �Ӽ��� ��Ÿ���� Ư�� ���� �����̱� ������ ����� ���� �־ case�� ����� �Ұ���
    printf("Time Data Stamp: 0x%x = (���α׷��� ���� �ð�) : %d��\n", ntHeader.FileHeader.TimeDateStamp, ntHeader.FileHeader.TimeDateStamp); //���� ������ ���� �ð�

    bringOptionalHeader();
}