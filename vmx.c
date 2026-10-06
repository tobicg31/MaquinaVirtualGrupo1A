#include <stdio.h>
#include <string.h>
#include <string.h>
#include <stdlib.h>
#include "operaciones.h"

int validarDatos(FILE * arch, short int tamanioSegmentos[10], char *version);
void inicializarTabla(short int tamCodigo, short int Tabla[][2]);
void Ejecucion(int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], char* nomRegistro[32], int tamCodigo);
void DesensamblarEstatico(char memoria[MEMORIA], short int tamCodigo, char* nomRegistro[32]) {
    int ip = 0;
    int registros_dummy[REGISTROS] = {0}; // Arreglo falso para satisfacer los parámetros
    
    // Mapeo exacto de los Opcodes a String
    char* mnemonicos[32] = {"SYS", "JMP", "JP", "JN", "JZ", "JC", "JV", "JNP", "JNN", "JNZ", 
                            "NOT", "B", "C", "D", "E", "STOP", "MOV", "ADD", "SUB", "MUL", 
                            "DIV", "CMP", "AND", "OR", "XOR", "SWAP", "SHL", "SHR", "SAR", 
                            "LDL", "LDH", "RND"};

    while (ip < tamCodigo) {
        int IPant = ip;
        int opc = memoria[ip] & 0x1F;
        int TopA = 0, TopB = 0;
        int opA = 0, opB = 0;
        int cant_operandos = 0;

        // 1. Decodificar la instrucción
        if ((memoria[ip] >> 4) & 1) { // 2 operandos
            cant_operandos = 2;
            TopB = (memoria[ip] >> 6) & 0x03;
            switch (TopB) {
                case 1: opB = (unsigned char)memoria[ip+1]; break;
                case 2: opB = ((unsigned char)memoria[ip+1] << 8) | (unsigned char)memoria[ip+2]; break;
                case 3: opB = (((unsigned char)memoria[ip+1] << 8) | (unsigned char)memoria[ip+2]) << 8 | (unsigned char)memoria[ip+3]; break;
            }
            TopA = (memoria[ip] >> 4) & 0x03;
            if (TopA == 3)
                opA = (((unsigned char)memoria[ip+TopB+1] << 8) | (unsigned char)memoria[ip+TopB+2]) << 8 | (unsigned char)memoria[ip+TopB+3];
            else
                opA = (unsigned char)memoria[ip+TopB+1];

            ip += 1 + TopB + TopA;
        } else {
            if (((memoria[ip] >> 5) & 0x07) == 0x000) {
                cant_operandos = 0; // Sin operandos
                ip += 1;
            } else { // 1 operando
                cant_operandos = 1;
                TopA = (memoria[ip] >> 6) & 0x03;
                switch (TopA) {
                    case 1: opA = (unsigned char)memoria[ip+1]; break;
                    case 2: opA = ((unsigned char)memoria[ip+1] << 8) | (unsigned char)memoria[ip+2]; break;
                    case 3: opA = (((unsigned char)memoria[ip+1] << 8) | (unsigned char)memoria[ip+2]) << 8 | (unsigned char)memoria[ip+3]; break;
                }
                ip += 1 + TopA;
            }
        }

        // 2. Preparar registros_dummy y armar operandos consolidados
        registros_dummy[IP] = ip; // Le decimos al disassembler hasta qué byte debe imprimir
        
        int op1_completo = (cant_operandos > 0) ? (TopA << 24) | opA : 0;
        int op2_completo = (cant_operandos == 2) ? (TopB << 24) | opB : 0;

        // 3. Llamar a la función existente (forzando flag=1 para que imprima)
        disassembler(1, cant_operandos, op1_completo, op2_completo, nomRegistro, IPant, memoria, registros_dummy, mnemonicos[opc]);
    }
}

