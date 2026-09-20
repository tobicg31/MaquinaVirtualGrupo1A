#include "operaciones.h"

void STOP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8], int IPant, char* nomRegistro[32]){
    if (flag){
        printf("[%04X] %02X\t | STOP", IPant, memoria[IPant]);
    }
    registros[IP]=-1;
}

void JMP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    if(op1>>24==3){ //operando de memoria
        //cargo en el LAR la direccion logica, reviso el cod de segmento:  
        //registros[DS] = 00 01 00 00
        if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
            cargarLAR(op1, registros, tabla); //guardo la dir logica

            //en la parte del MAR cuantos bytes:
            registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
            //traducir a dir fisica, guardarla en la parte baja del MBR, reviso si no se cae del segmento:
            registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
            if( validoDirFisica(op1, registros, tabla) ){
                //guardar en el MBR el valor:
                registros[MBR] = memoria[registros[MAR] & 0xFFFF];
            }
            else
                printf("FALLO DE SEGMENTO");   
            registros[IP] = registros[MBR]; //hago el salto
            if (flag){
                printf("[%04X]", IPant);
                for (int i = IPant; i < registros[IP]; i++){
                    printf("%02X", memoria[i]);
                }
                printf("\t | JMP [%d]", registros[MBR]);
                
            }
                
        }
        else
            printf("FALLO DE SEGMENTO");
    }
    else{// operando de registro
        registros[IP] = registros[op1 & 0x1F];
        if (flag){
                printf("[%04X]", IPant);
                for (int i = IPant; i < registros[IP]; i++){
                    printf("%02X", memoria[i]);
                }
                printf("\t | JMP [%s]", nomRegistro[op1 & 0x1F]); //podria haber un vector con los nombres d los registros
            }
    }
}
void JP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    if (registros[CC]>>30 == 0){ // N y Z son 0
        JMP(op1, op2, 0, memoria, registros, tabla, IPant, nomRegistro);
        if (flag){
            printf("[%04X]", IPant);
            for (int i = IPant; i < registros[IP]; i++){
                printf("%02X", memoria[i]);
            }
            if (op1 >> 24 == 3)
                printf("\t | JP [%d]", registros[MBR]);
            else
                printf("\t | JP [%s]", nomRegistro[op1 & 0x1F]);
        }
    }
}
void JN(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    if (registros[CC]>>30 == 0b10){ // N==1 Z==0
        JMP(op1, op2, 0, memoria, registros, tabla, IPant, nomRegistro);
        if (flag){
            printf("[%04X]", IPant);
            for (int i = IPant; i < registros[IP]; i++){
                printf("%02X", memoria[i]);
            }
            if (op1 >> 24 == 3)
                printf("\t | JN [%d]", registros[MBR]);
            else
                printf("\t | JN [%s]", nomRegistro[op1 & 0x1F]);
        }
    }
}
void JZ(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    if (registros[CC]>>30 == 0b01){ // N==0 Z==1
        JMP(op1, op2, 0, memoria, registros, tabla, IPant, nomRegistro);
        if (flag){
            printf("[%04X]", IPant);
            for (int i = IPant; i < registros[IP]; i++){
                printf("%02X", memoria[i]);
            }
            if (op1 >> 24 == 3)
                printf("\t | JZ [%d]", registros[MBR]);
            else
                printf("\t | JZ [%s]", nomRegistro[op1 & 0x1F]);
        }
    }
}
void JC(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    if (registros[CC]>>29 & 1 == 1){ // C==1
        JMP(op1, op2, 0, memoria, registros, tabla, IPant, nomRegistro);
        if (flag){
            printf("[%04X]", IPant);
            for (int i = IPant; i < registros[IP]; i++){
                printf("%02X", memoria[i]);
            }
            if (op1 >> 24 == 3)
                printf("\t | JC [%d]", registros[MBR]);
            else
                printf("\t | JC [%s]", nomRegistro[op1 & 0x1F]);
        }
    }
}
void JV(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    if (registros[CC]>>28 & 1 == 1){ // V==1
        JMP(op1, op2, 0, memoria, registros, tabla, IPant, nomRegistro);
        if (flag){
            printf("[%04X]", IPant);
            for (int i = IPant; i < registros[IP]; i++){
                printf("%02X", memoria[i]);
            }
            if (op1 >> 24 == 3)
                printf("\t | JV [%d]", registros[MBR]);
            else
                printf("\t | JV [%s]", nomRegistro[op1 & 0x1F]);
        }
    }
}
void JNP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    if (registros[CC]>>30 == 0b10 || registros[CC]>>30 == 0b01 ){ // N o Z == 1
        JMP(op1, op2, 0, memoria, registros, tabla, IPant, nomRegistro);
        if (flag){
            printf("[%04X]", IPant);
            for (int i = IPant; i < registros[IP]; i++){
                printf("%02X", memoria[i]);
            }
            if (op1 >> 24 == 3)
                printf("\t | JNP [%d]", registros[MBR]);
            else
                printf("\t | JNP [%s]", nomRegistro[op1 & 0x1F]);
        }
    }
}
void JNN(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    if (registros[CC]>>31 == 0 ){ // N == 0
        JMP(op1, op2, 0, memoria, registros, tabla, IPant, nomRegistro);
        if (flag){
            printf("[%04X]", IPant);
            for (int i = IPant; i < registros[IP]; i++){
                printf("%02X", memoria[i]);
            }
            if (op1 >> 24 == 3)
                printf("\t | JNP [%d]", registros[MBR]);
            else
                printf("\t | JNP [%s]", nomRegistro[op1 & 0x1F]);
        }
    }
}
void JNZ(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    if (registros[CC]>>30 & 1 == 0 ){ // Z == 0
        JMP(op1, op2, 0, memoria, registros, tabla, IPant, nomRegistro);
        if (flag){
            printf("[%04X]", IPant);
            for (int i = IPant; i < registros[IP]; i++){
                printf("%02X", memoria[i]);
            }
            if (op1 >> 24 == 3)
                printf("\t | JNP [%d]", registros[MBR]);
            else
                printf("\t | JNP [%s]", nomRegistro[op1 & 0x1F]);
        }
    }
}

void NOT(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    
}
void B(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    
}
void C(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    
}
void D(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    
}
void E(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    
}
void SYS(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    
}
void MOV(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    
}
void ADD(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    
}
void SUB(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    
}
void MUL(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    
}
void DIV(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    
}
void AND(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    
}
void OR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    
}
void XOR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    
}
void SWAP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    
}
void SHL(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    
}
void SHR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    
}
void SAR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    
}
void LDL(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    
}
void LDH(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    
}
void RND(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    
}
