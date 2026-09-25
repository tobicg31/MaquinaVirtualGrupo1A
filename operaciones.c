#include "operaciones.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>

void cargarLAR( int op,int registros[REGISTROS], short int tabla[8][2]){
    registros[LAR] = tabla[(registros[op & 0x1F/*DS*/])>>16][0] | ((op>>8)&0xFFFFFF);
}
int validoDirFisica(int op,int registros[REGISTROS], short int tabla[8][2]){
    return (registros[MAR] & 0xFFFF < tabla[registros[LAR]>>16][1]) && (((registros[MAR]>>16)+registros[MAR] & 0xFFFF) < (registros[op & 0x1F/*DS*/]));
}


void disassembler(int flag, int tOp, int op1, int op2, char *nomRegistro[32],int IPant, char memoria[MEMORIA], int registros[MBR], char *funcion){
	if (flag){
        	printf("[%04X]:", IPant);
        	for (int i = IPant; i < registros[IP]; i++){
            		printf("%02X ", (unsigned char)memoria[i]);
        	}
        	printf("\t | %s ", funcion);
            switch(tOp){
                case 0:
                printf(" %02X \tSTOP", (unsigned char)memoria[IPant]);
                break;
                case 1:
                imprimirOperando(tOp,op1, nomRegistro, registros);
                printf("\n");
                break;
                case 2:
                imprimirOperando(tOp,op1, nomRegistro, registros);
                printf(",");
                imprimirOperando(tOp,op2, nomRegistro, registros);
                printf("\n");
                break;
            }
    }
}

void imprimirOperando(int Top,int op, char* nomRegistro[32], int registros[REGISTROS]){
    switch(op>>24){
        case 1:
            printf("%s", nomRegistro[op & 0x1F]);
        break;
        case 2:
            printf("%d", (short int)(op & 0xFFFFFF));
        break;
        case 3:
            printf("[%d]", registros[Top] & 0xFFFF);
        break;
    }
}

void STOP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char* nomRegistro[32]){
    registros[IP]=-1;
    disassembler(flag, 0, 0, 0, nomRegistro, IPant, memoria, registros, "STOP");
}

void JMP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]){
    if (op1 >> 24 == 3){
        if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
            cargarLAR(op1, registros, tabla); //guardo la dir logica
            //en la parte del MAR cuantos bytes:
            registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
            //traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
            registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
            if( validoDirFisica(op1, registros, tabla) ){
                //guardar en el MbR el valor:
                registros[MBR] = memoria[registros[MAR] & 0xFFFF];

                registros[IP] = registros[MBR]; //hago el salto
                disassembler(flag, 1, op1, op2, nomRegistro, IPant, memoria, registros, "JMP");
            }
            else{
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }
        }
        else{
            printf("FALLO DE SEGMENTO");
            registros[IP] = -1;
            return;
        }

    }else if (op1 >> 24 == 1){//registro
        registros[IP] = registros[op1 & 0x1F];
        disassembler(flag, 1, op1, op2, nomRegistro, IPant, memoria, registros, "JMP");

    }else{//segundo op inmediato
        registros[IP] = ((op1 & 0xFFFFFF))&0xFFFF;
        disassembler(flag, 1, op1, op2, nomRegistro, IPant, memoria, registros, "JMP");

    }
}
void JP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]){
    if (registros[CC]>>30 == 0){ // N y Z son 0
                JMP(op1, op2, 0, memoria, registros, tabla, IPant, nomRegistro);
                disassembler(flag, 1, op1, op2, nomRegistro, IPant, memoria, registros, "JP");
    }
}
void JN(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]){
    if (registros[CC]>>30 == 0b10){ // N==1 Z==0
        JMP(op1, op2, 0, memoria, registros, tabla, IPant, nomRegistro);
        disassembler(flag, 1, op1, op2, nomRegistro, IPant, memoria, registros, "JN");
    }
}
void JZ(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]){
    if (registros[CC]>>30 == 0b01){ // N==0 Z==1
        JMP(op1, op2, 0, memoria, registros, tabla, IPant, nomRegistro);
        disassembler(flag, 1, op1, op2, nomRegistro, IPant, memoria, registros, "JZ");
    }
}
void JC(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]){
    if (registros[CC]>>29 & 1 == 1){ // C==1
        JMP(op1, op2, 0, memoria, registros, tabla, IPant, nomRegistro);
        disassembler(flag, 1, op1, op2, nomRegistro, IPant, memoria, registros, "JC");
    }
}
void JV(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]){
    if (registros[CC]>>28 & 1 == 1){ // V==1
        JMP(op1, op2, 0, memoria, registros, tabla, IPant, nomRegistro);
        disassembler(flag, 1, op1, op2, nomRegistro, IPant, memoria, registros, "JV");
    }
}
void JNP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]){
    if (registros[CC]>>30 == 0b10 || registros[CC]>>30 == 0b01 ){ // N o Z == 1
        JMP(op1, op2, 0, memoria, registros, tabla, IPant, nomRegistro);
        disassembler(flag, 1, op1, op2, nomRegistro, IPant, memoria, registros, "JNP");
    }
}
void JNN(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]){
    if (registros[CC]>>31 == 0 ){ // N == 0
        JMP(op1, op2, 0, memoria, registros, tabla, IPant, nomRegistro);
        disassembler(flag, 1, op1, op2, nomRegistro, IPant, memoria, registros, "JNN");
    }
}
void JNZ(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]){
    if ((registros[CC]>>30 & 1) == 0 ){ // Z == 0
        JMP(op1, op2, 0, memoria, registros, tabla, IPant, nomRegistro);
        disassembler(flag, 1, op1, op2, nomRegistro, IPant, memoria, registros, "JNZ");
    }
}

void cambiarCC(int valor, int registros[REGISTROS]){
    registros[CC] = 0;
    if (valor < 0)
        registros[CC] |= 1 << 31;
    else if (valor == 0)
        registros[CC] |= 1 << 30;
}

