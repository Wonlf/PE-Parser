#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h> //windows.h 파일이 필요함. (2)
#include <WinNT.h> //이걸 사용하려면 (1)


//C:\Windows\notepad.exe 예제

//struct.word & 0xff 이 구문을 하는 이유는 WORD형의 뒷 8비트만 남기게 해서 출력
//(struct.word & 0xff00) >> 8 위와 동일한 원리로 앞 8비트만 남기게 해서 출력하는건데, 8만큼 오른쪽으로 비트연산 하는 이유는 뒤에 비트가 0이 남게 되니까 앞자리로 덮어쓰기


IMAGE_DOS_HEADER dosHeader;
IMAGE_NT_HEADERS ntHeader;



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

    fseek(fp, dosHeader.e_lfanew, SEEK_SET); //파일 포인터의 위치를 dos_header의 끝으로 옮김.
    fread(&ntHeader, sizeof(IMAGE_NT_HEADERS), 1, fp); //dos_header의 끝부터 IMAGE_NT_HEADERS의 크기만큼 읽기.
    bringDosStub(filename); //nt_header를 불러왔을때 실행해야 PE인지 확인 가능
    bringNTheader();


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
    printf("e_magic : %2X %2X (%c%c)\n", dosHeader.e_magic & 0xff, (dosHeader.e_magic & 0xff00) >> 8, dosHeader.e_magic & 0xff, (dosHeader.e_magic & 0xff00) >> 8);
    printf("e_lfanew : 0x%08X\n",dosHeader.e_lfanew);
    printf("\n");
}

void bringDosStub(char filename[]) {
    FILE* fp2 = NULL;
    fp2 = fopen(filename, "rb"); //argv[1]를 통해 받는걸로 구현
    int size = 0;
    unsigned char *binary = NULL; //unsigned char를 사용하는 이유는 바이너리를 받기 위해

    fseek(fp2, 0, SEEK_END);
    size = ftell(fp2);
    fseek(fp2, 0, SEEK_SET);

    binary = malloc(size+1);
    fread(binary, 1, size, fp2);
    fclose(fp2);


    int count = 0;
    int line_count = 34;
    printf("이 파일의 DosStub ↓\n");
    for (int i = 0; i < line_count + 3; ++i) {
        printf("-");
    }
    printf("\n");
    for (int i = 60; i; i++) {
        if (binary[i] == 'P' && binary[i + 1] == 'E') {
            break;
        } else {
            if (binary[i] >= 0x20 && binary[i] <= 0x7E) { // 특수 문자 아니면 출력
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
                printf("."); // 특수문자, 그래픽문자 등은 마침표로 출력
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
            printf(" = (IMAGE_OPTIONAL_HEADER32) : 32비트\n");
            break;
        case 0x20b:
            printf(" = (IMAGE_OPTIONAL_HEADER64) : 64비트\n");
            break;
    }
    printf("Address of Entry Point: 0x%x\n", ntHeader.OptionalHeader.AddressOfEntryPoint); //프로그램 Entry Point의 RVA
    printf("Image Base: 0x%x\n", ntHeader.OptionalHeader.ImageBase); //메모리의 로딩되는 시작 주소, 실제 EP의 VA(실제 주소)는 AddressOfEntryPoint+ImageBase
    printf("Section Alignment: 0x%x\n", ntHeader.OptionalHeader.SectionAlignment); //메모리에서 섹션의 최소 단위
    printf("File Alignment: 0x%x\n", ntHeader.OptionalHeader.FileAlignment); //파일에서 섹션의 최소 단위
    printf("Size of Image: 0x%x\n", ntHeader.OptionalHeader.SizeOfImage); //메모리에 올라 갔을 때 PE 구조의 크기
    printf("Size of Headers: 0x%x\n", ntHeader.OptionalHeader.SizeOfHeaders); //메모리의 올라 갔을 때 PE 헤더의 크기
    printf("Subsystem: 0x%x", ntHeader.OptionalHeader.Subsystem); //값 별로 파일의 유형을 알 수 있음.
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
    printf("Number of RVA sizes: 0x%x\n", ntHeader.OptionalHeader.NumberOfRvaAndSizes); //마지막 멤버 DataDirectory 배열의 개수
    printf("\n\n");
}

void bringNTheader(){
    printf("이 파일의 NT HEADER 주요 멤버\n");
    printf("Signature: %x %x (%c%c)\n", ntHeader.Signature & 0xff, (ntHeader.Signature & 0xff00) >> 8, ntHeader.Signature & 0xff, (ntHeader.Signature & 0xff00) >> 8);
    printf("MACHINE: 0x%x",ntHeader.FileHeader.Machine); //사용중인 CPU의 특정 값이 들어있음
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

        //case가 더 많지만 사용하는 비중이 큰 MACHINE만 출력
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
    printf("Number of Sections: 0x%x = (전체 섹션의 개수) : %x개\n", ntHeader.FileHeader.NumberOfSections, ntHeader.FileHeader.NumberOfSections); //파일의 있는 섹션의 전체 갯수
    printf("Size of OptionalHeader: 0x%x\n",ntHeader.FileHeader.SizeOfOptionalHeader); //NT header의 IMAGE_OPTIONAL_HEADER(n) 의 크기
    printf("File Characteristics: 0x%x\n", ntHeader.FileHeader.Characteristics); //파일의 속성을 나타내는 특정 수의 조합이기 때문의 경우의 수가 있어서 case로 출력은 불가능
    printf("Time Data Stamp: 0x%x = (프로그램의 빌드 시간) : %d초\n", ntHeader.FileHeader.TimeDateStamp, ntHeader.FileHeader.TimeDateStamp); //실행 파일의 빌드 시간

    bringOptionalHeader();
}