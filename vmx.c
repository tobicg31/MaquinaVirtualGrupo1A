#include <stdio.h>
#include <string.h>
#include <string.h>
#include "operaciones.h"

int validarDatos(FILE * arch, short int *tamanoCodigo);
void inicializarTabla(short int tamCodigo, short int Tabla[][2]);
void Ejecucion(int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], char* nomRegistro[32]);

void main(int argc, char *argv[]){
    int flag;

    char memoria[MEMORIA]; //vector de 1 byte
    short int tabla[8][2]; //matriz de 2 bytes * 8 bytes para tabla de segmentos
    int registros[REGISTROS] = {0}; //podriamos meter todas las bases q tenemos en un mismo void inicializadores

    char* nomRegistro[32] = {"IP", "OPC", "OP1","OP2", "LAR", "MAR", "MBR", "nada", "nada", "nada", "EAX", "EBX", "ECX", "EDX", "EEX", "EFX", "AC", "CC", "nada", "nada", "nada", "nada", "nada", "nada", "nada", "nada", "CS", "DS", "nada", "nada", "nada", "nada"};

    if (argc >= 3)
        flag = strcmp(argv[2],"-d")==0;//argv[2]=="-d"; soy un boludo por dios
    else
        flag = 0;

    printf("nombre archivo:%s \n dissassembler:%d \n", argv[1], flag);

    FILE * arch = fopen(argv[1], "rb");

    char dato;
    int tamanioArchivo=0;
    short int tamCodigo;

    int validar;
    validar = validarDatos(arch, &tamCodigo); //ya me queda el puntero actualizado ?????????
    if (validar){
        inicializarTabla(tamCodigo, tabla);
        int j=0;

        while (j<tamCodigo){
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

int validarDatos(FILE *arch, short int *tamanioCodigo){
    char dato;
    char version;
    char datos[6];

    for (int i = 0; i < 5; i++){
        fread(&dato, sizeof(dato), 1, arch);
        datos[i] = dato;
    }
    datos[5] = '\0'; // terminador para que strcmp sea seguro

    printf("%s \n", datos);

    if (strcmp(datos, "VMX26") == 0){
        fread(&version, sizeof(version), 1, arch);
        if (version == 1){
            unsigned char byteAlto, byteBajo;
            fread(&byteAlto, 1, 1, arch);
            fread(&byteBajo, 1, 1, arch);
            *tamanioCodigo = (short int)((byteAlto << 8) | byteBajo); // antes: sizeof(tamanioCodigo)
            return 1;
        }
    }

    *tamanioCodigo = -1;
    return 0;
}

void inicializarTabla(short int tamCodigo, short int Tabla[][2]){
    Tabla[0][0] = 0;
    Tabla[0][1] = tamCodigo;
    Tabla[1][0] = tamCodigo;
    Tabla[1][1] = MEMORIA - tamCodigo;
    for (int i = 2; i <= 7; i++){
        Tabla[i][0] = -1;
        Tabla[i][1] = -1;
    }
}
void Ejecucion(int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], char* nomRegistro[32]){
    int errorSig;

    registros[CS] = 0x00000000;
    registros[DS] = 0x00010000;
    registros[IP] = registros[CS];
    void (*Operaciones[32])(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]) = {SYS, JMP, JP, JN, JZ, JC, JV, JNP, JNN, JNZ, NOT, B, C, D, E, STOP, MOV, ADD, SUB, MUL, DIV, CMP, AND, OR, XOR, SWAP, SHL, SHR, SAR, LDL, LDH, RND};

    int IPant;
    
    do{ //<----------------cambiar a un Do-while
        int TopB=0;
        int TopA=0;
        int opA=0, opB=0;
        //memoria[IP] = 50 / 01010000
        //registro[opc] = 10000
        //printf("La ip es:%d \n", registros[IP]);
        registros[OPC] = memoria[(registros[IP])] & 0x1F; // me guardo los 5 bits del codigo de operacio
        errorSig = !((registros[OPC]>=0 && registros[OPC]<=10) || (registros[OPC] >=16 && registros[OPC]<=0x1F) || (registros[OPC]==0x0F));

        if ((memoria[registros[IP]] >> 4)& 1){ //2 operandos
            TopB= (memoria[registros[IP]]>> 6)& 0x03; // si es un operando de mas de 1 byte, como lo guardo
            switch (TopB){
                case 1:
                    opB = (unsigned char) memoria[registros[IP]+1];
                    break;
                case 2:
                    opB = ((unsigned char) memoria[registros[IP]+1] << 8) | (unsigned char) memoria[registros[IP]+2];
                    break;
                case 3:
                    opB = (((unsigned char) memoria[registros[IP]+1] << 8) |(unsigned char) memoria[registros[IP]+2]) << 8 |(unsigned char) memoria[registros[IP]+3];
                    break;
                default:
                    opB = 0;
                    break;
            }
            TopA= (memoria[registros[IP]] >> 4)& 0x03;
            if (TopA == 3)
                opA = (((unsigned char) memoria[registros[IP]+TopB+1] << 8) |
                        (unsigned char) memoria[registros[IP]+TopB+2]) << 8 |
                    (unsigned char) memoria[registros[IP]+TopB+3];
            else
                opA = (unsigned char) memoria[registros[IP]+TopB+1];
            //analizo pesos y tipos funcion aparte
            //reviso que no me caiga del CS registros if (memoria[IP]+tamanoopA+tamanoB es mewnor a tamanocodigo
            // me parece que no hace falta verificar esto)
        }
        else{
            if (((memoria[registros[IP]] >> 5) & 0x07 ) == 0x000){
                TopA=0;
            }
            else{ //1 solo operando
                TopA= (memoria[registros[IP]] >> 6)& 0x03;
                switch (TopA){
                    case 1:
                        opA = (unsigned char) memoria[registros[IP] + 1];
                        break;
                    case 2:
                        opA = ((unsigned char) memoria[registros[IP] + 1] << 8) | (unsigned char) memoria[registros[IP] + 2];
                        break;
                    case 3:
                        opA = (((unsigned char) memoria[registros[IP] + 1] << 8) | (unsigned char) memoria[registros[IP] + 2]) << 8 | (unsigned char) memoria[registros[IP] + 3];
                        break;
                    default:
                        opA = 0;
                        break;
                }
            }
        }

        // aca hay que guardar en registros[OP1] y registros[OP2] los operandos
        // el byte mas significativo va el tipo d operando y en el resto el operando
        registros[OP1] = TopA <<24 | opA;
        registros[OP2] = TopB <<24 | opB;

        IPant = registros[IP];

        registros[IP] += 1+TopB+TopA;

        //printf("operacion:%d tipo de op1:%d tipo de op2:%d\n", registros[OPC], TopA, TopB);
        Operaciones[registros[OPC]](registros[OP1], registros[OP2], flag, memoria, registros, tabla, IPant, nomRegistro);
        printf("el valor del cc es:%x \n", registros[CC]);
        // aca iria la parte de ejecutar la instruccion guardada en registros[OPC]

    }while (!errorSig && registros[IP]!=-1);

}