void NOT(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]){
    if (op1 >> 24 == 3){
        if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
            cargarLAR(op1, registros, tabla); //guardo la dir logica
            //en la parte del MAR cuantos bytes:
            registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
            //traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
            registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
            if( validoDirFisica(op1, registros, tabla) ){
                //guardar en el MbR el valor:
                registros[MBR] = memoria[registros[MAR] & 0xFFFF];
                memoria[registros[MAR] & 0xFFFF]= ~registros[MBR];
                disassembler(flag, 1, op1, op2, nomRegistro, IPant, memoria, registros, "NOT");
            }
            else{
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }
        }
        else{
            printf("FALLO DE SEGMENTO");
            registros[IP] = -1;
            return;
        }

    }else if (op1 >> 24 == 1){//registro
        registros[op1 & 0x1F]= ~registros[op1 & 0x1F];
        disassembler(flag, 1, op1, op2, nomRegistro, IPant, memoria, registros, "NOT");
    }else{//segundo op inmediato
        memoria[tabla[registros[DS]>>16][0]+((op2 & 0xFFFFFF))&0xFFFF]= ~ memoria[tabla[registros[DS]>>16][0]+((op2 & 0xFFFFFF))&0xFFFF] ;
        disassembler(flag, 1, op1, op2, nomRegistro, IPant, memoria, registros, "NOT");
    }
}
void B(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]){
    printf("funcion vacia");

}
void C(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]){
printf("funcion vacia");
}
void D(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]){
printf("funcion vacia  d");
}
void E(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]){
printf("funcion vacia");
}
// Imprime un entero en formato binario con prefijo 0b según la cantidad de bits
void imprimir_binario(int valor, int tam_bytes) {
    int total_bits = tam_bytes * 8;
    printf("0b");
    for (int b = total_bits - 1; b >= 0; b--) {
        printf("%d", (valor >> b) & 1);
    }
}
void SYS(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]){
    int tiposys = op1 & 0xFFFF;

    int tamanio=(registros[ECX]>>16)&0XFFFF;
    int dirlog=registros[EDX];//desde donde parto
    int formato=registros[EAX];
    int offset=dirlog&0XFFFF;
    int segment=(dirlog>>16)&0XFFFF;
    int dirfis=tabla[segment][0];
    int cantidad=registros[ECX]&0XFFFF;/*cantidad de elementos a leer\escribir*/
    int i;
    if (tiposys==0x1){
        for (i=0;i<cantidad;i++){
            int offset_actual = offset + (i * tamanio);
            if(offset_actual+tamanio<=tabla[segment][1]){ //valido dir fisica
               int dirfis_actual= dirfis + offset_actual;
                registros[LAR] = (segment << 16) | (offset_actual & 0xFFFF); //guardo la dir logica
                registros[MAR] = (tamanio << 16)|(dirfis_actual&0xFFFF); //tamaño y direccion fisica de elemento a guardar
                printf("[%04X]: ", dirfis_actual & 0xFFFF);
                int datoleido=0;
                if (formato&0x02){ //caracter, pesa 1 byte
                    char c=getchar();
                    datoleido=(unsigned char) c;
                }
                else{
                    if(formato&0X08){   //HEXA, uso %x
                        scanf("%x",&datoleido);
                    }
                    else{
                        if (formato&0X04){ //OCTAL, uso %o
                            scanf("%o",&datoleido);
                        }
                        else{
                            if (formato&0x10){ //BINARIO, se lee como string y strtol lo pasa a entero base 2 (no sabia como hacerlo esto me dijo chat GPT, preguntar)
                                char buffer[64];
                                scanf("%s", buffer);
                                char *p = buffer;
                                if (p[0] == '0' && (p[1] == 'b' || p[1] == 'B')) p += 2;
                                datoleido = (int)strtol(p, NULL, 2);
                            }
                            else{ //DECIMAL
                                scanf("%d",&datoleido);
                            }
                        }
                    }
                }
                 //guardar en el MBR el valor:
                registros[MBR] = datoleido;
                for (int byte = 0; byte < tamanio; byte++) { //iteracion que guarda el (tipo de dato)valor del dato en memoria
                    memoria[dirfis_actual + byte] = (datoleido >> (8 * (tamanio - 1 - byte))/*Calcula cuántos bits hay que desplazar valor_leido hacia la derecha para bajar el byte deseado*/) & 0xFF /*un byte*/;
                }
            }
            else{
                printf("FALLO DE SEGMENTO fallo sys");
                registros[IP] = -1;
                return;
            }
        }
        
        disassembler(flag, 1, op1, op2, nomRegistro, IPant, memoria, registros, "SYS");
    }
    else{ //WRITE
      for (i=0;i<cantidad;i++){
            int offset_actual = offset + (i * tamanio);
            int dirfis_actual;
             if(offset_actual+tamanio<=tabla[segment][1]){//valido dirfis
                dirfis_actual=dirfis + offset_actual;
                registros[LAR] = (segment << 16) | (offset_actual & 0xFFFF); //guardo la dir logica
                registros[MAR] = (tamanio << 16)|(dirfis_actual&0xFFFF); //tamaño y direccion fisica de elemento a guardar
                int valor = 0;
                for (int byte = 0; byte < tamanio; byte++) {
                    valor = (valor << 8) | (unsigned char)memoria[dirfis_actual + byte]; //unsigned para q no interprete negativos para asi poder unir bits
                }
                registros[MBR] = valor;

                // 1. Imprime dirección física (4 dígitos hexadecimales)
                printf("[%04X]: ", dirfis_actual & 0xFFFF);

                // 2. Imprimir en los formatos activos según bits de EAX, puede ser que me venga mas de un formato (combinado), entonces no usop else
                if (formato & 0x10) { // bit 4: binario
                    imprimir_binario(valor, tamanio);
                    printf(" ");
                }
                if (formato & 0x08) { // bit 3: Hexadecimal
                    // Ajustar ancho de dígitos según el tamaño en bytes, sugerencia de IA, preguntar
                    if (tamanio == 1)
                          printf("0x%02X ", valor & 0xFF);
                    else{
                    if (tamanio == 2)
                    printf("0x%04X ", valor & 0xFFFF);
                    else
                     printf("0x%08X ", valor);
                    }
                }
                if (formato & 0x04) { // bit 2: Octal
                    printf("0o%o ", valor);
                }
                if (formato & 0x02) { // bit 1: Caracteres ASCII
                    for (int b = 0; b < tamanio; b++) {
                        unsigned char c = (unsigned char)memoria[dirfis_actual + b];
                        printf("%c", isprint(c) ? c : '.');
                    }
                    printf(" ");
                }
                if (formato & 0x01) { // bit 0: Decimal
                    printf("%d", valor);
                }

                printf("\n"); //bajo de linea por si tengo q escribir otro

            }
            else{
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }
            
        }
        disassembler(flag, 1, op1, op2, nomRegistro, IPant, memoria, registros, "SYS");
    }
}
void MOV(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char* nomRegistro[32]){
    int valor_fuente = 0;

    // 1. Obtener valor de la fuente (op2)
    if (op2 >> 24 == 3) { // Memoria
        if (tabla[(registros[op2 & 0x1F]) >> 16][0] != -1) {
            cargarLAR(op2, registros, tabla);
            registros[MAR] = 4 << 16;
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op2, registros, tabla)) {
                registros[MBR] = memoria[registros[MAR] & 0xFFFF];
                valor_fuente = registros[MBR];
            } else {
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }
        } else {
            printf("FALLO DE SEGMENTO");
            registros[IP] = -1;
            return;
        }
    } else if (op2 >> 24 == 1) { // Registro
        valor_fuente = registros[op2 & 0x1F];
    } else { // Inmediato
        valor_fuente = op2 & 0xFFFFFF;
    }

    // 2. Guardar en el destino (op1)
    if (op1 >> 24 == 3) { // Memoria
        if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1) {
            cargarLAR(op1, registros, tabla);
            registros[MAR] = 4 << 16;
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op1, registros, tabla)) {
                registros[MBR] = valor_fuente;
                memoria[(registros[LAR] & 0xFFFF)] = valor_fuente; // simplificado a 1 byte o según corresponda
                disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "MOV");
            } else {
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }
        } else {
            printf("FALLO DE SEGMENTO");
            registros[IP] = -1;
            return;
        }
    } else if (op1 >> 24 == 1) { // Registro
        registros[op1 & 0x1F] = valor_fuente;
        disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "MOV");
    }

    cambiarCC(valor_fuente, registros); // MOV afecta al registro CC[cite: 4]
}


void ADD(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char* nomRegistro[32]){

    int valor_fuente = 0;

    // 1. Obtener valor de la fuente (op2)
    if (op2 >> 24 == 3) { // Memoria
        if (tabla[(registros[op2 & 0x1F]) >> 16][0] != -1) {
            cargarLAR(op2, registros, tabla);
            registros[MAR] = 4 << 16;
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op2, registros, tabla)) {
                registros[MBR] = memoria[registros[MAR] & 0xFFFF];
                valor_fuente = registros[MBR];
            } else {
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }
        } else {
            printf("FALLO DE SEGMENTO");
            registros[IP] = -1;
            return;
        }
    } else if (op2 >> 24 == 1) { // Registro
        valor_fuente = registros[op2 & 0x1F];
    } else { // Inmediato
        valor_fuente = op2 & 0xFFFFFF;
    }

    // 2. Guardar en el destino (op1)
    if (op1 >> 24 == 3) { // Memoria
        if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1) {
            cargarLAR(op1, registros, tabla);
            registros[MAR] = 4 << 16;
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op1, registros, tabla)) {
                registros[MBR] = valor_fuente;
                memoria[(registros[LAR] & 0xFFFF)] += valor_fuente; // simplificado a 1 byte o según corresponda
                disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "ADD");
            } else {
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }
        } else {
            printf("FALLO DE SEGMENTO");
            registros[IP] = -1;
            return;
        }
    } else if (op1 >> 24 == 1) { // Registro
        registros[op1 & 0x1F] += valor_fuente;
        disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "ADD");
    }

    cambiarCC(valor_fuente, registros); // MOV afecta al registro CC[cite: 4]
}


