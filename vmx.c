#include <stdio.h>
#include <string.h>
#include <string.h>


#define MEMORIA 16834
#define REGISTROS 32
#define IP 0
#define OPC 1
#define OP1 2
#define OP2 3
#define LAR 4
#define MAR 5
#define MBR 7
#define EAX 10
#define EBX 11
#define ECX 12
#define EDX 13
#define EEX 14
#define EFX 15
#define AC 16
#define CC 17
#define CS 26
#define DS 27

int validarDatos(file * arch, short int *tamanoCodigo);
void inicializarTabla(short int tamCodigo, short int Tabla[][8]);
void Ejecucion(char memoria[MEMORIA], short int tabla[][8], char registros[REGISTROS]);

void STOP(int, int, int, char, int, short int);
void JMP(int, int, int, char, int, short int);
void JP(int, int, int, char, int, short int);
void JN(int, int, int, char, int, short int);
void JZ(int, int, int, char, int, short int);
void JC(int, int, int, char, int, short int);
void JV(int, int, int, char, int, short int);
void JNP(int, int, int, char, int, short int);
void JNN(int, int, int, char, int, short int);
void JNZ(int, int, int, char, int, short int);
void NOT(int, int, int, char, int, short int);
void B(int, int, int, char, int, short int);
void C(int, int, int, char, int, short int);
void D(int, int, int, char, int, short int);
void E(int, int, int, char, int, short int);
void STOP(int, int, int, char, int, short int, int);
void MOV(int, int, int, char, int, short int);
void ADD(int, int, int, char, int, short int);
void SUB(int, int, int, char, int, short int);
void MUL(int, int, int, char, int, short int);
void DIV(int, int, int, char, int, short int);
void AND(int, int, int, char, int, short int);
void OR(int, int, int, char, int, short int);
void XOR(int, int, int, char, int, short int);
void SWAP(int, int, int, char, int, short int);
void SHL(int, int, int, char, int, short int);
void SHR(int, int, int, char, int, short int);
void SAR(int, int, int, char, int, short int);
void LDL(int, int, int, char, int, short int);
void LDH(int, int, int, char, int, short int);
void RND(int, int, int, char, int, short int);

void main(int argc, char *argv[]){
    int flag;

    char memoria[MEMORIA]; //vector de 1 byte
    short int tabla[2][8]; //matriz de 2 bytes * 8 bytes para tabla de segmentos
    int registros[REGISTROS]; //podriamos meter todas las bases q tenemos en un mismo void inicializadores
    void (*Operaciones[31])(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant) = {SYS, JMP, JP, JN, JZ, JC, JV, JNP, JNN, JNZ, NOT, B, C, D, E, STOP, MOV, ADD, SUB, MUL, DIV, CMP, AND, OR, XOR, SWAP, SHL, SHR, SAR, LDL, LDH, RND}
    //vector de *funciones

   // function = {mov, add, sub}; f[0](uigiygi)

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
        Ejecucion(memoria, tabla, registros);

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

void Ejecucion(char memoria[MEMORIA], short int tabla[][8], int registros[REGISTROS]){
    int errorSig;

    registros[CS] = 0x00000000;
    registros[DS] = 0x00010000;
    registros[IP] = registros[CS];
    registros[OPC] = (memoria[registro[IP]]) & 0x1F; // me guardo los 5 bits del codigo de operacion

    errorSig = !((registros[OPC]>=0 && registros[OPC]<=10) || (registros[OPC] >=16 && registros[OPC]<=0x1F) || (registros[OPC]==0x0F)); //preguntar
    int TopB=0;
    int TopA=0;
    int opA, opB;

    while (!errorSig && registros[IP]!=-1){ //<----------------cambiar a un Do-while
        //memoria[IP] = 50 / 01010000
        //registro[opc] = 10000

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
            if ((memoria[registros[IP]] >> 5) & 0xFFF  == 0x000)
                //sin operando, stop
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
        registros[OP1] = TopA<<24 | opA;
        registros[OP2] = TopB <<24 | opB;

        int IPant;
        IPant = registros[IP];

        registros[IP] = 1+TopB+TopA;

        Operaciones[registros[OPC]](registros[OP1], registros[OP2], flag, memoria, registros, tabla, IPAnt);

        // aca iria la parte de ejecutar la instruccion guardada en registros[OPC]

        registros[OPC] = memoria[(registro[IP])] & 0x1F; // me guardo los 5 bits del codigo de operacio
        errorSig = !((registros[OPC]>=0 && registros[OPC]<=10) || (registros[OPC] >=16 && registros[OPC]<=0x1F) || (registros[OPC]==0x0F));


    }

}

void STOP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8], int IPant){
    if (flag){
        printf("[%04X] %02X\t | STOP", IPant, memoria[IPant]);
    }
    registros[IP]=-1;
}
void cargarLAR( int op,int registros[REGISTROS], short int tabla[2][8]){
    registros[LAR] = tabla[(registros[op & 0x1F/*DS*/])>>16][0] | (op>>8);
}
int validoDirFisica(int op,int registros[REGISTROS], short int tabla[2][8]){
    return (registros[MAR] & 0xFFFF < tabla[registros[LAR]>>16][1]) && (((registros[MAR]>>16)+registros[MAR] & 0xFFFF) < (registros[op & 0x1F/*DS*/]));
}

void JMP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant){
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
void JP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
void JN(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
void JZ(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
void JC(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
void JV(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
void JNP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
void JNN(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
void JNZ(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
void NOT(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
void B(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
void C(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
void D(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
void E(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
void STOP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
void MOV(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
void ADD(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
void SUB(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
void MUL(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
void DIV(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
void AND(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
void OR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
void XOR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
void SWAP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
void SHL(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
void SHR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
void SAR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
void LDL(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
void LDH(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
void RND(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8]);
