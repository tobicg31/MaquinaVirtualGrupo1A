#include <stdio.h>
#include <string.h>
#include <string.h>
#include "operaciones.h"

int validarDatos(file * arch, short int *tamanoCodigo);
void inicializarTabla(short int tamCodigo, short int Tabla[][8]);
void Ejecucion(int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8], char* nomRegistro[31]);
void cargarLAR( int op,int registros[REGISTROS], short int tabla[2][8]);
int validoDirFisica(int op,int registros[REGISTROS], short int tabla[2][8]);

void main(int argc, char *argv[]){
    int flag;

    char memoria[MEMORIA]; //vector de 1 byte
    short int tabla[2][8]; //matriz de 2 bytes * 8 bytes para tabla de segmentos
    int registros[REGISTROS] = {0}; //podriamos meter todas las bases q tenemos en un mismo void inicializadores
    void (*Operaciones[31])(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[31]) = {SYS, JMP, JP, JN, JZ, JC, JV, JNP, JNN, JNZ, NOT, B, C, D, E, STOP, MOV, ADD, SUB, MUL, DIV, CMP, AND, OR, XOR, SWAP, SHL, SHR, SAR, LDL, LDH, RND};
    char* nomRegistro[31] = {"IP", "OPC", "OP1","OP2", "LAR", "MAR", "MBR", "nada", "nada", "nada", "EAX", "EBX", "ECX", "EDX", "EEX", "EFX", "AC", "CC", "nada", "nada", "nada", "nada", "nada", "nada", "nada", "nada", "CS", "DS", "nada", "nada", "nada", "nada"};

    if (argc >= 3)
        flag = strcmp(argv[2],"-d")==0//argv[2]=="-d"; soy un boludo por dios
    else
        flag = 0;

    printf("nombre archivo:%s \n dissassembler:%d", argv[1], flag);

    FILE * arch = fopen(argv[1], "rb");

    char dato;
    int tamanioArchivo=0;
    short int tamCodigo;

    int validar;
    validar = validarDatos(arch, &tamCodigo); //ya me queda el puntero actualizado ?????????
    if (validar){
        inicializarTabla(tamCodigo);
        int j=0;

        while (!feof(arch) || j<tamCodigo){
        fread(&dato, sizeof(dato),1,arch);
        memoria[j++] = dato;
        }
        fclose(arch);
        Ejecucion(flag, memoria, registros, tabla, nomRegistro);

    }
    else {
        printf("Error de validacion");
    }
}

int validarDatos(file * arch, short int *tamanioCodigo){ //preguntar
    char dato[5]; int version;

    fread(dato, sizeof(dato), 1, arch);

    if (strncmp(dato,"VMX26",5)==0){
        fread(&version, sizeof(version), 1, arch)
        if (version ==1 ){
            fread(tamanioCodigo, sizeof(tamanioCodigo),1,arch);
            return 1;
        }
    }
    *tamanioCodigo =-1;
    return 0;
}

void inicializarTabla(short int tamCodigo, short int Tabla[][8]){ //inicializar tabla de segmentos
    Tabla[0][0]=0;
    Tabla[0][1]=tamCodigo;
    Tabla[1][0]=tamCodigo;
    Tabla[1][1]=MEMORIA-tamCodigo;
    for (int i=0; i<=1;i++)
        for(int j=2;i<=7;j++)
            Tabla[i][j] = -1;
}

void Ejecucion(int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8], char* nomRegistro[31]){
    int errorSig;

    registros[CS] = 0x00000000;
    registros[DS] = 0x00010000;
    registros[IP] = registros[CS];
    
    int TopB=0;
    int TopA=0;
    int opA, opB;
    int IPant;
    
    do{ //<----------------cambiar a un Do-while
        //memoria[IP] = 50 / 01010000
        //registro[opc] = 10000
        registros[OPC] = memoria[(registro[IP])] & 0x1F; // me guardo los 5 bits del codigo de operacio
        errorSig = !((registros[OPC]>=0 && registros[OPC]<=10) || (registros[OPC] >=16 && registros[OPC]<=0x1F) || (registros[OPC]==0x0F));

        if ((memoria[registros[IP]] >> 4)& 1){ //2 operandos
            TopB= (memoria[IP]>> 6)& 0xFF; // si es un operando de mas de 1 byte, como lo guardo
            switch (TopB){
                case 2:
                    opB = (memoria[IP+1] << 4) | memoria[IP+2];
                    break;
                case 3:
                    opB = ((memoria[IP+1] << 4) | memoria[IP+2]) << 4 | memoria[IP+3];
                    break;
                default:
                    opB = memoria[IP+1];
                    break;
            }
            TopA= (memoria[IP] >> 4)& 0xFF;
            if (TopA == 3) // el tipo de operando de A solo puede ser 1 o 3
                opA = ((memoria[IP+TopB+1] << 4) | memoria[IP+TopB+2]) << 4 | memoria[IP+TopB+3]
            else
                opB = memoria[IP+TopB+1];
            //analizo pesos y tipos funcion aparte
            //reviso que no me caiga del CS registros if (memoria[IP]+tamanoopA+tamanoB es mewnor a tamanocodigo
            // me parece que no hace falta verificar esto)
        }
        else{
            if ((memoria[registros[IP]] >> 5) & 0xFFF  == 0x000){
                TopA=0;
            }
            else{ //1 solo operando
                TopA= (memoria[IP] >> 4)& 0xFF;
                if (TopA == 3) // el operando A solo puede ser 1 o 3
                    opA = ((memoria[IP+TopB+1] << 4) | memoria[IP+TopB+2]) << 4 | memoria[IP+TopB+3]
                else
                    opB = memoria[IP+TopB+1];
            }
        }

        // aca hay que guardar en registros[OP1] y registros[OP2] los operandos
        // el byte mas significativo va el tipo d operando y en el resto el operando
        registros[OP1] = TopA <<24 | opA;
        registros[OP2] = TopB <<24 | opB;

        IPant = registros[IP];

        registros[IP] = 1+TopB+TopA;

        Operaciones[registros[OPC]](registros[OP1], registros[OP2], flag, memoria, registros, tabla, IPAnt, nomRegistro);

        // aca iria la parte de ejecutar la instruccion guardada en registros[OPC]

    }while (!errorSig && registros[IP]!=-1);

}
void cargarLAR( int op,int registros[REGISTROS], short int tabla[2][8]){
    registros[LAR] = tabla[(registros[op & 0x1F/*DS*/])>>16][0] | (op>>8);
}
int validoDirFisica(int op,int registros[REGISTROS], short int tabla[2][8]){
    return (registros[MAR] & 0xFFFF < tabla[registros[LAR]>>16][1]) && (((registros[MAR]>>16)+registros[MAR] & 0xFFFF) < (registros[op & 0x1F/*DS*/]));
}