void SUB(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char* nomRegistro[32]){
    int valor_fuente = 0;

    // 1. Obtener valor de la fuente (op2)
    if (op2 >> 24 == 3) { // Memoria
        if (tabla[(registros[op2 & 0x1F]) >> 16][0] != -1) {
            cargarLAR(op2, registros, tabla);
            registros[MAR] = 4 << 16;
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op2, registros, tabla)) {
                registros[MBR] = memoria[registros[MAR] & 0xFFFF];
                valor_fuente = registros[MBR];
            } else {
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }
        } else {
            printf("FALLO DE SEGMENTO");
            registros[IP] = -1;
            return;
        }
    } else if (op2 >> 24 == 1) { // Registro
        valor_fuente = registros[op2 & 0x1F];
    } else { // Inmediato
        valor_fuente = op2 & 0xFFFFFF;
    }

    // 2. Guardar en el destino (op1)
    if (op1 >> 24 == 3) { // Memoria
        if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1) {
            cargarLAR(op1, registros, tabla);
            registros[MAR] = 4 << 16;
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op1, registros, tabla)) {
                registros[MBR] = valor_fuente;
                memoria[(registros[LAR] & 0xFFFF)] -= valor_fuente; // simplificado a 1 byte o según corresponda
                disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "SUB");
            } else {
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }
        } else {
            printf("FALLO DE SEGMENTO");
            registros[IP] = -1;
            return;
        }
    } else if (op1 >> 24 == 1) { // Registro
        registros[op1 & 0x1F] -= valor_fuente;
        disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "SUB");
    }

    cambiarCC(valor_fuente, registros);
}

void MUL(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char* nomRegistro[32]){
    int valor_fuente = 0;

    // 1. Obtener valor de la fuente (op2)
    if (op2 >> 24 == 3) { // Memoria
        if (tabla[(registros[op2 & 0x1F]) >> 16][0] != -1) {
            cargarLAR(op2, registros, tabla);
            registros[MAR] = 4 << 16;
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op2, registros, tabla)) {
                registros[MBR] = memoria[registros[MAR] & 0xFFFF];
                valor_fuente = registros[MBR];
            } else {
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }
        } else {
            printf("FALLO DE SEGMENTO");
            registros[IP] = -1;
            return;
        }
    } else if (op2 >> 24 == 1) { // Registro
        valor_fuente = registros[op2 & 0x1F];
    } else { // Inmediato
        valor_fuente = op2 & 0xFFFFFF;
    }

    // 2. Guardar en el destino (op1)
    if (op1 >> 24 == 3) { // Memoria
        if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1) {
            cargarLAR(op1, registros, tabla);
            registros[MAR] = 4 << 16;
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op1, registros, tabla)) {
                registros[MBR] = valor_fuente;
                memoria[(registros[LAR] & 0xFFFF)] *= valor_fuente; // simplificado a 1 byte o según corresponda
                disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "MUL");
            } else {
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }
        } else {
            printf("FALLO DE SEGMENTO");
            registros[IP] = -1;
            return;
        }
    } else if (op1 >> 24 == 1) { // Registro
        registros[op1 & 0x1F] *= valor_fuente;
        disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "MUL");
    }

    cambiarCC(valor_fuente, registros);
}


void DIV(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char* nomRegistro[32]){

    int valor_fuente = 0;

    // 1. Obtener valor de la fuente (op2)
    if (op2 >> 24 == 3) { // Memoria
        if (tabla[(registros[op2 & 0x1F]) >> 16][0] != -1) {
            cargarLAR(op2, registros, tabla);
            registros[MAR] = 4 << 16;
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op2, registros, tabla)) {
                registros[MBR] = memoria[registros[MAR] & 0xFFFF];
                valor_fuente = registros[MBR];
            } else {
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }
        } else {
            printf("FALLO DE SEGMENTO");
            registros[IP] = -1;
            return;
        }
    } else if (op2 >> 24 == 1) { // Registro
        valor_fuente = registros[op2 & 0x1F];
    } else { // Inmediato
        valor_fuente = op2 & 0xFFFFFF;

    if (valor_fuente == 0) {
        printf("Error: División por cero\n");
        registros[IP] = -1;
        return;
    }else {
            // 2. Guardar en el destino (op1)
    if (op1 >> 24 == 3) { // Memoria
        if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1) {
            cargarLAR(op1, registros, tabla);
            registros[MAR] = 4 << 16;
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op1, registros, tabla)) {
                registros[MBR] = valor_fuente;
                memoria[(registros[LAR] & 0xFFFF)] /= valor_fuente;
                registros[AC] %= valor_fuente; // simplificado a 1 byte o según corresponda
                disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "DIV");
            } else {
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }
        } else {
            printf("FALLO DE SEGMENTO");
            registros[IP] = -1;
            return;
        }
    } else if (op1 >> 24 == 1) { // Registro
        registros[op1 & 0x1F] /= valor_fuente;
                disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "DIV");
        registros[AC] %= valor_fuente;
    }

    cambiarCC(valor_fuente, registros);
    }
}
}

void CMP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char* nomRegistro[32]){
    int valor_fuente = 0;

    // 1. Obtener valor de la fuente (op2)
    if (op2 >> 24 == 3) { // Memoria
        if (tabla[(registros[op2 & 0x1F]) >> 16][0] != -1) {
            cargarLAR(op2, registros, tabla);
            registros[MAR] = 4 << 16;
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op2, registros, tabla)) {
                registros[MBR] = memoria[registros[MAR] & 0xFFFF];
                valor_fuente = registros[MBR];
            } else {
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }
        } else {
            printf("FALLO DE SEGMENTO estp");
            registros[IP] = -1;
            return;
        }
    } else if (op2 >> 24 == 1) { // Registro
        valor_fuente = registros[op2 & 0x1F];
    } else { // Inmediato
        valor_fuente = op2 & 0xFFFFFF;
    }

    // 2. Guardar en el destino (op1)
    if (op1 >> 24 == 3) { // Memoria
        if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1) {
            cargarLAR(op1, registros, tabla);
            registros[MAR] = 4 << 16;
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op1, registros, tabla)) {
                registros[MBR] = valor_fuente; // simplificado a 1 byte o según corresponda
                cambiarCC(memoria[(registros[LAR] & 0xFFFF)] - valor_fuente, registros);
                disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "CMP");
            } else {
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }
        } else {
            printf("FALLO DE SEGMENTO");
            registros[IP] = -1;
            return;
        }
    } else if (op1 >> 24 == 1) { // Registro
        cambiarCC(registros[op1 & 0x1F] - valor_fuente, registros);
        disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "CMP");
    }

}