void main(int argc, char *argv[]) {
    // 1. Variables para identificar qué parámetros se ingresaron
    char *archivo_vmx = NULL;
    char *archivo_vmi = NULL;
    int flag = 0;
    int tam_memoria_kib = 16; 
    int param_index = -1;

    // 2. Leemos los parámetros evaluando su contenido, no su posición estricta.
    // Esto evita crasheos si el usuario omite el archivo .vmi o el tamaño de memoria.
    for (int i = 1; i < argc; i++) {
        if (strstr(argv[i], ".vmx") != NULL) {
            archivo_vmx = argv[i];
        } else if (strstr(argv[i], ".vmi") != NULL) {
            archivo_vmi = argv[i];
        } else if (strncmp(argv[i], "m=", 2) == 0) {
            tam_memoria_kib = atoi(argv[i] + 2);
        } else if (strcmp(argv[i], "-d") == 0) {
            flag = 1;
        } else if (strcmp(argv[i], "-p") == 0) {
            param_index = i + 1; // Lo que sigue son los parámetros del programa
            break; // Dejamos de buscar flags de la máquina virtual
        }
    }

    // 3. Declaración de memoria de TAMAÑO VARIABLE según m=M
    char memoria[tam_memoria_kib * 1024]; 
    short int tabla[8][2]; 
    int registros[REGISTROS] = {0}; 
    char* nomRegistro[32] = {"IP", "OPC", "OP1","OP2", "LAR", "MAR", "MBR", "SP", "BP", "nada", "EAX", "EBX", "ECX", "EDX", "EEX", "EFX", "AC", "CC", "nada", "nada", "nada", "nada", "nada", "nada", "nada", "nada", "CS", "DS", "ES", "SS", "KS", "PS"};

    short int tamanioSegmentos[10] = {0};
    char version = 0;

    // 4. Lógica principal de carga y ejecución
    if (archivo_vmx != NULL) { 
        FILE *archvmx = fopen(archivo_vmx, "rb");
        if (!archvmx) {
            printf("Error al abrir el archivo %s\n", archivo_vmx);
            return;
        }

        int validar = validarDatos(archvmx, tamanioSegmentos, &version);
        if (validar) {
            if (version == 1) {
                inicializarTabla(tamanioSegmentos[0], tabla);
                int j = 0; char dato;
                while (j < tamanioSegmentos[0] && fread(&dato, 1, 1, archvmx)) {
                    memoria[j++] = dato;
                }
                if (flag) {  
                    DesensamblarEstatico(memoria, tamanioSegmentos[0], nomRegistro);
                }
                    
                Ejecucion(0, memoria, registros, tabla, nomRegistro, tamanioSegmentos[0]);
            }
            else if (version == 2) {
                // 1. Calcular el Param Segment (Siempre existe si ejecutamos un .vmx)
                short int tamanioPS = 0;
                int argcSubrutina = 0;
                
                tamanioPS += strlen(archivo_vmx) + 1; // +1 por el '\0'
                argcSubrutina++;
                
                if (param_index != -1) {
                    for (int i = param_index; i < argc; i++) {
                        tamanioPS += strlen(argv[i]) + 1;
                        argcSubrutina++;
                    }
                }
                tamanioPS += (argcSubrutina * 4); // Arreglo de punteros al final

                // 2. Ordenar los tamaños para la memoria física: PS, KS, CS, DS, ES, SS
                // Según el archivo .vmx, validarDatos los guardó en:
                // tamanioSegmentos[0] = CS
                // tamanioSegmentos[1] = DS
                // tamanioSegmentos[2] = ES
                // tamanioSegmentos[3] = SS
                // tamanioSegmentos[4] = KS
                short int tamOrdenados[6] = {tamanioPS,tamanioSegmentos[4], tamanioSegmentos[0], tamanioSegmentos[1], tamanioSegmentos[2],tamanioSegmentos[3]};

                if (sumaTamanios(tamOrdenados) <= tam_memoria_kib){
                    // 3. Inicializar la tabla y los registros de segmentos
                    inicializarTablaV2(registros, tabla, tamOrdenados);
                    
                    registros[IP] = tabla[CS>>16][0]<<16 | tamanioSegmentos[5];
                    registros[SP] = registros[SS] + tamanioPS; 
                    //cargaMemoria, param, codigo y constantes
                    //ejecucion
                }
                else{
                    printf("no alcanza el tamanio en memoria");
                }
            }
            fclose(archvmx);

        } else {
            printf("Error de validacion\n");
            fclose(archvmx);
        }
    } else if (archivo_vmi != NULL) {
        // Lógica para reanudar desde .vmi
        //cargar desde el vmi
        //ejecucion
        FILE * archvmi = fopen(archivo_vmi, "rb");
        char ident[5]; char verVmi; short int tamMemoria;
        if (strcmp("VMI26", fread(ident, sizeof(ident), 1, archvmi)) == 0){
            if(fread(&verVmi, sizeof(char), 1, archvmi)==1){
                fread(&tamMemoria, sizeof(tamMemoria), 1, archvmi);
                //cargo los registros
                //cargo la tabla
                //cargo la memoria
                //ejecuto al toque
            }
        }
    } else {
        printf("Error: Se requiere archivo .vmx o .vmi\n");
    }
}

