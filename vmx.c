#include <stdio.h>
#include <string.h>

void main(int argc, char *argv[]){
    int flag;

    if (argc >= 3)
        flag = strcmp(argv[2],"-d")==0;//argv[2]=="-d"; soy un boludo por dios
    else
        flag = 0;

    printf("nombre archivo:%s \n dissassembler:%d", argv[1], flag);
    file * arch = fopen(argv[1],"rb");

}