void AND(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]){


    if (op1>>24 == 3){//primer op de memoria
        if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
            cargarLAR(op1, registros, tabla); //guardo la dir logica
            //en la parte del MAR cuantos bytes:
            registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
            //traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
            registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
            if( validoDirFisica(op1, registros, tabla) ){
                //guardar en el MbR el valor:
                registros[MBR] = memoria[registros[MAR] & 0xFFFF];
                if (op2 >> 24 == 3){// segundo op de memoria
                    int valorA = registros[MAR] & 0xFFFF;

                    if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
                        cargarLAR(op2, registros, tabla); //guardo la dir logica
                        //en la parte del MAR cuantos bytes:
                        registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
                        //traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
                        registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
                        if( validoDirFisica(op2, registros, tabla) ){
                            //guardar en el MbR el valor:
                            registros[MBR] = memoria[registros[MAR] & 0xFFFF];
                            memoria[valorA] &= registros[MBR];
                            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "AND");
                        }
                        else{
                            printf("FALLO DE SEGMENTO");
                            registros[IP] = -1;
                            return;
                        }
                    }
                    else{
                        printf("FALLO DE SEGMENTO");
                        registros[IP] = -1;
                        return;
                    }

                }else if (op2 >> 24 == 1){//segundo op de registro
                    memoria[registros[MAR]&0xFFFF] &= registros[op2 & 0x1F];
                disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "AND");
                }else{//segundo op inmediato
                    memoria[registros[MAR]&0xFFFF] &= (op2 & 0xFFFFFF);
                disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "AND");
                }
            }
            else{
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }
        }
        else{
            printf("FALLO DE SEGMENTO");
            registros[IP] = -1;
            return;
        }
        cambiarCC(memoria[tabla[registros[DS]>>16][0]+registros[MBR]], registros);
    }
    else { //primer op de registro
        if (op2 >> 24 == 3){// segundo op de memoria
            if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
                cargarLAR(op2, registros, tabla); //guardo la dir logica
                //en la parte del MAR cuantos bytes:
                registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
                //traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
                registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
                if( validoDirFisica(op2, registros, tabla) ){
                    //guardar en el MbR el valor:
                    registros[MBR] = memoria[registros[MAR] & 0xFFFF];

                    registros[op1 & 0x1F] &= registros[MBR];
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "AND");
                }
                else{
                    printf("FALLO DE SEGMENTO");
                    registros[IP] = -1;
                    return;
                }
            }
            else{
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }

        }else if (op2 >> 24 == 1){//segundo op de registro
            registros[op1 & 0x1F] &= registros[op2 & 0x1F];
            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "AND");
        }else{//segundo op inmediato
            registros[op1 & 0x1F] &= (op2 & 0xFFFFFF);
            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "AND");
        }
        cambiarCC(registros[op1 & 0x1F], registros);
    }
}
void OR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]){

    if (op1>>24 == 3){//primer op de memoria
        if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
            cargarLAR(op1, registros, tabla); //guardo la dir logica
            //en la parte del MAR cuantos bytes:
            registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
            //traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
            registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
            if( validoDirFisica(op1, registros, tabla) ){
                //guardar en el MbR el valor:
                registros[MBR] = memoria[registros[MAR] & 0xFFFF];
                if (op2 >> 24 == 3){// segundo op de memoria
                    int valorA = registros[MAR] & 0xFFFF;

                    if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
                        cargarLAR(op2, registros, tabla); //guardo la dir logica
                        //en la parte del MAR cuantos bytes:
                        registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
                        //traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
                        registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
                        if( validoDirFisica(op2, registros, tabla) ){
                            //guardar en el MbR el valor:
                            registros[MBR] = memoria[registros[MAR] & 0xFFFF];

                            memoria[valorA] |= registros[MBR];
                            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "OR");
                        }
                        else{
                            printf("FALLO DE SEGMENTO");
                            registros[IP] = -1;
                            return;
                        }
                    }
                    else{
                        printf("FALLO DE SEGMENTO");
                        registros[IP] = -1;
                        return;
                    }

                }else if (op2 >> 24 == 1){//segundo op de registro
                    memoria[registros[MAR]&0xFFFF] |= registros[op2 & 0x1F];
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "OR");
                }else{//segundo op inmediato
                    memoria[registros[MAR]&0xFFFF] |= (op2 & 0xFFFFFF);
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "OR");
                }
            }
            else{
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }
        }
        else{
            printf("FALLO DE SEGMENTO");
            registros[IP] = -1;
            return;
        }
        cambiarCC(memoria[tabla[registros[DS]>>16][0]+registros[MBR]], registros);
    }
    else { //primer op de registro
        if (op2 >> 24 == 3){// segundo op de memoria
            if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
                cargarLAR(op2, registros, tabla); //guardo la dir logica
                //en la parte del MAR cuantos bytes:
                registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
                //traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
                registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
                if( validoDirFisica(op2, registros, tabla) ){
                    //guardar en el MbR el valor:
                    registros[MBR] = memoria[registros[MAR] & 0xFFFF];
                    registros[op1 & 0x1F] |= registros[MBR];
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "OR");
                }
                else{
                    printf("FALLO DE SEGMENTO");
                    registros[IP] = -1;
                    return;
                }
            }
            else{
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }

        }else if (op2 >> 24 == 1){//segundo op de registro
            registros[op1 & 0x1F] |= registros[op2 & 0x1F];
            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "OR");
        }else{//segundo op inmediato
            registros[op1 & 0x1F] |= (op2 & 0xFFFFFF);
            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "OR");
        }
        cambiarCC(registros[op1 & 0x1F], registros);
    }
}
void XOR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]){


    if (op1>>24 == 3){//primer op de memoria
        if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
            cargarLAR(op1, registros, tabla); //guardo la dir logica
            //en la parte del MAR cuantos bytes:
            registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
            //traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
            registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
            if( validoDirFisica(op1, registros, tabla) ){
                //guardar en el MbR el valor:
                registros[MBR] = memoria[registros[MAR] & 0xFFFF];
                if (op2 >> 24 == 3){// segundo op de memoria
                    int valorA = registros[MAR] & 0xFFFF;

                    if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
                        cargarLAR(op2, registros, tabla); //guardo la dir logica
                        //en la parte del MAR cuantos bytes:
                        registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
                        //traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
                        registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
                        if( validoDirFisica(op2, registros, tabla) ){
                            //guardar en el MbR el valor:
                            registros[MBR] = memoria[registros[MAR] & 0xFFFF];
                            memoria[valorA] ^= registros[MBR];
                            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "XOR");
                        }
                        else{
                            printf("FALLO DE SEGMENTO");
                            registros[IP] = -1;
                            return;
                        }
                    }
                    else{
                        printf("FALLO DE SEGMENTO");
                        registros[IP] = -1;
                        return;
                    }

                }else if (op2 >> 24 == 1){//segundo op de registro
                    memoria[registros[MAR] & 0xFFFF] ^= registros[op2 & 0x1F];
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "XOR");
                }else{//segundo op inmediato
                    memoria[registros[MAR] & 0xFFFF] ^= (op2 & 0xFFFFFF);
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "XOR");
                }
            }
            else{
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }
        }
        else{
            printf("FALLO DE SEGMENTO");
            registros[IP] = -1;
            return;
        }
        cambiarCC(memoria[tabla[registros[DS]>>16][0]+registros[MBR]], registros);
    }
    else { //primer op de registro
        if (op2 >> 24 == 3){// segundo op de memoria
            if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
                cargarLAR(op2, registros, tabla); //guardo la dir logica
                //en la parte del MAR cuantos bytes:
                registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
                //traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
                registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
                if( validoDirFisica(op2, registros, tabla) ){
                    //guardar en el MbR el valor:
                    registros[MBR] = memoria[registros[MAR] & 0xFFFF];
                    registros[op1 & 0x1F] ^= registros[MBR];
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "XOR");
                }
                else{
                    printf("FALLO DE SEGMENTO");
                    registros[IP] = -1;
                    return;
                }
            }
            else{
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }

        }else if (op2 >> 24 == 1){//segundo op de registro
            registros[op1 & 0x1F] ^= registros[op2 & 0x1F];
             disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "XOR");
        }else{//segundo op inmediato
            registros[op1 & 0x1F] ^= (op2 & 0xFFFFFF);
            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "XOR");
        }
        cambiarCC(registros[op1 & 0x1F], registros);
    }
}
void SWAP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]){


    if (op1>>24 == 3){//primer op de memoria
        if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
            cargarLAR(op1, registros, tabla); //guardo la dir logica
            //en la parte del MAR cuantos bytes:
            registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
            //traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
            registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
            if( validoDirFisica(op1, registros, tabla) ){
                //guardar en el MbR el valor:
                registros[MBR] = memoria[registros[MAR] & 0xFFFF];
                if (op2 >> 24 == 3){// segundo op de memoria
                    int valorA = registros[MAR] & 0xFFFF;

                    if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
                        cargarLAR(op2, registros, tabla); //guardo la dir logica
                        //en la parte del MAR cuantos bytes:
                        registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
                        //traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
                        registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
                        if( validoDirFisica(op2, registros, tabla) ){
                            //guardar en el MbR el valor:
                            registros[MBR] = memoria[registros[MAR] & 0xFFFF];

                            memoria[valorA] ^= registros[MBR];
                            registros[MBR] ^= memoria[valorA];
                            memoria[valorA] ^= registros[MBR];
                            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "SWAP");
                        }
                        else{
                            printf("FALLO DE SEGMENTO");
                            registros[IP] = -1;
                            return;
                        }
                    }
                    else{
                        printf("FALLO DE SEGMENTO");
                        registros[IP] = -1;
                        return;
                    }

                }else if (op2 >> 24 == 1){//segundo op de registro
                    memoria[registros[MAR] & 0xFFFF] ^= registros[op2 & 0x1F];
                    registros[op2 & 0x1F] ^= memoria[registros[MAR] & 0xFFFF];
                    memoria[registros[MAR] & 0xFFFF] ^= registros[op2 & 0x1F];
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "SWAP");
                }else{//segundo op inmediato
                    int opb = (op2 & 0xFFFFFF);

                    memoria[registros[MAR] & 0xFFFF] ^= opb;
                    opb  ^= memoria[registros[MAR] & 0xFFFF];
                    memoria[registros[MAR] & 0xFFFF] ^= opb;
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "SWAP");
                }
            }
            else{
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }
        }
        else{
            printf("FALLO DE SEGMENTO");
            registros[IP] = -1;
            return;
        }
        cambiarCC(memoria[tabla[registros[DS]>>16][0]+registros[MBR]], registros);
    }
    else { //primer op de registro
        if (op2 >> 24 == 3){// segundo op de memoria
            if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
                cargarLAR(op2, registros, tabla); //guardo la dir logica
                //en la parte del MAR cuantos bytes:
                registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
                //traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
                registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
                if( validoDirFisica(op2, registros, tabla) ){
                    //guardar en el MbR el valor:
                    registros[MBR] = memoria[registros[MAR] & 0xFFFF];

                    registros[op1 & 0x1F] ^= registros[MBR];
                    registros[MBR] ^=  registros[op1 & 0x1F];
                    registros[op1 & 0x1F] ^= registros[MBR];
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "SWAP");
                }
                else{
                    printf("FALLO DE SEGMENTO");
                    registros[IP] = -1;
                    return;
                }
            }
            else{
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }

        }else if (op2 >> 24 == 1){//segundo op de registro
            registros[op1 & 0x1F] ^= registros[op2 & 0x1F];
            registros[op2 & 0x1F]  ^=registros[op1 & 0x1F];
            registros[op1 & 0x1F] ^= registros[op2 & 0x1F];
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "SWAP");
        }else{//segundo op inmediato
            int opb= (op2 & 0xFFFFFF);
            registros[op1 & 0x1F] ^= opb;
            opb  ^= registros[op1 & 0x1F];
            registros[op1 & 0x1F] ^= opb;
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "SWAP");
        }
        cambiarCC(registros[op1 & 0x1F], registros);
    }
}
void SHL(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]){
    int i,primerbit,direfisopa,direfisopb,valorA;
    int64_t clonconsigno;
    uint64_t clonsinsigno;
    clonconsigno=clonsinsigno=0;

    if (flag){
        printf("[%04X]:", IPant);
        for (int i = IPant; i < registros[IP]; i++){
            printf("%02X", memoria[i]);
        }
        printf("\t SHL ");
    }

    if (op1>>24 == 3){//primer op de memoria
        if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
            cargarLAR(op1, registros, tabla); //guardo la dir logica
            //en la parte del MAR cuantos bytes:
            registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
            //traducir a dir fisica, guardarla en la parte baja del MBR, reviso si no se cae del segmento:
            registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
            if( validoDirFisica(op1, registros, tabla) ){
                //guardar en el MBR el valor:
                registros[MBR] =memoria[registros[MAR] & 0xFFFF];
                direfisopa=registros[MAR] & 0xFFFF;
                if(flag)
                    printf("[%d], ", registros[MBR]);
                if (op2 >> 24 == 3){// segundo op de memoria
                    
                    clonconsigno|=registros[MBR];clonsinsigno|=(unsigned int)registros[MBR];
                    int valorA = registros[MBR];

                    if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
                        cargarLAR(op2, registros, tabla); //guardo la dir logica
                        //en la parte del MAR cuantos bytes:
                        registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
                        //traducir a dir fisica, guardarla en la parte baja del MBR, reviso si no se cae del segmento:
                        registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
                        if( validoDirFisica(op2, registros, tabla) ){
                            //guardar en el MBR el valor:
                            registros[MBR] = memoria[registros[MAR] & 0xFFFF];
                            if (flag)
                                printf("[%d]", registros[MBR]);
                             
                            for (i=1;i<=+registros[MBR];i++){ //en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                             primerbit=memoria[direfisopa]>>31;//agarro el primer bit y me guardo su valor, si es uno y se hace shiftleft habre carreo
                             memoria[direfisopa]<<=1;
                             clonsinsigno<<=1;
                             clonconsigno<<=1;
                
                            }

                            registros[CC]=0; //limpio CC
                            if((clonsinsigno>>32)|0){
                                registros[CC]|=1<<29;
                            }
                            if (clonconsigno!=clonsinsigno){
                             registros[CC]|=1<<28;
                            }

                            registros[CC]|=(memoria[direfisopa]<0)<<31;//NEGATIVO?
                            registros[CC]|=(memoria[direfisopa]==0)<<30;//CERO?
                        }
                        else{
                            printf("FALLO DE SEGMENTO");
                            registros[IP] = -1;
                            return;
                        }
                    }
                    else{
                        printf("FALLO DE SEGMENTO");
                        registros[IP] = -1;
                        return;
                    }

                }else if (op2 >> 24 == 1){//segundo op de registro

                     
                            for (i=1;i<=registros[op2&0x1F];i++){ //en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                             primerbit=memoria[tabla[registros[DS]>>16][0]+registros[valorA]]>>31;//agarro el primer bit y me guardo su valor, si es uno y se hace shiftleft habre carreo
                             memoria[direfisopa]<<=1;
                             clonsinsigno<<=1;
                             clonconsigno<<=1;
                            }
                            registros[CC]=0; //limpio CC
                            if((clonsinsigno>>32)|0){
                            registros[CC]|=1<<29;
                            }
                            if (clonconsigno!=clonsinsigno){
                                registros[CC]|=1<<28;
                            }
                            registros[CC]|=(memoria[direfisopa]<0)<<31;//NEGATIVO?
                            registros[CC]|=(memoria[direfisopa]==0)<<30;//CERO?
                    if (flag)
                        printf("%s\n", nomRegistro[op2 & 0x1F]);
                }else{//segundo op inmediato
                     
                            for (i=1;i<=(op2 & 0xFFFFFF);i++){ //en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                             primerbit=memoria[direfisopa]>>31;//agarro el primer bit y me guardo su valor, si es uno y se hace shiftleft habre carreo
                             memoria[direfisopa]<<=1;
                            clonsinsigno<<=1;
                            clonconsigno<<=1;
                             
                            }
                            registros[CC]=0; //limpio CC
                            if((clonsinsigno>>32)|0){
                            registros[CC]|=1<<29;
                            }
                            if (clonconsigno!=clonsinsigno){
                                registros[CC]|=1<<28;
                            }
                            registros[CC]|=(memoria[direfisopa]<0)<<31;//NEGATIVO?
                            registros[CC]|=(memoria[direfisopa]==0)<<30;//CERO?
                    if (flag)
                        printf("%d\n", op2&0xFFFFFF);
                }
            }
            else{
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }
        }
        else{
            printf("FALLO DE SEGMENTO");
            registros[IP] = -1;
            return;
        }
    }
    else { //primer op de registro
        clonconsigno=registros[op1&0X1F];clonsinsigno=(unsigned int)registros[op1&0X1F];
        if (flag)
            printf("%s, ", nomRegistro[op1 & 0x1F]);
        if (op2 >> 24 == 3){// segundo op de memoria
            if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
                cargarLAR(op2, registros, tabla); //guardo la dir logica
                //en la parte del MAR cuantos bytes:
                registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
                //traducir a dir fisica, guardarla en la parte baja del MBR, reviso si no se cae del segmento:
                registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
                if( validoDirFisica(op2, registros, tabla) ){
                    //guardar en el MBR el valor:
                    registros[MBR] = memoria[registros[MAR] & 0xFFFF]; direfisopb=registros[MAR] & 0xFFFF;
                    if (flag)
                        printf("[%d]\n", registros[MBR]);
                    // registros[op1 & 0x1F] (primer operando)
                        
                            for (i=1;i<=registros[MBR];i++){ //en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                             primerbit=registros[op1 & 0x1F]>>31;//agarro el primer bit y me guardo su valor, si es uno y se hace shiftleft habre carreo
                             registros[op1 & 0x1F]<<=1;
                             clonconsigno<<=1;
                             clonsinsigno<<=1;
                            }
                            registros[CC]=0; //limpio CC
                            if((clonsinsigno>>32)|0){
                                registros[CC]|=1<<29;
                            }
                            if (clonconsigno!=clonsinsigno){
                                registros[CC]|=1<<28;
                            }
                            registros[CC]|=(registros[op1 & 0x1F]<0)<<31;//NEGATIVO?
                            registros[CC]|=(registros[op1 & 0x1F]==0)<<30;//CERO?

                }
                else{
                    printf("FALLO DE SEGMENTO");
                    registros[IP] = -1;
                    return;
                }
            }
            else{
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }

        }else if (op2 >> 24 == 1){//segundo op de registro
              
                            for (i=1;i<=registros[op2 & 0x1F];i++){ //en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                             primerbit=registros[op1 & 0x1F]>>31;//agarro el primer bit y me guardo su valor, si es uno y se hace shiftleft habre carreo
                             registros[op1 & 0x1F]<<=1;
                             clonsinsigno<<=1;
                             clonconsigno<<=1;
                            }
                            registros[CC]=0; //limpio CC
                            if((clonsinsigno>>32)|0){
                                registros[CC]|=1<<29;
                            }
                            if (clonconsigno!=clonsinsigno){
                                registros[CC]|=1<<28;
                            }
                            registros[CC]|=(registros[op1 & 0x1F]<0)<<31;//NEGATIVO?
                            registros[CC]|=(registros[op1 & 0x1F]==0)<<30;//CERO?
            if (flag)
                printf("%s\n", nomRegistro[op2 & 0x1F]);
        }else{//segundo op inmediato
             
                            for (i=1;i<=(op2 & 0xFFFFFF);i++){ //en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                             primerbit=registros[op1 & 0x1F]>>31;//agarro el primer bit y me guardo su valor, si es uno y se hace shiftleft habre carreo
                             registros[op1 & 0x1F]<<=1;
                             clonsinsigno<<=1;
                             clonconsigno<<=1;
                            }
                            registros[CC]=0; //limpio CC
                            if((clonsinsigno>>32)|0){
                                registros[CC]|=1<<29;
                            }
                            if (clonconsigno!=clonsinsigno){
                                registros[CC]|=1<<28;
                            }
                            registros[CC]|=(registros[op1 & 0x1F]<0)<<31;//NEGATIVO?
                            registros[CC]|=(registros[op1 & 0x1F]==0)<<30;//CERO?
            if (flag)
                printf("%d\n", op2&0xFFFFFF);
        }
    }
}