int sumaTamanios(short int tamanio[6]){
    int suma=0;
    for (int i = 0; i < 6; i++){
        suma += tamanio[i];
    }
    return suma;
}

void inicializarTablaV2(int registros[REGISTROS], short int tabla[][2], short int tamOrdenados[6]) {
    int dirActual = 0; 
    int indice_tabla = 0;

    // 1. Limpiamos la tabla
    for (int i = 0; i < 8; i++) {
        tabla[i][0] = -1;
        tabla[i][1] = -1;
    }

    // 2. Registros en el orden exacto de carga en memoria física (Parte II)
    int registros_segmento[6] = {PS, KS, CS, DS, ES, SS};

    // 3. Iteramos sobre los 6 segmentos posibles
    for (int i = 0; i < 6; i++) {
        int tam = tamOrdenados[i];
        int reg_id = registros_segmento[i];

        if (tam > 0) {
            tabla[indice_tabla][0] = dirActual;
            tabla[indice_tabla][1] = tam;
            
            registros[reg_id] = indice_tabla << 16;
            
            dirActual += tam;
            indice_tabla++;
        } else {
            registros[reg_id] = -1; 
        }
    }
}
// void main(int argc, char *argv[]){
//     char memoria[MEMORIA]; //vector de 1 byte
//     short int tabla[8][2]; //matriz de 2 bytes * 8 bytes para tabla de segmentos
//     int registros[REGISTROS] = {0}; //podriamos meter todas las bases q tenemos en un mismo void inicializadores
//     char* nomRegistro[32] = {"IP", "OPC", "OP1","OP2", "LAR", "MAR", "MBR", "SP", "BP", "nada", "EAX", "EBX", "ECX", "EDX", "EEX", "EFX", "AC", "CC", "nada", "nada", "nada", "nada", "nada", "nada", "nada", "nada", "CS", "DS", "ES", "SS", "KS", "PS"};
//     FILE * archvmx= fopen(argv[1], "rb");
//     int version; int validar;
//     short int tamanioSegmentos[10];

//     if (strcmp(strrchr(argv[1], '.'), ".vmx") == 0 && strcmp(strrchr(argv[2], '.'), ".vmi") < 0){ //tengo solo vmx
//         validar = validarDatos(archvmx, tamanioSegmentos, &version);
//         if (validar)
//             if (version == 1){
//                 int flag;
//                 if (argc >= 3)
//                     flag = strcmp(argv[2],"-d")==0;//argv[2]=="-d"; soy un boludo por dios
//                 else
//                     flag = 0;
//                 inicializarTabla(tamanioSegmentos[0], tabla);
//                 int j=0; char dato;

//                 while (j<tamanioSegmentos[0]){
//                 fread(&dato, sizeof(dato),1,archvmx);
//                 memoria[j++] = dato;
//                 }
//                 fclose(archvmx);

//                 if(flag){  
//                     DesensamblarEstatico(memoria, tamanioSegmentos[0], nomRegistro);
//                 }
                    
