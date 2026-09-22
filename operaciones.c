#include "operaciones.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


void STOP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8], int IPant, char* nomRegistro[32]){
    if (flag){
        printf("[%04X]: %02X\t | STOP", IPant, memoria[IPant]);
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
            //traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
            registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
            if( validoDirFisica(op1, registros, tabla) ){
                //guardar en el MbR el valor:
                registros[MBR] = memoria[registros[MAR] & 0xFFFF];

                registros[IP] = registros[MBR]; //hago el salto
                if (flag){
                    printf("[%04X]:", IPant);
                    for (int i = IPant; i < registros[IP]; i++){
                        printf("%02X", memoria[i]);
                    }
                    printf("\t | JMP [%d]", registros[MBR]);
                    
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
    else{// operando de registro
        registros[IP] = registros[op1 & 0x1F];
        if (flag){
                printf("[%04X]:", IPant);
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
            printf("[%04X]:", IPant);
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
            printf("[%04X]:", IPant);
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
            printf("[%04X]:", IPant);
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
            printf("[%04X]:", IPant);
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
            printf("[%04X]:", IPant);
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
            printf("[%04X]:", IPant);
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
            printf("[%04X]:", IPant);
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
            printf("[%04X]:", IPant);
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

void cambiarCC(int valor, int registros[REGISTROS]){
    registros[CC] = 0;
    if (valor < 0)
        registros[CC] |= 1 << 31; 
    else if (valor == 0)
        registros[CC] |= 1 << 30;
}

void NOT(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    if(op1>>24==3){ //operando de memoria
        //cargo en el LAR la direccion logica, reviso el cod de segmento:  
        //registros[DS] = 00 01 00 00
        if ( tabla[(registros[op1 & 0x1F])>>16][0] != -1 ){ //pregunto si el codigo de segmento es valido
            cargarLAR(op1, registros, tabla); //guardo la dir logica

            //en la parte del MAR cuantos bytes:
            registros[MAR] = 4 << 16; //creo q son 4 bytes porq leemos numeros(?)
            //traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
            registros[MAR] |= tabla[registros[LAR]>>16][0] + (registros[LAR] & 0xFFFF);
            if( validoDirFisica(op1, registros, tabla) ){
                //guardar en el MbR el valor:
                registros[MBR] = memoria[registros[MAR] & 0xFFFF];
                memoria[tabla[registros[DS]>>16][0]+registros[MBR]] = ~(memoria[tabla[registros[DS]>>16][0]+registros[MBR]]);
                cambiarCC(memoria[tabla[registros[DS]>>16][0]+registros[MBR]], registros);
                if (flag){
                    printf("[%04X]:", IPant);
                    for (int i = IPant; i < registros[IP]; i++){
                        printf("%02X", memoria[i]);
                    }
                    printf("\t | NOT [%d]", registros[MBR]);
                    
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
    else{// operando de registro
        registros[op1 & 0x1F] = ~registros[op1 & 0x1F];
        cambiarCC(registros[op1 & 0x1F], registros);
        if (flag){
                printf("[%04X]:", IPant);
                for (int i = IPant; i < registros[IP]; i++){
                    printf("%02X", memoria[i]);
                }
                printf("\t | NOT [%s]", nomRegistro[op1 & 0x1F]); 
            }
    }
}
void b(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    
}
void C(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    
}
void D(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    
}
void E(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    
}
// Imprime un entero en formato binario con prefijo 0b según la cantidad de bits
void imprimir_binario(int valor, int tam_bytes) {
    int total_bits = tam_bytes * 8;
    printf("0b");
    for (int b = total_bits - 1; b >= 0; b--) {
        printf("%d", (valor >> b) & 1);
    }
}
void SYS(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    int tiposys = op1 & 0xFFFF;
    int i;
    if (flag) {
    printf("[%04X] ", IPant);
    for (i = IPant; i < registros[IP]; i++) {
        printf("%02X ", (unsigned char)memoria[i]);
    }
    printf("\t| SYS %d \n", tiposys);
    }
    int tamanio=(registros[ECX]>>16)&0XFFFF;
    int dirlog=registros[EDX];//desde donde parto
    int formato=registros[EAX];
    int offset=dirlog&0XFFFF;
    int segment=(dirlog>>16)&0XFFFF;
    int dirfis=tabla[segment][0];
    int cantidad=registros[ECX]&0XFFFF;/*cantidad de elementos a leer\escribir*/
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
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }
        }

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
    }
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
    if (flag){
        printf("[%04X]:", IPant);
        for (int i = IPant; i < registros[IP]; i++){
            printf("%02X", memoria[i]);
        }
        printf("\t AND ");
    }
    
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
                if(flag)
                    printf("[%d], ", registros[MBR]);
                if (op2 >> 24 == 3){// segundo op de memoria
                    int valorA = registros[MBR];

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
                            memoria[tabla[registros[DS]>>16][0]+registros[valorA]] &= memoria[tabla[registros[DS]>>16][0]+registros[MBR]];
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
                    memoria[tabla[registros[DS]>>16][0]+registros[MBR]] &= registros[op2 & 0x1F];
                    if (flag)
                        printf("%s\n", nomRegistro[op2 & 0x1F]);
                }else{//segundo op inmediato
                    memoria[tabla[registros[DS]>>16][0]+registros[MBR]] &= (op2 & 0xFFFFFF); 
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
        cambiarCC(memoria[tabla[registros[DS]>>16][0]+registros[MBR]], registros);
    }
    else { //primer op de registro
        if (flag)
            printf("%s, ", nomRegistro[op2 & 0x1F]);
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
                    if (flag)
                        printf("[%d]\n", registros[MBR]);
                    registros[op1 & 0x1F] &= memoria[tabla[registros[DS]>>16][0]+registros[MBR]];
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
            if (flag)
                printf("%s\n", nomRegistro[op2 & 0x1F]);
        }else{//segundo op inmediato
            registros[op1 & 0x1F] &= (op2 & 0xFFFFFF); 
            if (flag)
                printf("%d\n", op2&0xFFFFFF);
        }
        cambiarCC(registros[op1 & 0x1F], registros);
    }
}
void OR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    if (flag){
        printf("[%04X]:", IPant);
        for (int i = IPant; i < registros[IP]; i++){
            printf("%02X", memoria[i]);
        }
        printf("\t OR ");
    }
    
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
                if(flag)
                    printf("[%d], ", registros[MBR]);
                if (op2 >> 24 == 3){// segundo op de memoria
                    int valorA = registros[MBR];

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
                            memoria[tabla[registros[DS]>>16][0]+registros[valorA]] |= memoria[tabla[registros[DS]>>16][0]+registros[MBR]];
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
                    memoria[tabla[registros[DS]>>16][0]+registros[MBR]] |= registros[op2 & 0x1F];
                    if (flag)
                        printf("%s\n", nomRegistro[op2 & 0x1F]);
                }else{//segundo op inmediato
                    memoria[tabla[registros[DS]>>16][0]+registros[MBR]] |= (op2 & 0xFFFFFF); 
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
        cambiarCC(memoria[tabla[registros[DS]>>16][0]+registros[MBR]], registros);
    }
    else { //primer op de registro
        if (flag)
            printf("%s, ", nomRegistro[op2 & 0x1F]);
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
                    if (flag)
                        printf("[%d]\n", registros[MBR]);
                    registros[op1 & 0x1F] |= memoria[tabla[registros[DS]>>16][0]+registros[MBR]];
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
            if (flag)
                printf("%s\n", nomRegistro[op2 & 0x1F]);
        }else{//segundo op inmediato
            registros[op1 & 0x1F] |= (op2 & 0xFFFFFF); 
            if (flag)
                printf("%d\n", op2&0xFFFFFF);
        }
        cambiarCC(registros[op1 & 0x1F], registros);
    }
}
void XOR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    if (flag){
        printf("[%04X]:", IPant);
        for (int i = IPant; i < registros[IP]; i++){
            printf("%02X", memoria[i]);
        }
        printf("\t XOR ");
    }
    
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
                if(flag)
                    printf("[%d], ", registros[MBR]);
                if (op2 >> 24 == 3){// segundo op de memoria
                    int valorA = registros[MBR];

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
                            memoria[tabla[registros[DS]>>16][0]+registros[valorA]] ^= memoria[tabla[registros[DS]>>16][0]+registros[MBR]];
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
                    memoria[tabla[registros[DS]>>16][0]+registros[MBR]] ^= registros[op2 & 0x1F];
                    if (flag)
                        printf("%s\n", nomRegistro[op2 & 0x1F]);
                }else{//segundo op inmediato
                    memoria[tabla[registros[DS]>>16][0]+registros[MBR]] ^= (op2 & 0xFFFFFF); 
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
        cambiarCC(memoria[tabla[registros[DS]>>16][0]+registros[MBR]], registros);
    }
    else { //primer op de registro
        if (flag)
            printf("%s, ", nomRegistro[op2 & 0x1F]);
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
                    if (flag)
                        printf("[%d]\n", registros[MBR]);
                    registros[op1 & 0x1F] ^= memoria[tabla[registros[DS]>>16][0]+registros[MBR]];
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
            if (flag)
                printf("%s\n", nomRegistro[op2 & 0x1F]);
        }else{//segundo op inmediato
            registros[op1 & 0x1F] ^= (op2 & 0xFFFFFF); 
            if (flag)
                printf("%d\n", op2&0xFFFFFF);
        }
        cambiarCC(registros[op1 & 0x1F], registros);
    }
}
void SWAP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]){
    if (flag){
        printf("[%04X]:", IPant);
        for (int i = IPant; i < registros[IP]; i++){
            printf("%02X", memoria[i]);
        }
        printf("\t SWAP ");
    }
    
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
                if(flag)
                    printf("[%d], ", registros[MBR]);
                if (op2 >> 24 == 3){// segundo op de memoria
                    int valorA = registros[MBR];

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

                            memoria[tabla[registros[DS]>>16][0]+registros[valorA]] ^= memoria[tabla[registros[DS]>>16][0]+registros[MBR]];
                            memoria[tabla[registros[DS]>>16][0]+registros[MBR]] ^= memoria[tabla[registros[DS]>>16][0]+registros[valorA]];  
                            memoria[tabla[registros[DS]>>16][0]+registros[valorA]] ^= memoria[tabla[registros[DS]>>16][0]+registros[MBR]];
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
                    memoria[tabla[registros[DS]>>16][0]+registros[MBR]] ^= registros[op2 & 0x1F];
                    registros[op2 & 0x1F] ^= memoria[tabla[registros[DS]>>16][0]+registros[MBR]];
                    memoria[tabla[registros[DS]>>16][0]+registros[MBR]] ^= registros[op2 & 0x1F];
                    if (flag)
                        printf("%s\n", nomRegistro[op2 & 0x1F]);
                }else{//segundo op inmediato
                    memoria[tabla[registros[DS]>>16][0]+registros[MBR]] ^= (op2 & 0xFFFFFF);
                    (op2 & 0xFFFFFF)  ^= memoria[tabla[registros[DS]>>16][0]+registros[MBR]];
                    memoria[tabla[registros[DS]>>16][0]+registros[MBR]] ^= (op2 & 0xFFFFFF); 
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
        cambiarCC(memoria[tabla[registros[DS]>>16][0]+registros[MBR]], registros);
    }
    else { //primer op de registro
        if (flag)
            printf("%s, ", nomRegistro[op2 & 0x1F]);
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
                    if (flag)
                        printf("[%d]\n", registros[MBR]);
                    registros[op1 & 0x1F] ^= memoria[tabla[registros[DS]>>16][0]+registros[MBR]];
                    memoria[tabla[registros[DS]>>16][0]+registros[MBR]] ^=  registros[op1 & 0x1F];
                    registros[op1 & 0x1F] ^= memoria[tabla[registros[DS]>>16][0]+registros[MBR]];
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
            if (flag)
                printf("%s\n", nomRegistro[op2 & 0x1F]);
        }else{//segundo op inmediato
            registros[op1 & 0x1F] ^= (op2 & 0xFFFFFF);
            (op2 & 0xFFFFFF)  ^= registros[op1 & 0x1F];
            registros[op1 & 0x1F] ^= (op2 & 0xFFFFFF); 
            if (flag)
                printf("%d\n", op2&0xFFFFFF);
        }
        cambiarCC(registros[op1 & 0x1F], registros);
    }
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