void SHR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]){
     int i,direfisopa,direfisopb;
     uint64_t clonsinsigno;
     clonsinsigno=0;//en SHR no hay overflow
    if (flag){
        printf("[%04X]:", IPant);
        for (int i = IPant; i < registros[IP]; i++){
            printf("%02X", memoria[i]);
        }
        printf("\t SHL ");
    }

    if (op1>>24 == 3){//primer op de memoria
        if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
            cargarLAR(op1, registros, tabla); //guardo la dir logica
            //en la parte del MAR cuantos bytes:
            registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
            //traducir a dir fisica, guardarla en la parte baja del MBR, reviso si no se cae del segmento:
            registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
            if( validoDirFisica(op1, registros, tabla) ){
                //guardar en el MBR el valor:
                registros[MBR] = memoria[registros[MAR] & 0xFFFF];
                direfisopa=registros[MAR] & 0xFFFF;
                if(flag)
                    printf("[%d], ", registros[MBR]);
                if (op2 >> 24 == 3){// segundo op de memoria
                    int valorA = registros[MBR];

                    if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
                        cargarLAR(op2, registros, tabla); //guardo la dir logica
                        //en la parte del MAR cuantos bytes:
                        registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
                        //traducir a dir fisica, guardarla en la parte baja del MBR, reviso si no se cae del segmento:
                        registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
                        if( validoDirFisica(op2, registros, tabla) ){
                            //guardar en el MBR el valor:
                            registros[MBR] = memoria[registros[MAR] & 0xFFFF];
                            if (flag)
                                printf("[%d]", registros[MBR]);
                             
                            for (i=1;i<=+registros[MBR];i++){ //en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift  
                             memoria[direfisopa]>>=1;
                            clonsinsigno>>=1;
                            }

                            registros[CC]=0; //limpio CC
                            if(( clonsinsigno&0XFFFFFFFF)|0){
                                registros[CC]|=1<<29;
                            }
                            
                            registros[CC]|=(memoria[direfisopa]<0)<<31;//NEGATIVO?
                            registros[CC]|=(memoria[direfisopa]==0)<<30;//CERO?
                        }
                        else{
                            printf("FALLO DE SEGMENTO");
                            registros[IP] = -1;
                            return;
                        }
                    }
                    else{
                        printf("FALLO DE SEGMENTO");
                        registros[IP] = -1;
                        return;
                    }

                }else if (op2 >> 24 == 1){//segundo op de registro

                     
                            for (i=1;i<=registros[op2&0x1F];i++){ //en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                             memoria[direfisopa]>>=1;
                             clonsinsigno>>=1;
                            }
                            registros[CC]=0; //limpio CC
                            if(( clonsinsigno&0XFFFFFFFF)|0){
                                registros[CC]|=1<<29;
                            }
                            registros[CC]|=(memoria[direfisopa]<0)<<31;//NEGATIVO?
                            registros[CC]|=(memoria[direfisopa]==0)<<30;//CERO?
                    if (flag)
                        printf("%s\n", nomRegistro[op2 & 0x1F]);
                }else{//segundo op inmediato
                     
                            for (i=1;i<=(op2 & 0xFFFFFF);i++){ //en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                              
                             memoria[direfisopa]>>=1;
                             clonsinsigno>>=1;
                            }
                            registros[CC]=0; //limpio CC
                            if(( clonsinsigno&0XFFFFFFFF)|0){
                                registros[CC]|=1<<29;
                            }
                            registros[CC]|=(memoria[direfisopa]<0)<<31;//NEGATIVO?
                            registros[CC]|=(memoria[direfisopa]==0)<<30;//CERO?
                    if (flag)
                        printf("%d\n", op2&0xFFFFFF);
                }
            }
            else{
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }
        }
        else{
            printf("FALLO DE SEGMENTO");
            registros[IP] = -1;
            return;
        }
    }
    else { //primer op de registro
        clonsinsigno=registros[op1&0X1F];
        if (flag)
            printf("%s, ", nomRegistro[op1 & 0x1F]);
        if (op2 >> 24 == 3){// segundo op de memoria
            if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
                cargarLAR(op2, registros, tabla); //guardo la dir logica
                //en la parte del MAR cuantos bytes:
                registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
                //traducir a dir fisica, guardarla en la parte baja del MBR, reviso si no se cae del segmento:
                registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
                if( validoDirFisica(op2, registros, tabla) ){
                    //guardar en el MBR el valor:
                    registros[MBR] = memoria[registros[MAR] & 0xFFFF]; direfisopb=registros[MAR] & 0xFFFF;
                    if (flag)
                        printf("[%d]\n", registros[MBR]);
                    // registros[op1 & 0x1F] (primer operando)
                     
                            for (i=1;i<=registros[MBR];i++){ //en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                             
                            registros[op1 & 0x1F]>>=1;
                            clonsinsigno>>=1;
                            }
                            registros[CC]=0; //limpio CC
                            if((clonsinsigno&0XFFFFFFFF)|0){
                            registros[CC]|=1<<29;
                            }
                            registros[CC]|=(registros[op1 & 0x1F]<0)<<31;//NEGATIVO?
                            registros[CC]|=(registros[op1 & 0x1F]==0)<<30;//CERO?

                }
                else{
                    printf("FALLO DE SEGMENTO");
                    registros[IP] = -1;
                    return;
                }
            }
            else{
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }

        }else if (op2 >> 24 == 1){//segundo op de registro
              
                            for (i=1;i<=registros[op2 & 0x1F];i++){ //en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                            registros[op1 & 0x1F]>>=1;
                            clonsinsigno>>=1;
                            }
                            registros[CC]=0; //limpio CC
                            if((clonsinsigno&0XFFFFFFFF)|0){
                                registros[CC]|=1<<29;
                            }
                            registros[CC]|=(registros[op1 & 0x1F]<0)<<31;//NEGATIVO?
                            registros[CC]|=(registros[op1 & 0x1F]==0)<<30;//CERO?
            if (flag)
                printf("%s\n", nomRegistro[op2 & 0x1F]);
        }else{//segundo op inmediato
             
                            for (i=1;i<=(op2 & 0xFFFFFF);i++){ //en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                             registros[op1 & 0x1F]>>=1;
                             clonsinsigno>>=1;

                            }
                            registros[CC]=0; //limpio CC
                            if((clonsinsigno&0XFFFFFFFF)|0){
                                registros[CC]|=1<<29;
                            }
                            registros[CC]|=(registros[op1 & 0x1F]<0)<<31;//NEGATIVO?
                            registros[CC]|=(registros[op1 & 0x1F]==0)<<30;//CERO?
            if (flag)
                printf("%d\n", op2&0xFFFFFF);
        }
    }
}
void SAR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]){
     int i,direfisopa,direfisopb,bitdesigno;
     uint64_t clonsinsigno;
     clonsinsigno=0;//en SHR no hay overflow
    if (flag){
        printf("[%04X]:", IPant);
        for (int i = IPant; i < registros[IP]; i++){
            printf("%02X", memoria[i]);
        }
        printf("\t SHL ");
    }

    if (op1>>24 == 3){//primer op de memoria
        if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
            cargarLAR(op1, registros, tabla); //guardo la dir logica
            //en la parte del MAR cuantos bytes:
            registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
            //traducir a dir fisica, guardarla en la parte baja del MBR, reviso si no se cae del segmento:
            registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
            if( validoDirFisica(op1, registros, tabla) ){
                //guardar en el MBR el valor:
                registros[MBR] = memoria[registros[MAR] & 0xFFFF];
                direfisopa=registros[MAR] & 0xFFFF;
                bitdesigno=memoria[direfisopa]>>31;
                if(flag)
                    printf("[%d], ", registros[MBR]);
                if (op2 >> 24 == 3){// segundo op de memoria
                    int valorA = registros[MBR];

                    if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
                        cargarLAR(op2, registros, tabla); //guardo la dir logica
                        //en la parte del MAR cuantos bytes:
                        registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
                        //traducir a dir fisica, guardarla en la parte baja del MBR, reviso si no se cae del segmento:
                        registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
                        if( validoDirFisica(op2, registros, tabla) ){
                            //guardar en el MBR el valor:
                            registros[MBR] = memoria[registros[MAR] & 0xFFFF];
                            if (flag)
                                printf("[%d]", registros[MBR]);
                             
                            for (i=1;i<=+registros[MBR];i++){ //en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift  
                            memoria[direfisopa]>>=1;memoria[direfisopa]|=bitdesigno<<31;
                            clonsinsigno>>=1;
                            }

                            registros[CC]=0; //limpio CC
                            if(( clonsinsigno&0XFFFFFFFF)|0){
                                registros[CC]|=1<<29;
                            }
                            
                            registros[CC]|=(memoria[direfisopa]<0)<<31;//NEGATIVO?
                            registros[CC]|=(memoria[direfisopa]==0)<<30;//CERO?
                        }
                        else{
                            printf("FALLO DE SEGMENTO");
                            registros[IP] = -1;
                            return;
                        }
                    }
                    else{
                        printf("FALLO DE SEGMENTO");
                        registros[IP] = -1;
                        return;
                    }

                }else if (op2 >> 24 == 1){//segundo op de registro

                     
                            for (i=1;i<=registros[op2&0x1F];i++){ //en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                             memoria[direfisopa]>>=1;memoria[direfisopa]|=bitdesigno<<31;
                             clonsinsigno>>=1;
                            }
                            registros[CC]=0; //limpio CC
                            if(( clonsinsigno&0XFFFFFFFF)|0){
                                registros[CC]|=1<<29;
                            }
                            registros[CC]|=(memoria[direfisopa]<0)<<31;//NEGATIVO?
                            registros[CC]|=(memoria[direfisopa]==0)<<30;//CERO?
                    if (flag)
                        printf("%s\n", nomRegistro[op2 & 0x1F]);
                }else{//segundo op inmediato
                     
                            for (i=1;i<=(op2 & 0xFFFFFF);i++){ //en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                              
                             memoria[direfisopa]>>=1;memoria[direfisopa]|=bitdesigno<<31;
                             clonsinsigno>>=1;
                            }
                            registros[CC]=0; //limpio CC
                            if(( clonsinsigno&0XFFFFFFFF)|0){
                                registros[CC]|=1<<29;
                            }
                            registros[CC]|=(memoria[direfisopa]<0)<<31;//NEGATIVO?
                            registros[CC]|=(memoria[direfisopa]==0)<<30;//CERO?
                    if (flag)
                        printf("%d\n", op2&0xFFFFFF);
                }
            }
            else{
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }
        }
        else{
            printf("FALLO DE SEGMENTO");
            registros[IP] = -1;
            return;
        }
    }
    else { //primer op de registro
        clonsinsigno=registros[op1&0X1F];
        bitdesigno=registros[op1&0X1F]>>31;
        if (flag)
            printf("%s, ", nomRegistro[op1 & 0x1F]);
        if (op2 >> 24 == 3){// segundo op de memoria
            if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
                cargarLAR(op2, registros, tabla); //guardo la dir logica
                //en la parte del MAR cuantos bytes:
                registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
                //traducir a dir fisica, guardarla en la parte baja del MBR, reviso si no se cae del segmento:
                registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
                if( validoDirFisica(op2, registros, tabla) ){
                    //guardar en el MBR el valor:
                    registros[MBR] = memoria[registros[MAR] & 0xFFFF]; direfisopb=registros[MAR] & 0xFFFF;
                    if (flag)
                        printf("[%d]\n", registros[MBR]);
                    // registros[op1 & 0x1F] (primer operando)
                     
                            for (i=1;i<=registros[MBR];i++){ //en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                             
                            registros[op1 & 0x1F]>>=1;registros[op1 & 0x1F]|=bitdesigno<<31;
                            clonsinsigno>>=1;
                            }
                            registros[CC]=0; //limpio CC
                            if((clonsinsigno&0XFFFFFFFF)|0){
                            registros[CC]|=1<<29;
                            }
                            registros[CC]|=(registros[op1 & 0x1F]<0)<<31;//NEGATIVO?
                            registros[CC]|=(registros[op1 & 0x1F]==0)<<30;//CERO?

                }
                else{
                    printf("FALLO DE SEGMENTO");
                    registros[IP] = -1;
                    return;
                }
            }
            else{
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }

        }else if (op2 >> 24 == 1){//segundo op de registro
              
                            for (i=1;i<=registros[op2 & 0x1F];i++){ //en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                            registros[op1 & 0x1F]>>=1;registros[op1 & 0x1F]|=bitdesigno<<31;
                            clonsinsigno>>=1;
                            }
                            registros[CC]=0; //limpio CC
                            if((clonsinsigno&0XFFFFFFFF)|0){
                                registros[CC]|=1<<29;
                            }
                            registros[CC]|=(registros[op1 & 0x1F]<0)<<31;//NEGATIVO?
                            registros[CC]|=(registros[op1 & 0x1F]==0)<<30;//CERO?
            if (flag)
                printf("%s\n", nomRegistro[op2 & 0x1F]);
        }else{//segundo op inmediato
             
                            for (i=1;i<=(op2 & 0xFFFFFF);i++){ //en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                             registros[op1 & 0x1F]>>=1;registros[op1 & 0x1F]|=bitdesigno<<31;
                             clonsinsigno>>=1;

                            }
                            registros[CC]=0; //limpio CC
                            if((clonsinsigno&0XFFFFFFFF)|0){
                                registros[CC]|=1<<29;
                            }
                            registros[CC]|=(registros[op1 & 0x1F]<0)<<31;//NEGATIVO?
                            registros[CC]|=(registros[op1 & 0x1F]==0)<<30;//CERO?
            if (flag)
                printf("%d\n", op2&0xFFFFFF);
        }
    }
}
void LDL(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]){


    if (op1>>24 == 3){//primer op de memoria
        if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
            cargarLAR(op1, registros, tabla); //guardo la dir logica
            //en la parte del MAR cuantos bytes:
            registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
            //traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
            registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
            if( validoDirFisica(op1, registros, tabla) ){
                //guardar en el MbR el valor:
                registros[MBR] = memoria[registros[MAR] & 0xFFFF];
                if (op2 >> 24 == 3){// segundo op de memoria
                    int valorA = registros[MAR] & 0xFFFF;

                    if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
                        cargarLAR(op2, registros, tabla); //guardo la dir logica
                        //en la parte del MAR cuantos bytes:
                        registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
                        //traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
                        registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
                        if( validoDirFisica(op2, registros, tabla) ){
                            //guardar en el MbR el valor:
                            registros[MBR] = memoria[registros[MAR] & 0xFFFF];

                            memoria[valorA] |= (registros[MBR]) & 0xFFFF;
                            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "LDL");
                        }
                        else{
                            printf("FALLO DE SEGMENTO");
                            registros[IP] = -1;
                            return;
                        }
                    }
                    else{
                        printf("FALLO DE SEGMENTO");
                        registros[IP] = -1;
                        return;
                    }

                }else if (op2 >> 24 == 1){//segundo op de registro
                    memoria[registros[MAR] & 0xFFFF] |=( registros[op2 & 0x1F]) & 0xFFFF;
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "LDL");
                }else{//segundo op inmediato
                    memoria[registros[MAR] & 0xFFFF] |= ((op2 & 0xFFFFFF))&0xFFFF;
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "LDL");
                }
            }
            else{
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }
        }
        else{
            printf("FALLO DE SEGMENTO");
            registros[IP] = -1;
            return;
        }
    }
    else { //primer op de registro
        if (op2 >> 24 == 3){// segundo op de memoria
            if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
                cargarLAR(op2, registros, tabla); //guardo la dir logica
                //en la parte del MAR cuantos bytes:
                registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
                //traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
                registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
                if( validoDirFisica(op2, registros, tabla) ){
                    //guardar en el MbR el valor:
                    registros[MBR] = memoria[registros[MAR] & 0xFFFF];
                    registros[op1 & 0x1F] |= registros[MBR]&0xFFFF;
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "LDL");
                }
                else{
                    printf("FALLO DE SEGMENTO");
                    registros[IP] = -1;
                    return;
                }
            }
            else{
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }

        }else if (op2 >> 24 == 1){//segundo op de registro
            registros[op1 & 0x1F] |= (registros[op2 & 0x1F])&0xFFFF;
            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "LDL");
        }else{//segundo op inmediato
            registros[op1 & 0x1F] |= (op2 & 0xFFFFFF)&0xFFFF;
            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "LDL");
        }
    }
}
void LDH(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]){


    if (op1>>24 == 3){//primer op de memoria
        if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
            cargarLAR(op1, registros, tabla); //guardo la dir logica
            //en la parte del MAR cuantos bytes:
            registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
            //traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
            registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
            if( validoDirFisica(op1, registros, tabla) ){
                //guardar en el MbR el valor:
                registros[MBR] = memoria[registros[MAR] & 0xFFFF];
                if (op2 >> 24 == 3){// segundo op de memoria
                    int valorA = registros[MAR] & 0xFFFF;

                    if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
                        cargarLAR(op2, registros, tabla); //guardo la dir logica
                        //en la parte del MAR cuantos bytes:
                        registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
                        //traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
                        registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
                        if( validoDirFisica(op2, registros, tabla) ){
                            //guardar en el MbR el valor:
                            registros[MBR] = memoria[registros[MAR] & 0xFFFF];

                            memoria[valorA] |= ((registros[MBR]) & 0xFFFF)<<16;
                            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "LDH");
                        }
                        else{
                            printf("FALLO DE SEGMENTO");
                            registros[IP] = -1;
                            return;
                        }
                    }
                    else{
                        printf("FALLO DE SEGMENTO");
                        registros[IP] = -1;
                        return;
                    }

                }else if (op2 >> 24 == 1){//segundo op de registro
                    memoria[registros[MAR] & 0xFFFF] |=(( registros[op2 & 0x1F]) & 0xFFFF)<<16;
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "LDH");
                }else{//segundo op inmediato
                    memoria[registros[MAR] & 0xFFFF] |= (((op2 & 0xFFFFFF))&0xFFFF)<<16;
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "LDH");
                }
            }
            else{
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }
        }
        else{
            printf("FALLO DE SEGMENTO");
            registros[IP] = -1;
            return;
        }
    }
    else { //primer op de registro
        if (op2 >> 24 == 3){// segundo op de memoria
            if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
                cargarLAR(op2, registros, tabla); //guardo la dir logica
                //en la parte del MAR cuantos bytes:
                registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
                //traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
                registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
                if( validoDirFisica(op2, registros, tabla) ){
                    //guardar en el MbR el valor:
                    registros[MBR] = memoria[registros[MAR] & 0xFFFF];
                    registros[op1 & 0x1F] |= ((registros[MBR])&0xFFFF)<<16;
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "LDH");
                }
                else{
                    printf("FALLO DE SEGMENTO");
                    registros[IP] = -1;
                    return;
                }
            }
            else{
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }

        }else if (op2 >> 24 == 1){//segundo op de registro
            registros[op1 & 0x1F] |= ((registros[op2 & 0x1F])&0xFFFF)<<16;
            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "LDH");
        }else{//segundo op inmediato
            registros[op1 & 0x1F] |= ((op2 & 0xFFFFFF)&0xFFFF)<<16;
            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "LDH");
        }
    }
}
void RND(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]){

    if (op1>>24 == 3){//primer op de memoria
        if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
            cargarLAR(op1, registros, tabla); //guardo la dir logica
            //en la parte del MAR cuantos bytes:
            registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
            //traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
            registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
            if( validoDirFisica(op1, registros, tabla) ){
                //guardar en el MbR el valor:
                registros[MBR] = memoria[registros[MAR] & 0xFFFF];
                if (op2 >> 24 == 3){// segundo op de memoria
                    int valorA = registros[MAR] & 0xFFFF;

                    if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
                        cargarLAR(op2, registros, tabla); //guardo la dir logica
                        //en la parte del MAR cuantos bytes:
                        registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
                        //traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
                        registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
                        if( validoDirFisica(op2, registros, tabla) ){
                            //guardar en el MbR el valor:
                            registros[MBR] = memoria[registros[MAR] & 0xFFFF];
                            if (flag)
                                printf("[%d]", registros[MBR]);

                            memoria[valorA] = rand() % registros[MBR];
                            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "RND");
                        }
                        else{
                            printf("FALLO DE SEGMENTO");
                            registros[IP] = -1;
                            return;
                        }
                    }
                    else{
                        printf("FALLO DE SEGMENTO");
                        registros[IP] = -1;
                        return;
                    }

                }else if (op2 >> 24 == 1){//segundo op de registro
                    memoria[registros[MAR] & 0xFFFF] = rand() % registros[op2 & 0x1F];
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "RND");
                }else{//segundo op inmediato
                    memoria[registros[MAR] & 0xFFFF] = rand() % (op2 & 0xFFFFFF);
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "RND");
                }
            }
            else{
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }
        }
        else{
            printf("FALLO DE SEGMENTO");
            registros[IP] = -1;
            return;
        }
    }
    else { //primer op de registro
        if (op2 >> 24 == 3){// segundo op de memoria
            if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
                cargarLAR(op2, registros, tabla); //guardo la dir logica
                //en la parte del MAR cuantos bytes:
                registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
                //traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
                registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
                if( validoDirFisica(op2, registros, tabla) ){
                    //guardar en el MbR el valor:
                    registros[MBR] = memoria[registros[MAR] & 0xFFFF];
                    registros[op1 & 0x1F] = rand() % registros[MBR];
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "RND");
                }
                else{
                    printf("FALLO DE SEGMENTO");
                    registros[IP] = -1;
                    return;
                }
            }
            else{
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }

        }else if (op2 >> 24 == 1){//segundo op de registro
            registros[op1 & 0x1F] = rand() % registros[op2 & 0x1F];
            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "RND");
        }else{//segundo op inmediato
            registros[op1 & 0x1F] = rand() % (op2 & 0xFFFFFF);
            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "RND");
        }
    }
}