//                 Ejecucion(0, memoria, registros, tabla, nomRegistro, tamanioSegmentos[0]);
//             }
//             else{

//             }
//         else 
//             printf("error de validacion");
//     }
//     else if (strcmp(strrchr(argv[1], '.'), ".vmi") ==0 && strcmp(strrchr(argv[2], '.'), NULL)==0){//tengo solo vmi
        
//     }

// }

// void mainV1(int argc, char *argv[]){
//     int flag;


//     if (argc >= 3)
//         flag = strcmp(argv[2],"-d")==0;//argv[2]=="-d"; soy un boludo por dios
//     else
//         flag = 0;

//     FILE * arch = fopen(argv[1], "rb");

//     char dato;
//     //int tamanioArchivo=0;
//     short int tamCodigo;

//     int validar;
//     validar = validarDatos(arch, &tamCodigo); //ya me queda el puntero actualizado ?????????
//     if (validar){
//         inicializarTabla(tamCodigo, tabla);
//         int j=0;

//         while (j<tamCodigo){
//         fread(&dato, sizeof(dato),1,arch);
//         memoria[j++] = dato;
//         }
//         fclose(arch);

//         if(flag){  
//             DesensamblarEstatico(memoria, tamCodigo, nomRegistro);
//         }
            
//         Ejecucion(0, memoria, registros, tabla, nomRegistro, tamCodigo);

//     }
//     else {
//         printf("Error de validacion");
//     }
// }

int validarDatos(FILE *arch, short int tamanioSegmentos[10], char *version){
    char dato;
    //char version;
    char datos[6];

    for (int i = 0; i < 5; i++){
        fread(&dato, sizeof(dato), 1, arch);
        datos[i] = dato;
    }
    datos[5] = '\0'; // terminador para que strcmp sea seguro

   // printf("%s \n", datos);

    if (strcmp(datos, "VMX26") == 0){
        fread(version, sizeof(char), 1, arch);
        printf("%d", *version);
        if ((*version) == 1){
            unsigned char byteAlto, byteBajo;
            fread(&byteAlto, 1, 1, arch);
            fread(&byteBajo, 1, 1, arch);
            tamanioSegmentos[0] = (short int)((byteAlto << 8) | byteBajo); // antes: sizeof(tamanioCodigo)
            return 1;
        }
        else {
            for (int i=0; i<=5; i++){    
                unsigned char byteAlto, byteBajo;
                fread(&byteAlto, 1, 1, arch);
                fread(&byteBajo, 1, 1, arch);
                tamanioSegmentos[i] = (short int)((byteAlto << 8) | byteBajo);
            }
        }
    }

    tamanioSegmentos[0] = -1;
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
void Ejecucion(int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], char* nomRegistro[32], int tamCodigo){
    int errorSig;

    registros[CS] = 0x00000000;
    registros[DS] = 0x00010000;
    registros[IP] = registros[CS];
    void (*Operaciones[32])(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]) = {SYS, JMP, JP, JN, JZ, JC, JV, JNP, JNN, JNZ, NOT, PUSH, POP, CALL, RET, STOP, MOV, ADD, SUB, MUL, DIV, CMP, AND, OR, XOR, SWAP, SHL, SHR, SAR, LDL, LDH, RND};

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

        if (errorSig) {
            printf("\n[ERROR] Instruccion invalida (%02X) en la direccion IP: [%04X]\n", registros[OPC], registros[IP]);
            registros[IP] = -1; // Marcamos el fin de la ejecución
            break;              // Rompemos el ciclo inmediatamente
        }

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
                TopB=0;
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
        //printf("el valor del cc es:%x \n", registros[CC]);
        //printf("el valor del cc es:");
        //imprimir_binario(registros[CC], 4);
        //printf("\n");
        // aca iria la parte de ejecutar la instruccion guardada en registros[OPC]
        //printf("aca \n");
        

    }while (!errorSig && registros[IP]!=-1 && registros[IP]<tamCodigo);

}

