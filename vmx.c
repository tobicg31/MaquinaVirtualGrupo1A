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

void main(int argc, char *argv[]){
    int flag;

    char memoria[MEMORIA]; //vector de 1 byte
    short int tabla[2][8]; //matriz de 2 bytes * 8 bytes para tabla de segmentos
    int registros[REGISTROS]; //podriamos meter todas las bases q tenemos en un mismo void inicializadores
    //vector de *funciones

   // function = {mov, add, sub}; f[0](uigiygi)

    if (argc >= 3)
        flag = strcmp(argv[2],"-d")==0;//argv[2]=="-d"; soy un boludo por dios
    else
        flag = 0;

    printf("nombre archivo:%s \n dissassembler:%d", argv[1], flag);

    FILE * arch = fopen(argv[1], "rb");

    char dato;
    int tamanioArchivo=0;
    short int tamCodigo;

    validar = validarDatos(arch, &tamCodigo); //ya me queda el puntero actualizado ?????????
    if (validar){
        inicializarTabla(tamCodigo);

        while (!feof(arch) || j<tamCodigo){
        fread(&dato, sizeof(dato),1,arch);
        int j=0;
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

    while (!errorSig && registros[OPC]!=0x0F){ //<----------------cambiar a un Do-while
        //memoria[IP] = 50 / 01010000
        //registro[opc] = 10000

        if ((memoria[registros[IP]] >> 4)& 1){ //2 operandos
            TopB= (memoria[IP]>> 6)& 0xFF; // si es un operando de mas de 1 byte, como lo guardo
            switch (TopB){
                case 2:
                    opB = (memoria[IP+1] << 4) | memoria[IP+2]
                    break;
                case 3:
                    opB = ((memoria[IP+1] << 4) | memoria[IP+2]) << 4 | memoria[IP+3]
                    break;
                default:
                    opB = memoria[IP+1]
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
        // el byte mas significativo va el tipo de operando y en el resto el operando


        registros[IP] = 1+TopB+TopA;

        // aca iria la parte de ejecutar la instruccion guardada en registros[OPC]

        registros[OPC] = memoria[(registro[IP])] & 0x1F; // me guardo los 5 bits del codigo de operacio
        errorSig = !((registros[OPC]>=0 && registros[OPC]<=10) || (registros[OPC] >=16 && registros[OPC]<=0x1F) || (registros[OPC]==0x0F));


    }

}
