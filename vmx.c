#include <stdio.h>
#include <string.h>
#include <string.h>


#define MEMORIA 16834
#define REGISTROS 32
#define IP 0
#define OPC 1
#define CS 26
#define DS 27

int validarDatos(file * arch, short int *tamanoCodigo);
void inicializarTabla(short int tamCodigo, short int Tabla[][8]);
void Ejecucion(char memoria[MEMORIA], short int tabla[][8], char registros[REGISTROS]);

void main(int argc, char *argv[]){
    int flag;

    char memoria[MEMORIA]; //vector de 1 byte
    short int tabla[2][8]; //matriz de 2 bytes * 8 bytes para tabla de segmentos
    char registros[REGISTROS]; //podriamos meter todas las bases q tenemos en un mismo void inicializadores

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



    for (int k =0; k <= tamanio; k++){
        printf("%x \n", memoria[k]);
    }
    printf("%d", tamanio);



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

void Ejecucion(char memoria[MEMORIA], short int tabla[][8], char registros[REGISTROS]){
    int errorSig; int Topc;


    registros[CS] = 0x00;
    registros[DS] = 0x10;
    registros[IP] = registros[CS];
    registros[OPC] = (registro[IP]) & 0x1F; // me guardo los 5 bits del codigo de operacion

    errorSig = !((Topc>=0 && Topc<=10) || (Topc >=16 && Topc<=0x1F) || (Topc==0x0F)); //preguntar
    TopB=0;
    TopA=0;

    while (!errorSig && registros[OPC]!=0x0F){re
        //registro[opc] = 50

        if ((registros[OPC] >> 4)& 1){ //2 operandos
            TopB= ( >> 6)& 0xFF; // si es un operando de mas de 1 byte, como lo guardo
            TopA= ( >> 4)& 0xFF;
            //analizo pesos y tipos funcion aparte
            //reviso que no me caiga del CS registros if (memoria[IP]+tamanoopA+tamanoB es mewnor a tamanocodigo)

            registros[IP]=registros[IP]+tamnototal
        }
        else{
            if ((registros[OPC] >> 5) & 0xFFF  == 0x000)
                //sin operando, stop
            else{
                TopA=
            }
        }
        registros[IP] = 1+TopB+TopA;

        Topc = (registro[IP]) & 0x1F; // me guardo los 5 bits del codigo de operacio
        errorSig = !((Topc>=0 && Topc<=10) || (Topc >=16 && Topc<=0x1F) || (Topc==0x0F));


    }

}
