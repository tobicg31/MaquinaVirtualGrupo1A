#include "operaciones.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>



void escribirRegistro(int registros[REGISTROS], int regis, int valor, int ver){
    if(ver==1){
        registros[regis]=valor;
        return;
    }
    switch ((regis >> 5) & 0x3){
    case 3:
        registros[regis] = registros[regis] & 0xFFFF0000 | (valor & 0xFFFF);
        break;
    case 2:
        registros[regis] = registros[regis] & 0xFFFF00FF | ((valor & 0xFF) << 8);
        break;
    case 1:
        registros[regis] = registros[regis] & 0xFFFFFF00 | (valor & 0xFF);
        break;
    default:
        registros[regis] = valor;
        break;
    }
}

int32_t leerRegistro(int registros[REGISTROS], int regis, int ver){
    int32_t valor = 0xFFFFFFFF;
    if (registros[regis] >= 0)
        valor = 0;
        switch ((regis >> 5) & 0x3){
            case 3:
            valor |= registros[regis] & 0xFFFF;
            break;
            case 2:
            valor |= (registros[regis] >> 8) & 0xFF;
            break;
            case 1:
            valor |= registros[regis] & 0xFF;
            break;
            default:
            valor = registros[regis];
            break;
        }
    if (ver==1){
        valor=registros[regis];
    }
    return valor;
}

int32_t leerMemoria32(char memoria[MEMORIA], int dir, int ver, int mod){
    if (ver == 1){
        return ((unsigned char)memoria[dir] << 24) |
               ((unsigned char)memoria[dir + 1] << 16) |
               ((unsigned char)memoria[dir + 2] << 8) |
               ((unsigned char)memoria[dir + 3]);
    }
    else{
        int32_t dato = 0;
        for (int i = 0; i < mod; i++){
            dato |= memoria[dir + i] << (mod - i - 1) * 8;
        }
        return dato;
    }
}
/*
 */

void escribirMemoria32(char memoria[MEMORIA], int dir, int32_t valor, int ver, int mod){
    if (ver == 1){
        memoria[dir] = (valor >> 24) & 0xFF;
        memoria[dir + 1] = (valor >> 16) & 0xFF;
        memoria[dir + 2] = (valor >> 8) & 0xFF;
        memoria[dir + 3] = valor & 0xFF;
    }
    else{
        for (int i = 0; i < mod; i++){
            memoria[dir + i] = (valor >> (mod - i - 1) * 8) & 0xFF;
        }
    }
}

void cargarLAR(int op, int registros[REGISTROS], short int tabla[8][2]){
    int base = registros[op & 0x1F]; // puntero base: segmento(16) + offset(16)
    int segmento = (base >> 16) & 0xFFFF;
    int offsetBase = base & 0xFFFF;
    int desplazamiento = (op >> 8) & 0xFFFF; // el offset codificado en la instruccion
    registros[LAR] = (segmento << 16) | ((offsetBase + desplazamiento) & 0xFFFF);
}

int validoDirFisica(int op, int registros[REGISTROS], short int tabla[8][2]){
    int segmento = registros[LAR] >> 16;
    int offset = registros[LAR] & 0xFFFF;
    int tamanio = registros[MAR] >> 16;
    return (offset + tamanio) <= tabla[segmento][1];
}
void imprimirOperando(int Top, int op, char *nomRegistro[32], int registros[REGISTROS]){
    switch (op >> 24){
    case 1:
        printf("%s", nomRegistro[op & 0x1F]);
        break;
    case 2:
        short int inmediato = (short int)(op & 0xFFFF);
        printf("%d", inmediato);
        break;
    case 3:
        int reg = op & 0x1F;                                // código del registro
        short int offset = (short int)((op >> 8) & 0xFFFF); // desplazamiento codificado
        printf("[%s+%d]", nomRegistro[reg], offset);
        break;
    }
}

void disassembler(int flag, int tOp, int op1, int op2, char *nomRegistro[32], int IPant, char memoria[MEMORIA], int registros[REGISTROS], char *funcion){
    if (flag){
        printf("[%04X]:", IPant);
        for (int i = IPant; i < registros[IP]; i++){
            printf("%02X ", (unsigned char)memoria[i]);
        }
        printf("\t | %s ", funcion);
        switch (tOp){
        case 0:
            printf("\n");
            break;
        case 1:
            imprimirOperando(tOp, op1, nomRegistro, registros);
            printf("\n");
            break;
        case 2:
            imprimirOperando(tOp, op1, nomRegistro, registros);
            printf(",");
            imprimirOperando(tOp, op2, nomRegistro, registros);
            printf("\n");
            break;
        }
    }
}

void STOP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){
    disassembler(flag, 0, 0, 0, nomRegistro, IPant, memoria, registros, "STOP");
    registros[IP] = -1;
}

void JMP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){
    if (op1 >> 24 == 3){
        if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){// pregunto si el codigo de segmento es valido
            cargarLAR(op1, registros, tabla); // guardo la dir logica
            // en la parte del MAR cuantos bytes:
            registros[MAR] = 4 << 16; // creo q son 4 bytes porq leemos numeros(?)
            // traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op1, registros, tabla)){
                // guardar en el MbR el valor:
                registros[MBR] = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op1 >> 5) & 0x3);

                registros[IP] = registros[MBR]; // hago el salto;
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
    else if (op1 >> 24 == 1){ // registro
        registros[IP] = leerRegistro(registros, op1&0x1F, ver);
    }
    else{ // segundo op inmediato
        registros[IP] = ((op1 & 0xFFFFFF)) & 0xFFFF;
    }
}
void JP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){
    if ((uint32_t)registros[CC] >> 30 == 0){ // N y Z son 0
        JMP(op1, op2, 0, memoria, registros, tabla, IPant, nomRegistro, ver,enter,archivo_vmi);
        disassembler(flag, 1, op1, op2, nomRegistro, IPant, memoria, registros, "JP");
    }
}
void JN(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){
    if ((uint32_t)registros[CC] >> 30 == 0b10){ // N==1 Z==0
        JMP(op1, op2, 0, memoria, registros, tabla, IPant, nomRegistro, ver,enter,archivo_vmi);
        disassembler(flag, 1, op1, op2, nomRegistro, IPant, memoria, registros, "JN");
    }
}
void JZ(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){
    if ((uint32_t)registros[CC] >> 30 == 0b01){ // N==0 Z==1
        JMP(op1, op2, 0, memoria, registros, tabla, IPant, nomRegistro, ver,enter,archivo_vmi);
        disassembler(flag, 1, op1, op2, nomRegistro, IPant, memoria, registros, "JZ");
    }
}
void JC(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){
    if ((uint32_t)registros[CC] >> 29 & 1 == 1){ // C==1
        JMP(op1, op2, 0, memoria, registros, tabla, IPant, nomRegistro, ver,enter,archivo_vmi);
        disassembler(flag, 1, op1, op2, nomRegistro, IPant, memoria, registros, "JC");
    }
}
void JV(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){
    if ((uint32_t)registros[CC] >> 28 & 1 == 1){ // V==1
        JMP(op1, op2, 0, memoria, registros, tabla, IPant, nomRegistro, ver,enter,archivo_vmi);
        disassembler(flag, 1, op1, op2, nomRegistro, IPant, memoria, registros, "JV");
    }
}
void JNP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){
    if ((uint32_t)registros[CC] >> 30 == 0b10 || (uint32_t)registros[CC] >> 30 == 0b01){ // N o Z == 1
        JMP(op1, op2, 0, memoria, registros, tabla, IPant, nomRegistro, ver,enter,archivo_vmi);
        disassembler(flag, 1, op1, op2, nomRegistro, IPant, memoria, registros, "JNP");
    }
}
void JNN(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){
    if (((uint32_t)registros[CC] >> 31) & 1 == 0){ // N == 0
        JMP(op1, op2, 0, memoria, registros, tabla, IPant, nomRegistro, ver,enter,archivo_vmi);
        disassembler(flag, 1, op1, op2, nomRegistro, IPant, memoria, registros, "JNN");
    }
}
void JNZ(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){
    if (((uint32_t)registros[CC] >> 30 & 1) == 0)
    { // Z == 0
        JMP(op1, op2, 0, memoria, registros, tabla, IPant, nomRegistro, ver,enter,archivo_vmi);
        disassembler(flag, 1, op1, op2, nomRegistro, IPant, memoria, registros, "JNZ");
    }
}

void cambiarCC(int32_t valor, int carry, int overflow, int registros[REGISTROS]){
    uint32_t cc = 0;

    if (valor < 0)
        cc |= (uint32_t)1 << 31;
    else if (valor == 0)
        cc |= (uint32_t)1 << 30;

    if (carry)
        cc |= (uint32_t)1 << 29;

    if (overflow)
        cc |= (uint32_t)1 << 28;

    registros[CC] = (int32_t)cc;
}

void NOT(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){
    if (op1 >> 24 == 3){
        if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){// pregunto si el codigo de segmento es valido
            cargarLAR(op1, registros, tabla); // guardo la dir logica
            // en la parte del MAR cuantos bytes:
            registros[MAR] = 4 << 16; // creo q son 4 bytes porq leemos numeros(?)
            // traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op1, registros, tabla)){
                // guardar en el MbR el valor:
                registros[MBR] = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op1 >> 5) & 0x3);
                escribirMemoria32(memoria, registros[MAR] & 0xFFFF, ~registros[MBR], ver, (op1 >> 5) & 0x3);
                // registros[MBR] = leerMemoria32(memoria, registros[MAR] & 0xFFFF);
                // escribirMemoria32(memoria,registros[MAR] & 0xFFFF,~registros[MBR]);
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
    }
    else if (op1 >> 24 == 1){ // registro
        escribirRegistro(registros, op1&0x1F, ~leerRegistro(registros, op1&0x1F, ver), ver);
    }
    else{ // segundo op inmediato
        int dir = tabla[registros[DS] >> 16][0] + ((op2 & 0xFFFFFF) & 0xFFFF);
        int res = ~leerMemoria32(memoria, dir, ver, (op2 >> 5) & 0x3);
        escribirMemoria32(memoria, dir, res, ver, (op2 >> 5) & 0x3);
        disassembler(flag, 1, op1, op2, nomRegistro, IPant, memoria, registros, "NOT");
    }
}

void PUSH(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){
    int contenidoPila;
    if (registros[SP] - 4 <= registros[SS]){
        printf("STACK OVERFLOW");
        registros[IP] = -1;
        return;
    }
    else{
        if (op1 >> 24 == 3){
            if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){// pregunto si el codigo de segmento es valido
                cargarLAR(op1, registros, tabla); // guardo la dir logica
                // en la parte del MAR cuantos bytes:
                registros[MAR] = 4 << 16; // creo q son 4 bytes porq leemos numeros(?)
                // traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
                registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
                if (validoDirFisica(op1, registros, tabla)){
                    registros[MBR] = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op1 >> 5) & 0x3);
                    registros[SP] -= 4;
                    escribirMemoria32(memoria, registros[SP], registros[MBR], ver, 4);
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
    else   
        if (op1 >> 24 == 1){//registro

            int32_t leer = leerRegistro(registros, op1 & 0x1F, ver);
            registros[SP] -= 4;
            escribirMemoria32(memoria, registros[SP], leer, ver, (op1 >> 5) & 0x3);
        }
        else{ // segundo op inmediato
            registros[SP]-=4;
            escribirMemoria32(memoria, registros[SP] , op2&0xFFFFFF, ver, (op1>>5)&0x3);
        }
    }
}

void POP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){
    int contenidoPila;
    if (registros[SP] == (registros[SS] + tabla[registros[op1&0x1F]>>16][1]-tabla[registros[op1&0x1F]>>16][0])){
        printf("STACK UNDERFLOW");
        registros[IP] = -1;
        return;
    }
    else{
        if (op1 >> 24 == 3){
            if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){ // pregunto si el codigo de segmento es valido
                cargarLAR(op1, registros, tabla); // guardo la dir logica
                // en la parte del MAR cuantos bytes:
                registros[MAR] = 4 << 16; // creo q son 4 bytes porq leemos numeros(?)
                // traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
                registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
                if (validoDirFisica(op1, registros, tabla)){
                    registros[MBR] = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op1 >> 5) & 0x3);
                    contenidoPila = leerMemoria32(memoria, registros[SP], ver, 4);
                    escribirMemoria32(memoria, registros[MAR] & 0xFFFF, contenidoPila, ver, (op1 >> 5) & 0x3);
                    registros[SP] += 4;
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
        else if (op1 >> 24 == 1){ // registro
            escribirRegistro(registros, op1 & 0x1F, leerMemoria32(memoria, registros[SP], ver, 4), ver);
            registros[SP] += 4;
        }
    }
}
void CALL(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){
    int opIP = 1<<24; //operando de ip
    PUSH(opIP, op2, flag, memoria, registros, tabla, IPant, nomRegistro, ver,enter,archivo_vmi);
    JMP(op1,op2,flag,memoria,registros,tabla,IPant,nomRegistro,ver,enter,archivo_vmi);
}
void RET(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){
    int opIP = 1<<24; //operando de ip
    POP(opIP, op2, flag, memoria, registros, tabla, IPant, nomRegistro, ver,enter,archivo_vmi);
}

// Imprime un entero en formato binario con prefijo 0b según la cantidad de bits
void imprimir_binario(int valor, int tam_bytes){
    int total_bits = tam_bytes * 8;
    printf("0b");
    for (int b = total_bits - 1; b >= 0; b--)
    {
        printf("%d", (valor >> b) & 1);
    }
}
void hacebreakpoint(char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],char *archivo_vmi){
    FILE * f = fopen(archivo_vmi, "wb");
    printf("[BREAKPOINT] se genera imagen en %s\n", archivo_vmi);
    if (!f){
        printf("[BREAKPOINT] no se pudo crear %s\n", archivo_vmi);
        return;
    }
    const char ident[5] = {'V','M','I','2','6'};
    char version = 1;
    short int tamMemoriaKiB=16;
    fwrite(ident, 1, 5, f);
    fwrite(&version, 1, 1, f);
    fwrite(&tamMemoriaKiB, 2, 1, f);
    for (int i = 0; i < 32; i++)
        fwrite(&registros[i], 4, 1, f);
    for (int i = 0; i < 8; i++){
        fwrite(&tabla[i][0], 2, 1, f);
        fwrite(&tabla[i][1], 2, 1, f);
    }
    fwrite(memoria, 1, tamMemoriaKiB * 1024, f);
    fclose(f);
}
void SYS(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){
    int tiposys = (short int)op1 & 0xFFFF;
    if (tiposys == 15){ //aca porque no hay q validar nada, es un sys de breakpoint, no tiene sentido validar si se puede leer o escribir memoria
          if (archivo_vmi == NULL){
        printf("[BREAKPOINT] no se indico archivo .vmi: no se genera imagen\n");
        return;
          }
        else{
         hacebreakpoint(memoria,  registros, tabla, archivo_vmi);
         char caracter[2]; 
         scanf("%c",caracter);
        //no evaluo g porque si se escribe 'g' se sigue ejecutando igual

        if (caracter[0] == '\n'){
            //ejecuta paso a paso
            *enter=1;
        }
        else if (caracter[0] == 'q'){
            //termina ejecucion
            registros[IP] = -1;
        }
        return;
    }
    }
    int tamanio = (registros[ECX] >> 16) & 0XFFFF;
    int dirlog = registros[EDX]; // desde donde parto
    int formato = registros[EAX];
    int offset = dirlog & 0XFFFF;
    int segment = (dirlog >> 16) & 0XFFFF;
    if (segment < 0 || segment >= 8 || tabla[segment][0] == -1){
        printf("FALLO DE SEGMENTO");
        registros[IP] = -1;
        return;
    }
    int dirfis = tabla[segment][0];
    int cantidad = (registros[ECX] & 0XFFFF); /*cantidad de elementos a leer\escribir*/
    int i;
    if (tiposys == 1){
        for (i = 0; i < cantidad; i++){
            int offset_actual = offset + (i * tamanio);
            if (offset_actual + tamanio <= tabla[segment][1]){ // valido dir fisica
                int dirfis_actual = dirfis + offset_actual;
                registros[LAR] = (segment << 16) | (offset_actual & 0xFFFF); // guardo la dir logica
                registros[MAR] = (tamanio << 16) | (dirfis_actual & 0xFFFF); // tamaño y direccion fisica de elemento a guardar
                printf("[%04X]: ", dirfis_actual & 0xFFFF);
                int datoleido = 0;
                if (formato & 0x02){ // caracter, pesa 1 byte
                    char c = getchar();
                    datoleido = (unsigned char)c;
                }
                else{
                    if (formato & 0X08){ // HEXA, uso %x
                        scanf("%x", &datoleido);
                    }
                    else{
                        if (formato & 0X04){ // OCTAL, uso %o
                            scanf("%o", &datoleido);
                        }
                        else{
                            if (formato & 0x10){ // BINARIO, se lee como string y strtol lo pasa a entero base 2 (no sabia como hacerlo esto me dijo chat GPT, preguntar)
                                char buffer[64];
                                scanf("%s", buffer);
                                char *p = buffer;
                                if (p[0] == '0' && (p[1] == 'b' || p[1] == 'B'))
                                    p += 2;
                                datoleido = (int)strtol(p, NULL, 2);
                            }
                            else{ // DECIMAL
                                scanf("%d", &datoleido);
                            }
                        }
                    }
                }
                // guardar en el MBR el valor:
                registros[MBR] = datoleido;
                for (int byte = 0; byte < tamanio; byte++){ // iteracion que guarda el (tipo de dato)valor del dato en memoria
                    memoria[dirfis_actual + byte] = (datoleido >> (8 * (tamanio - 1 - byte)) /*Calcula cuántos bits hay que desplazar valor_leido hacia la derecha para bajar el byte deseado*/) & 0xFF /*un byte*/;
                }
            }
            else{
                printf("FALLO DE SEGMENTO fallo sys");
                registros[IP] = -1;
                return;
            }
        }
    }
    else if (tiposys == 2){ // WRITE
        for (i = 0; i < cantidad; i++){
            int offset_actual = offset + (i * tamanio);
            int dirfis_actual;
            if (offset_actual + tamanio <= tabla[segment][1]){ // valido dirfis
                dirfis_actual = dirfis + offset_actual;
                registros[LAR] = (segment << 16) | (offset_actual & 0xFFFF); // guardo la dir logica
                registros[MAR] = (tamanio << 16) | (dirfis_actual & 0xFFFF); // tamaño y direccion fisica de elemento a guardar
                int valor = 0;
                for (int byte = 0; byte < tamanio; byte++){
                    valor = (valor << 8) | (unsigned char)memoria[dirfis_actual + byte]; // unsigned para q no interprete negativos para asi poder unir bits
                }
                // Sign-extend según la cantidad de bytes leídos
                if (tamanio < 4){
                    int bits = tamanio * 8;
                    int signBit = 1 << (bits - 1);
                    if (valor & signBit){
                        valor -= (1 << bits);
                    }
                }
                registros[MBR] = valor;

                // 1. Imprime dirección física (4 dígitos hexadecimales)
                printf("[%04X]: ", dirfis_actual & 0xFFFF);

                // 2. Imprimir en los formatos activos según bits de EAX, puede ser que me venga mas de un formato (combinado), entonces no usop else
                if (formato & 0x10){ // bit 4: binario
                    imprimir_binario(valor, tamanio);
                    printf(" ");
                }
                if (formato & 0x08){ // bit 3: Hexadecimal
                    // Ajustar ancho de dígitos según el tamaño en bytes, sugerencia de IA, preguntar
                    if (tamanio == 1)
                        printf("0x%02X ", valor & 0xFF);
                    else
                    {
                        if (tamanio == 2)
                            printf("0x%04X ", valor & 0xFFFF);
                        else
                            printf("0x%08X ", valor);
                    }
                }
                if (formato & 0x04){ // bit 2: Octal
                    printf("0o%o ", valor);
                }
                if (formato & 0x02){ // bit 1: Caracteres ASCII
                    for (int b = 0; b < tamanio; b++){
                        unsigned char c = (unsigned char)memoria[dirfis_actual + b];
                        printf("%c", isprint(c) ? c : '.');
                    }
                    printf(" ");
                }
                if (formato & 0x01){ // bit 0: Decimal
                    printf("%d", (unsigned)valor);
                }

                printf("\n"); // bajo de linea por si tengo q escribir otro
            }
            else{
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }
        }
        
    }
    else if (tiposys == 0){
        printf("\x1b[2J\x1b[H");

    }
    else if (tiposys == 3){
    }
    else if (tiposys == 4){
    }
 disassembler(flag, 1, op1, op2, nomRegistro, IPant, memoria, registros, "SYS");
}
void MOV(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){
    int valor_fuente = 0;

    // 1. Obtener valor de la fuente (op2)
    if (op2 >> 24 == 3){ // Memoria
        if (tabla[(registros[op2 & 0x1F]) >> 16][0] != -1){
            cargarLAR(op2, registros, tabla);
            registros[MAR] = 4 << 16;
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op2, registros, tabla)){
                valor_fuente = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op2 >> 5) & 0x3);
                registros[MBR] = valor_fuente;
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
    else if (op2 >> 24 == 1){ // Registro
        valor_fuente = leerRegistro(registros, op2&0x1F,ver) ;
    }
    else{ // Inmediato
        short int inmediato16 = (short int)(op2 & 0xFFFF);
        valor_fuente = (int)inmediato16;
    }

    // 2. Guardar en el destino (op1)
    if (op1 >> 24 == 3){ // Memoria
        if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){
            cargarLAR(op1, registros, tabla);
            registros[MAR] = 4 << 16;
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op1, registros, tabla)){
                escribirMemoria32(memoria, registros[MAR] & 0xFFFF, valor_fuente, ver, (op1 >> 5) & 0x3); // simplificado a 1 byte o según corresponda
                disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "MOV");
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
    else if (op1 >> 24 == 1){ // Registro
        escribirRegistro(registros, op1&0x1F, valor_fuente, ver);
        disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "MOV");
    }

    cambiarCC(valor_fuente, 0, 0, registros); // MOV afecta al registro CC[cite: 4]
}

void ADD(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){
    int32_t valor_fuente = 0;
    int32_t valor_destino = 0;

    // 1. Obtener valor de la fuente (op2)
    if (op2 >> 24 == 3){ // Memoria
        if (tabla[(registros[op2 & 0x1F]) >> 16][0] != -1){
            cargarLAR(op2, registros, tabla);
            registros[MAR] = 4 << 16;
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op2, registros, tabla)){
                valor_fuente = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op2 >> 5) & 0x3);
                registros[MBR] = valor_fuente;
            }
            else{
                printf("FALLO DE SEGMENTO falla esto?");
                registros[IP] = -1;
                return;
            }
        }
        else{
            printf("FALLO DE SEGMENTO falla o esto");
            registros[IP] = -1;
            return;
        }
    }
    else if (op2 >> 24 == 1){ // Registro
        valor_fuente = leerRegistro(registros, op2 & 0x1F, ver);
    }
    else{ // Inmediato
        valor_fuente = (int)(short int)(op2 & 0xFFFF);
    }

    // 2. Guardar en el destino (op1)
    if (op1 >> 24 == 3){ // Memoria
        if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){
            cargarLAR(op1, registros, tabla);
            registros[MAR] = 4 << 16;
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op1, registros, tabla)){
                valor_destino = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op1 >> 5) & 0x3);
                escribirMemoria32(memoria, registros[MAR] & 0xFFFF, valor_destino + valor_fuente, ver, (op1 >> 5) & 0x3); // simplificado a 1 byte o según corresponda
                disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "ADD");
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
    else if (op1 >> 24 == 1){ // Registro
        valor_destino = leerRegistro(registros, op1 & 0x1F, ver);
        escribirRegistro(registros, op1&0x1F,valor_destino + valor_fuente, ver);
        disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "ADD");
    }
    int64_t res_signed = (int64_t)valor_destino + (int64_t)valor_fuente;
    uint64_t res_unsigned = (uint64_t)(uint32_t)valor_destino + (uint64_t)(uint32_t)valor_fuente;
    int carry = (res_unsigned > 0xFFFFFFFF) ? 1 : 0;
    int overflow = (res_signed > 2147483647 || res_signed < -2147483648) ? 1 : 0;

    cambiarCC((int32_t)res_signed, carry, overflow, registros); //
}

void SUB(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){
    int32_t valor_fuente = 0;
    int32_t valor_destino = 0;
    int64_t res_signed;
    uint64_t res_unsigned;
    int32_t resultado_32;

    // 1. Obtener valor de la fuente (op2)
    if (op2 >> 24 == 3){ // Memoria
        if (tabla[(registros[op2 & 0x1F]) >> 16][0] != -1){
            cargarLAR(op2, registros, tabla);
            registros[MAR] = 4 << 16;
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op2, registros, tabla)){
                valor_fuente = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op2 >> 5) & 0x3);
                registros[MBR] = valor_fuente;
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
    else if (op2 >> 24 == 1){ // Registro
        valor_fuente = leerRegistro(registros, op2 & 0x1F, ver);
    }
    else{ // Inmediato
        valor_fuente = (int)(short int)(op2 & 0xFFFF);
    }

    // 2. Guardar en el destino (op1)
    if (op1 >> 24 == 3){ // Memoria
        if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){
            cargarLAR(op1, registros, tabla);
            registros[MAR] = 4 << 16;
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op1, registros, tabla)){
                valor_destino = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op1 >> 5) & 0x3);
                escribirMemoria32(memoria, registros[MAR] & 0xFFFF, valor_destino - valor_fuente, ver, (op1 >> 5) & 0x3); // simplificado a 1 byte o según corresponda
                disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "SUB");
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
    else if (op1 >> 24 == 1){ // Registro
        valor_destino = leerRegistro(registros, op1&0x1F, ver);
        escribirRegistro(registros, op1&0x1F,valor_destino + valor_fuente, ver);
        disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "SUB");
    }
    // Cálculo de desbordamiento y acarreo
    res_signed = (int64_t)valor_destino - (int64_t)valor_fuente;

    // CORRECCIÓN: Para evaluar el acarreo en la resta, se suma el complemento a 2
    res_unsigned = (uint64_t)(uint32_t)valor_destino + (uint64_t)(uint32_t)(-valor_fuente);

    int carry = (res_unsigned > 0xFFFFFFFF) ? 1 : 0;
    int overflow = (res_signed > 2147483647 || res_signed < -2147483648) ? 1 : 0;

    resultado_32 = (int32_t)res_signed;
    cambiarCC(resultado_32, carry, overflow, registros);
}

void MUL(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){
    int32_t valor_fuente = 0;
    int32_t valor_destino = 0;
    int64_t res_signed;
    uint64_t res_unsigned;
    int32_t resultado_32;

    // 1. Obtener valor de la fuente (op2)
    if (op2 >> 24 == 3){ // Memoria
        if (tabla[(registros[op2 & 0x1F]) >> 16][0] != -1){
            cargarLAR(op2, registros, tabla);
            registros[MAR] = 4 << 16;
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op2, registros, tabla)){
                valor_fuente = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op2 >> 5) & 0x3);
                registros[MBR] = valor_fuente;
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
    else if (op2 >> 24 == 1){ // Registro
        valor_fuente = leerRegistro(registros, op2 & 0x1F, ver);
    }
    else{ // Inmediato
        valor_fuente = op2 & 0xFFFFFF;
    }

    // 2. Guardar en el destino (op1)
    if (op1 >> 24 == 3){ // Memoria
        if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){
            cargarLAR(op1, registros, tabla);
            registros[MAR] = 4 << 16;
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op1, registros, tabla)){
                valor_destino = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op1 >> 5) & 0x3);
                escribirMemoria32(memoria, registros[MAR] & 0xFFFF, valor_destino * valor_fuente, ver, (op1 >> 5) & 0x3); // simplificado a 1 byte o según corresponda
                disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "MUL");
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
    else if (op1 >> 24 == 1){ // Registro
        valor_destino = leerRegistro(registros, op1 & 0x1F, ver);
        escribirRegistro(registros, op1&0x1F,valor_destino * valor_fuente, ver);
        disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "MUL");
    }
    res_signed = (int64_t)valor_destino * (int64_t)valor_fuente;
    res_unsigned = (uint64_t)(uint32_t)valor_destino * (uint64_t)(uint32_t)valor_fuente;
    int carry = (res_unsigned > 0xFFFFFFFF) ? 1 : 0;
    int overflow = (res_signed > 2147483647 || res_signed < -2147483648) ? 1 : 0;
    resultado_32 = (int32_t)res_signed;
    cambiarCC(resultado_32, carry, overflow, registros);
}

void DIV(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){
    int32_t valor_fuente = 0;
    int32_t valor_destino = 0;
    int64_t res_signed;
    uint64_t res_unsigned;
    int32_t resultado_32;

    // printf("dasdasd");

    // 1. Obtener valor de la fuente (op2)
    if (op2 >> 24 == 3){ // Memoria
        if (tabla[(registros[op2 & 0x1F]) >> 16][0] != -1){
            cargarLAR(op2, registros, tabla);
            registros[MAR] = 4 << 16;
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op2, registros, tabla)){
                valor_fuente = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op2 >> 5) & 0x3);
                registros[MBR] = valor_fuente;
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
    else if (op2 >> 24 == 1){ // Registro
        valor_fuente = leerRegistro(registros, op2 & 0x1F, ver);
    }
    else{ // Inmediato
        valor_fuente = op2 & 0xFFFFFF;
    } // nos habia faltado esta llave que cierra no lo puedo creer

    if (valor_fuente == 0){
        printf("Error: División por cero\n");
        registros[IP] = -1;
        return;
    }
    else{
        // 2. Guardar en el destino (op1)
        if (op1 >> 24 == 3){ // Memoria
            if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){
                cargarLAR(op1, registros, tabla);
                registros[MAR] = 4 << 16;
                registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
                if (validoDirFisica(op1, registros, tabla)){
                    valor_destino = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op1 >> 5) & 0x3);
                    escribirMemoria32(memoria, registros[MAR] & 0xFFFF, valor_destino / valor_fuente, ver, (op1 >> 5) & 0x3);
                    registros[AC] = valor_destino % valor_fuente; // simplificado a 1 byte o según corresponda
                                                                  // printf("%d", registros[AC]);
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "DIV");
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
        else if (op1 >> 24 == 1){ // Registro
            valor_destino = leerRegistro(registros, op1 & 0x1F, ver);
             escribirRegistro(registros, op1&0x1F,valor_destino / valor_fuente, ver);
            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "DIV");
            registros[AC] = valor_destino % valor_fuente;
            // printf(" sdsdas%d", registros[AC]);
        }
        resultado_32 = valor_destino / valor_fuente;
        cambiarCC(resultado_32, 0, 0, registros);
    }
}

void CMP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){
    int32_t valor_fuente = 0;
    int32_t valor_destino = 0;
    int64_t res_signed;
    uint64_t res_unsigned;
    int32_t resultado_32;
    // 1. Obtener valor de la fuente (op2)
    if (op2 >> 24 == 3){ // Memoria
        if (tabla[(registros[op2 & 0x1F]) >> 16][0] != -1){
            cargarLAR(op2, registros, tabla);
            registros[MAR] = 4 << 16;
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op2, registros, tabla)){
                valor_fuente = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op2 >> 5) & 0x3);
                registros[MBR] = valor_fuente;
            }
            else{
                printf("FALLO DE SEGMENTO");
                registros[IP] = -1;
                return;
            }
        }
        else{
            printf("FALLO DE SEGMENTO estp");
            registros[IP] = -1;
            return;
        }
    }
    else if (op2 >> 24 == 1){ // Registro
        valor_fuente = leerRegistro(registros, op2 & 0x1F, ver);
    }
    else{ // Inmediato
        valor_fuente = op2 & 0xFFFFFF;
    }

    // 2. Guardar en el destino (op1)
    if (op1 >> 24 == 3){ // Memoria
        if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){
            cargarLAR(op1, registros, tabla);
            registros[MAR] = 4 << 16;
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op1, registros, tabla)){
                valor_destino = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op1 >> 5) & 0x3); // simplificado a 1 byte o según corresponda
                disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "CMP");
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
    else if (op1 >> 24 == 1){ // Registro
        valor_destino = leerRegistro(registros, op1 & 0x1F, ver);
        disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "CMP");
    }
    // Cálculo de desbordamiento y acarreo
    res_signed = (int64_t)valor_destino - (int64_t)valor_fuente;

    // CORRECCIÓN: Para evaluar el acarreo en la resta, se suma el complemento a 2
    res_unsigned = (uint64_t)(uint32_t)valor_destino + (uint64_t)(uint32_t)(-valor_fuente);

    int carry = (res_unsigned > 0xFFFFFFFF) ? 1 : 0;
    int overflow = (res_signed > 2147483647 || res_signed < -2147483648) ? 1 : 0;

    resultado_32 = (int32_t)res_signed;
    cambiarCC(resultado_32, carry, overflow, registros);
}

void AND(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){

    if (op1 >> 24 == 3){ // primer op de memoria
        if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){// pregunto si el codigo de segmento es valido
            cargarLAR(op1, registros, tabla); // guardo la dir logica
            // en la parte del MAR cuantos bytes:
            registros[MAR] = 4 << 16; // creo q son 4 bytes porq leemos numeros(?)
            // traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op1, registros, tabla)){
                // guardar en el MbR el valor:
                registros[MBR] = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op1 >> 5) & 0x3); // esto nos fallaba en el ultimo punto lpm
                int dirOpA = registros[MAR] & 0xFFFF;
                if (op2 >> 24 == 3){ // segundo op de memoria
                    if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){                                     // pregunto si el codigo de segmento es valido
                        cargarLAR(op2, registros, tabla); // guardo la dir logica
                        // en la parte del MAR cuantos bytes:
                        registros[MAR] = 4 << 16; // creo q son 4 bytes porq leemos numeros(?)
                        // traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
                        registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
                        if (validoDirFisica(op2, registros, tabla)){
                            // guardar en el MbR el valor:
                            registros[MBR] = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op2 >> 5) & 0x3);
                            int valorA = leerMemoria32(memoria, dirOpA, ver, (op1 >> 5) & 0x3);
                            escribirMemoria32(memoria, dirOpA, valorA &= registros[MBR], ver, (op2 >> 5) & 0x3);
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
                }
                else if (op2 >> 24 == 1){ // segundo op de registro
                    int res = leerMemoria32(memoria, dirOpA, ver, (op1 >> 5) & 0x3);
                    escribirMemoria32(memoria, dirOpA, res &= leerRegistro(registros, op2 & 0x1F, ver), ver, (op1 >> 5) & 0x3);
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "AND");
                }
                else{ // segundo op inmediato
                    int res = leerMemoria32(memoria, dirOpA, ver, (op1 >> 5) & 0x3);
                    escribirMemoria32(memoria, dirOpA, res &= (op2 & 0xFFFFFF), ver, (op1 >> 5) & 0x3);
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "AND");
                }
                cambiarCC(leerMemoria32(memoria, dirOpA, ver, (op1 >> 5) & 0x3), 0, 0, registros);
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
    else{ // primer op de registro
        if (op2 >> 24 == 3){ // segundo op de memoria
            if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){// pregunto si el codigo de segmento es valido
                cargarLAR(op2, registros, tabla); // guardo la dir logica
                // en la parte del MAR cuantos bytes:
                registros[MAR] = 4 << 16; // creo q son 4 bytes porq leemos numeros(?)
                // traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
                registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
                if (validoDirFisica(op2, registros, tabla)){
                    // guardar en el MbR el valor:
                    registros[MBR] = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op2 >> 5) & 0x3);
                    escribirRegistro(registros, op1&0x1F, leerRegistro(registros, op1 & 0x1F, ver)&registros[MBR], ver);
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
        }
        else if (op2 >> 24 == 1){ // segundo op de registro
            escribirRegistro(registros, op1&0x1F,leerRegistro(registros, op1 & 0x1F, ver) & leerRegistro(registros, op2 & 0x1F, ver), ver);
            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "AND");
        }
        else{ // segundo op inmediato
            escribirRegistro(registros, op1&0x1F,leerRegistro(registros, op1 & 0x1F, ver) &(op2 & 0xFFFFFF) , ver);
            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "AND");
        }
        cambiarCC(registros[op1 & 0x1F], 0, 0, registros);
    }
}
void OR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){

    if (op1 >> 24 == 3){ // primer op de memoria
        if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){// pregunto si el codigo de segmento es valido
            cargarLAR(op1, registros, tabla); // guardo la dir logica
            // en la parte del MAR cuantos bytes:
            registros[MAR] = 4 << 16; // creo q son 4 bytes porq leemos numeros(?)
            // traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op1, registros, tabla)){
                // guardar en el MbR el valor:
                registros[MBR] = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op1 >> 5) & 0x3);
                int dirOpA = registros[MAR] & 0xFFFF;
                if (op2 >> 24 == 3){ // segundo op de memoria

                    if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){   // pregunto si el codigo de segmento es valido
                        cargarLAR(op2, registros, tabla); // guardo la dir logica
                        // en la parte del MAR cuantos bytes:
                        registros[MAR] = 4 << 16; // creo q son 4 bytes porq leemos numeros(?)
                        // traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
                        registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
                        if (validoDirFisica(op2, registros, tabla)){
                            // guardar en el MbR el valor:
                            registros[MBR] = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op2 >> 5) & 0x3);
                            int valorA = leerMemoria32(memoria, dirOpA, ver, (op1 >> 5) & 0x3);
                            escribirMemoria32(memoria, dirOpA, valorA |= registros[MBR], ver, (op1 >> 5) & 0x3);
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
                }
                else if (op2 >> 24 == 1){ // segundo op de registro
                    int res = leerMemoria32(memoria, dirOpA, ver, (op1 >> 5) & 0x3);
                    escribirMemoria32(memoria, dirOpA, res |= leerRegistro(registros, op2 & 0x1F, ver), ver, (op1 >> 5) & 0x3);
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "OR");
                }
                else{ // segundo op inmediato
                    int res = leerMemoria32(memoria, dirOpA, ver, (op1 >> 5) & 0x3);
                    escribirMemoria32(memoria, dirOpA, res &= (op2 & 0xFFFFFF), ver, (op1 >> 5) & 0x3);
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "OR");
                }
                cambiarCC(leerMemoria32(memoria, dirOpA, ver, (op1 >> 5) & 0x3), 0, 0, registros);
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
    else{ // primer op de registro
        if (op2 >> 24 == 3){ // segundo op de memoria
            if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){// pregunto si el codigo de segmento es valido
                cargarLAR(op2, registros, tabla); // guardo la dir logica
                // en la parte del MAR cuantos bytes:
                registros[MAR] = 4 << 16; // creo q son 4 bytes porq leemos numeros(?)
                // traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
                registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
                if (validoDirFisica(op2, registros, tabla)){
                    // guardar en el MbR el valor:
                    int dest = leerRegistro(registros, op1 & 0x1F, ver);
                    registros[MBR] = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op2 >> 5) & 0x3);
                    escribirRegistro(registros, op1&0x1F, dest|= registros[MBR], ver);
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
        }
        else if (op2 >> 24 == 1){ // segundo op de registro
            int dest = leerRegistro(registros, op1 & 0x1F, ver);
            escribirRegistro(registros, op1&0x1F, dest |= leerRegistro(registros, op2 & 0x1F, ver), ver);
            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "OR");
        }
        else{ // segundo op inmediato
            int dest = leerRegistro(registros, op1 & 0x1F, ver);
            escribirRegistro(registros, op1&0x1F, dest |=(op2 & 0xFFFFFF), ver);
            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "OR");
        }
        cambiarCC(registros[op1 & 0x1F], 0, 0, registros);
    }
}
void XOR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){

    if (op1 >> 24 == 3){ // primer op de memoria
        if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){// pregunto si el codigo de segmento es valido
            cargarLAR(op1, registros, tabla); // guardo la dir logica
            // en la parte del MAR cuantos bytes:
            registros[MAR] = 4 << 16; // creo q son 4 bytes porq leemos numeros(?)
            // traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op1, registros, tabla)){
                // guardar en el MbR el valor:
                registros[MBR] = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op1 >> 5) & 0x3);
                int dirOpA = registros[MAR] & 0xFFFF;
                if (op2 >> 24 == 3){ // segundo op de memoria

                    if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){                                     // pregunto si el codigo de segmento es valido
                        cargarLAR(op2, registros, tabla); // guardo la dir logica
                        // en la parte del MAR cuantos bytes:
                        registros[MAR] = 4 << 16; // creo q son 4 bytes porq leemos numeros(?)
                        // traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
                        registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
                        if (validoDirFisica(op2, registros, tabla)){
                            // guardar en el MbR el valor:
                            registros[MBR] = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op2 >> 5) & 0x3);
                            int valorA = leerMemoria32(memoria, dirOpA, ver, (op1 >> 5) & 0x3);
                            escribirMemoria32(memoria, dirOpA, valorA ^= registros[MBR], ver, (op1 >> 5) & 0x3);
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
                }
                else if (op2 >> 24 == 1){ // segundo op de registro
                    int res = leerMemoria32(memoria, dirOpA, ver, (op1 >> 5) & 0x3);
                    escribirMemoria32(memoria, dirOpA, res ^= leerRegistro(registros, op1 & 0x1F, ver), ver, (op1 >> 5) & 0x3);
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "XOR");
                }
                else{ // segundo op inmediato
                    int res = leerMemoria32(memoria, dirOpA, ver, (op1 >> 5) & 0x3);
                    escribirMemoria32(memoria, dirOpA, res ^= (op2 & 0xFFFFFF), ver, (op1 >> 5) & 0x3);
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "XOR");
                }
                cambiarCC(leerMemoria32(memoria, dirOpA, ver, (op1 >> 5) & 0x3), 0, 0, registros);
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
    else{ // primer op de registro
        if (op2 >> 24 == 3){ // segundo op de memoria
            if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){   // pregunto si el codigo de segmento es valido
                cargarLAR(op2, registros, tabla); // guardo la dir logica
                // en la parte del MAR cuantos bytes:
                registros[MAR] = 4 << 16; // creo q son 4 bytes porq leemos numeros(?)
                // traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
                registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
                if (validoDirFisica(op2, registros, tabla)){
                    // guardar en el MbR el valor:
                    registros[MBR] = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op2 >> 5) & 0x3);
                    int dest=leerRegistro(registros, op1 & 0x1F, ver);
                    escribirRegistro(registros, op1&0x1F,dest^= registros[MBR] , ver);
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
        }
        else if (op2 >> 24 == 1){ // segundo op de registro
            int dest=leerRegistro(registros, op1 & 0x1F, ver);
            escribirRegistro(registros, op1&0x1F, dest ^= leerRegistro(registros, op2 & 0x1F, ver), ver);
            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "XOR");
        }
        else{ // segundo op inmediato
            int dest=leerRegistro(registros, op1 & 0x1F, ver);
            escribirRegistro(registros, op1&0x1F, dest ^= (op2 & 0xFFFFFF), ver);
            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "XOR");
        }
        cambiarCC(registros[op1 & 0x1F], 0, 0, registros);
    }
}
void SWAP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){
    // Se llama a XOR tres veces con flag en 0 para que no ensucie la pantalla
    XOR(op1, op2, 0, memoria, registros, tabla, IPant, nomRegistro, ver, enter, archivo_vmi);
    XOR(op2, op1, 0, memoria, registros, tabla, IPant, nomRegistro, ver, enter, archivo_vmi); // Se invierte el orden
    XOR(op1, op2, 0, memoria, registros, tabla, IPant, nomRegistro, ver, enter, archivo_vmi);

    // Se imprime la instrucción real una sola vez
    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "SWAP");
}
void SHL(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){
    int i, primerbit, direfisopa, direfisopb, valorA;
    int64_t clonconsigno;
    uint64_t clonsinsigno;
    clonconsigno = clonsinsigno = 0;
    int desbordamiento = 0;

    if (op1 >> 24 == 3){ // primer op de memoria
        if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){// pregunto si el codigo de segmento es valido
            cargarLAR(op1, registros, tabla); // guardo la dir logica
            // en la parte del MAR cuantos bytes:
            registros[MAR] = 4 << 16; // creo q son 4 bytes porq leemos numeros(?)
            // traducir a dir fisica, guardarla en la parte baja del MBR, reviso si no se cae del segmento:
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op1, registros, tabla)){
                // guardar en el MBR el valor:
                registros[MBR] = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op1 >> 5) & 0x3);
                direfisopa = registros[MAR] & 0xFFFF;
                clonconsigno = leerMemoria32(memoria, direfisopa, ver, (op1 >> 5) & 0x3);
                clonsinsigno = (uint32_t)clonconsigno;

                if (op2 >> 24 == 3){ // segundo op de memoria

                    // clonconsigno|=registros[MBR];
                    // clonsinsigno|=(unsigned int)registros[MBR];

                    if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){// pregunto si el codigo de segmento es valido
                        cargarLAR(op2, registros, tabla); // guardo la dir logica
                        // en la parte del MAR cuantos bytes:
                        registros[MAR] = 4 << 16; // creo q son 4 bytes porq leemos numeros(?)
                        // traducir a dir fisica, guardarla en la parte baja del MBR, reviso si no se cae del segmento:
                        registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
                        if (validoDirFisica(op2, registros, tabla)){
                            // guardar en el MBR el valor:
                            registros[MBR] = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op2 >> 5) & 0x3);
                            int acarreo = 0;
                            int valorDeA = leerMemoria32(memoria, direfisopa, ver, (op1 >> 5) & 0x3);

                            for (i = 1; i <= +registros[MBR]; i++){                                                                                // en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                                primerbit = leerMemoria32(memoria, direfisopa, ver, (op1 >> 5) & 0x3) >> 31; // agarro el primer bit y me guardo su valor, si es uno y se hace shiftleft habre carreo
                                valorDeA <<= 1;
                                int bitAntes = (clonsinsigno >> 31) & 1;
                                clonsinsigno <<= 1;
                                clonconsigno <<= 1;
                                int bitDespues = (clonsinsigno >> 31) & 1;

                                if (bitAntes != bitDespues)
                                    desbordamiento = 1;
                            }
                            escribirMemoria32(memoria, direfisopa, valorDeA, ver, (op1 >> 5) & 0x3);

                            registros[CC] = 0; // limpio CC
                            if ((clonsinsigno >> 32) != 0)
                            {
                                registros[CC] |= (uint32_t)1 << 29;
                            }
                            if (desbordamiento)
                            {
                                registros[CC] |= (uint32_t)1 << 28;
                            }

                            registros[CC] |= (uint32_t)(leerMemoria32(memoria, direfisopa, ver, (op1 >> 5) & 0x3) < 0) << 31;  // NEGATIVO?
                            registros[CC] |= (uint32_t)(leerMemoria32(memoria, direfisopa, ver, (op1 >> 5) & 0x3) == 0) << 30; // CERO?
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
                else if (op2 >> 24 == 1){ // segundo op de registro
                    int valorDeA = leerMemoria32(memoria, direfisopa, ver, (op1 >> 5) & 0x3);
                    registros[CC] = 0; // limpio CC
                    int tope= leerRegistro(registros, op2 & 0x1F, ver);

                    for (i = 1; i <= tope; i++){                                                                                // en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                        primerbit = leerMemoria32(memoria, direfisopa, ver, (op1 >> 5) & 0x3) >> 31; // agarro el primer bit y me guardo su valor, si es uno y se hace shiftleft habre carreo
                        valorDeA <<= 1;
                        int bitAntes = (clonsinsigno >> 31) & 1;
                        clonsinsigno <<= 1;
                        clonconsigno <<= 1;
                        int bitDespues = (clonsinsigno >> 31) & 1;
                        if (bitAntes != bitDespues)
                            desbordamiento = 1;
                    }
                    escribirMemoria32(memoria, direfisopa, valorDeA, ver, (op1 >> 5) & 0x3);
                    if ((clonsinsigno >> 32) != 0)
                    {
                        registros[CC] |= (uint32_t)1 << 29;
                    }
                    if (desbordamiento)
                    {
                        registros[CC] |= (uint32_t)1 << 28;
                    }
                    registros[CC] |= (uint32_t)(leerMemoria32(memoria, direfisopa, ver, (op1 >> 5) & 0x3) < 0) << 31;  // NEGATIVO?
                    registros[CC] |= (uint32_t)(leerMemoria32(memoria, direfisopa, ver, (op1 >> 5) & 0x3) == 0) << 30; // CERO?
                }
                else{ // segundo op inmediato
                    int valorDeA = leerMemoria32(memoria, direfisopa, ver, (op1 >> 5) & 0x3);
                    for (i = 1; i <= (op2 & 0xFFFFFF); i++)
                    {                                                                                // en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                        primerbit = leerMemoria32(memoria, direfisopa, ver, (op1 >> 5) & 0x3) >> 31; // agarro el primer bit y me guardo su valor, si es uno y se hace shiftleft habre carreo
                        valorDeA <<= 1;
                        int bitAntes = (clonsinsigno >> 31) & 1;
                        clonsinsigno <<= 1;
                        clonconsigno <<= 1;
                        int bitDespues = (clonsinsigno >> 31) & 1;

                        if (bitAntes != bitDespues)
                            desbordamiento = 1;
                    }
                    escribirMemoria32(memoria, direfisopa, valorDeA, ver, (op1 >> 5) & 0x3);
                    registros[CC] = 0; // limpio CC
                    if ((clonsinsigno >> 32) != 0)
                    {
                        registros[CC] |= (uint32_t)1 << 29;
                    }
                    if (desbordamiento)
                    {
                        registros[CC] |= (uint32_t)1 << 28;
                    }
                    registros[CC] |= (uint32_t)(leerMemoria32(memoria, direfisopa, ver, (op1 >> 5) & 0x3) < 0) << 31;  // NEGATIVO?
                    registros[CC] |= (uint32_t)(leerMemoria32(memoria, direfisopa, ver, (op1 >> 5) & 0x3) == 0) << 30; // CERO?
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
    else{ // primer op de registro
        clonconsigno = registros[op1 & 0X1F];
        clonsinsigno = (unsigned int)registros[op1 & 0X1F];

        if (op2 >> 24 == 3){ // segundo op de memoria
            if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){// pregunto si el codigo de segmento es valido
                cargarLAR(op2, registros, tabla); // guardo la dir logica
                // en la parte del MAR cuantos bytes:
                registros[MAR] = 4 << 16; // creo q son 4 bytes porq leemos numeros(?)
                // traducir a dir fisica, guardarla en la parte baja del MBR, reviso si no se cae del segmento:
                registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
                if (validoDirFisica(op2, registros, tabla)){
                    // guardar en el MBR el valor:
                    registros[MBR] = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op2 >> 5) & 0x3);
                    direfisopb = registros[MAR] & 0xFFFF;

                    // registros[op1 & 0x1F] (primer operando)

                    for (i = 1; i <= registros[MBR]; i++){  // en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                        primerbit = leerRegistro(registros, op1 & 0x1F, ver) >> 31; // agarro el primer bit y me guardo su valor, si es uno y se hace shiftleft habre carreo
                        escribirRegistro(registros, op1&0x1F, leerRegistro(registros, op1 & 0x1F, ver)<<1, ver);
                        int bitAntes = (clonsinsigno >> 31) & 1;
                        clonsinsigno <<= 1;
                        clonconsigno <<= 1;
                        int bitDespues = (clonsinsigno >> 31) & 1;

                        if (bitAntes != bitDespues)
                            desbordamiento = 1;
                    }
                    registros[CC] = 0; // limpio CC
                    if ((clonsinsigno >> 32) != 0)
                    {
                        registros[CC] |= (uint32_t)1 << 29;
                    }
                    if (desbordamiento)
                    {
                        registros[CC] |= (uint32_t)1 << 28;
                    }
                    registros[CC] |= (uint32_t)(registros[op1 & 0x1F] < 0) << 31;  // NEGATIVO?
                    registros[CC] |= (uint32_t)(registros[op1 & 0x1F] == 0) << 30; // CERO?
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
        else if (op2 >> 24 == 1){ // segundo op de registro

            for (i = 1; i <= registros[op2 & 0x1F]; i++){// en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                primerbit = leerRegistro(registros, op1 & 0x1F, ver) >> 31; // agarro el primer bit y me guardo su valor, si es uno y se hace shiftleft habre carreo
                escribirRegistro(registros, op1&0x1F, leerRegistro(registros, op1 & 0x1F, ver)<<1, ver);
                int bitAntes = (clonsinsigno >> 31) & 1;
                clonsinsigno <<= 1;
                clonconsigno <<= 1;
                int bitDespues = (clonsinsigno >> 31) & 1;

                if (bitAntes != bitDespues)
                    desbordamiento = 1;
            }
            registros[CC] = 0; // limpio CC
            if ((clonsinsigno >> 32) != 0)
            {
                registros[CC] |= (uint32_t)1 << 29;
            }
            if (desbordamiento)
            {
                registros[CC] |= (uint32_t)1 << 28;
            }
            registros[CC] |= (uint32_t)(registros[op1 & 0x1F] < 0) << 31;  // NEGATIVO?
            registros[CC] |= (uint32_t)(registros[op1 & 0x1F] == 0) << 30; // CERO?
            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "SHL");
        }
        else{ // segundo op inmediato

            for (i = 1; i <= (op2 & 0xFFFFFF); i++){// en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                primerbit = leerRegistro(registros, op1 & 0x1F, ver) >> 31; // agarro el primer bit y me guardo su valor, si es uno y se hace shiftleft habre carreo
                escribirRegistro(registros, op1&0x1F, leerRegistro(registros, op1 & 0x1F, ver)<<1, ver);
                int bitAntes = (clonsinsigno >> 31) & 1;
                clonsinsigno <<= 1;
                clonconsigno <<= 1;
                int bitDespues = (clonsinsigno >> 31) & 1;

                if (bitAntes != bitDespues)
                    desbordamiento = 1;
            }
            registros[CC] = 0; // limpio CC
            if ((clonsinsigno >> 32) != 0)
            {
                registros[CC] |= (uint32_t)1 << 29;
            }
            if (desbordamiento)
            {
                registros[CC] |= (uint32_t)1 << 28;
            }
            registros[CC] |= (uint32_t)(registros[op1 & 0x1F] < 0) << 31;  // NEGATIVO?
            registros[CC] |= (uint32_t)(registros[op1 & 0x1F] == 0) << 30; // CERO?
            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "SHL");
        }
    }
}

void SHR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){
    int i, direfisopa, direfisopb;
    uint64_t clonsinsigno;
    clonsinsigno = 0; // en SHR no hay overflow

    if (op1 >> 24 == 3){ // primer op de memoria
        if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){// pregunto si el codigo de segmento es valido
            cargarLAR(op1, registros, tabla); // guardo la dir logica
            // en la parte del MAR cuantos bytes:
            registros[MAR] = 4 << 16; // creo q son 4 bytes porq leemos numeros(?)
            // traducir a dir fisica, guardarla en la parte baja del MBR, reviso si no se cae del segmento:
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op1, registros, tabla)){
                // guardar en el MBR el valor:
                registros[MBR] = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op1 >> 5) & 0x3);
                direfisopa = registros[MAR] & 0xFFFF;

                uint32_t valorDeA = registros[MBR];
                if (op2 >> 24 == 3){ // segundo op de memoria

                    if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){// pregunto si el codigo de segmento es valido
                        cargarLAR(op2, registros, tabla); // guardo la dir logica
                        // en la parte del MAR cuantos bytes:
                        registros[MAR] = 4 << 16; // creo q son 4 bytes porq leemos numeros(?)
                        // traducir a dir fisica, guardarla en la parte baja del MBR, reviso si no se cae del segmento:
                        registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
                        if (validoDirFisica(op2, registros, tabla)){
                            // guardar en el MBR el valor:
                            registros[MBR] = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op2 >> 5) & 0x3);

                            for (i = 1; i <= +registros[MBR]; i++){ // en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                                valorDeA >>= 1;
                                clonsinsigno >>= 1;
                            }
                            escribirMemoria32(memoria, direfisopa, valorDeA, ver, (op1 >> 5) & 0x3);
                            registros[CC] = 0; // limpio CC
                            if ((clonsinsigno & 0XFFFFFFFF) != 0)
                            {
                                registros[CC] |= (uint32_t)1 << 29;
                            }

                            registros[CC] |= (uint32_t)(leerMemoria32(memoria, direfisopa, ver, (op1 >> 5) & 0x3) < 0) << 31;  // NEGATIVO?
                            registros[CC] |= (uint32_t)(leerMemoria32(memoria, direfisopa, ver, (op1 >> 5) & 0x3) == 0) << 30; // CERO?
                            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "SHR");
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
                else if (op2 >> 24 == 1){ // segundo op de registro

                    for (i = 1; i <= leerRegistro(registros, op2 & 0x1F, ver); i++){ // en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                        valorDeA >>= 1;
                        clonsinsigno >>= 1;
                    }
                    escribirMemoria32(memoria, direfisopa, valorDeA, ver, (op1 >> 5) & 0x3);
                    registros[CC] = 0; // limpio CC
                    if ((clonsinsigno & 0XFFFFFFFF) != 0)
                    {
                        registros[CC] |= (uint32_t)1 << 29;
                    }
                    registros[CC] |= (uint32_t)(leerMemoria32(memoria, direfisopa, ver, (op1 >> 5) & 0x3) < 0) << 31;  // NEGATIVO?
                    registros[CC] |= (uint32_t)(leerMemoria32(memoria, direfisopa, ver, (op1 >> 5) & 0x3) == 0) << 30; // CERO?
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "SHR");
                }
                else{ // segundo op inmediato

                    for (i = 1; i <= (op2 & 0xFFFFFF); i++){ // en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                        valorDeA >>= 1;
                        clonsinsigno >>= 1;
                    }
                    escribirMemoria32(memoria, direfisopa, valorDeA, ver, (op1 >> 5) & 0x3);
                    registros[CC] = 0; // limpio CC
                    if ((clonsinsigno & 0XFFFFFFFF) | 0){
                        registros[CC] |= (uint32_t)1 << 29;
                    }
                    registros[CC] |= (uint32_t)(leerMemoria32(memoria, direfisopa, ver, (op1 >> 5) & 0x3) < 0) << 31;  // NEGATIVO?
                    registros[CC] |= (uint32_t)(leerMemoria32(memoria, direfisopa, ver, (op1 >> 5) & 0x3) == 0) << 30; // CERO?
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "SHR");
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
    else{ // primer op de registro
        clonsinsigno = registros[op1 & 0X1F];

        if (op2 >> 24 == 3){ // segundo op de memoria
            if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){// pregunto si el codigo de segmento es valido
                cargarLAR(op2, registros, tabla); // guardo la dir logica
                // en la parte del MAR cuantos bytes:
                registros[MAR] = 4 << 16; // creo q son 4 bytes porq leemos numeros(?)
                // traducir a dir fisica, guardarla en la parte baja del MBR, reviso si no se cae del segmento:
                registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
                if (validoDirFisica(op2, registros, tabla)){
                    // guardar en el MBR el valor:
                    registros[MBR] = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op2 >> 5) & 0x3);
                    direfisopb = registros[MAR] & 0xFFFF;

                    // registros[op1 & 0x1F] (primer operando)

                    for (i = 1; i <= registros[MBR]; i++){ // en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                        escribirRegistro(registros, op1&0x1F, (uint32_t)leerRegistro(registros, op1 & 0x1F, ver)>>1, ver);
                        clonsinsigno >>= 1;
                    }
                    registros[CC] = 0; // limpio CC
                    if ((clonsinsigno & 0XFFFFFFFF) != 0){
                        registros[CC] |= (uint32_t)1 << 29;
                    }
                    registros[CC] |= (uint32_t)(registros[op1 & 0x1F] < 0) << 31;  // NEGATIVO?
                    registros[CC] |= (uint32_t)(registros[op1 & 0x1F] == 0) << 30; // CERO?
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
        else if (op2 >> 24 == 1){ // segundo op de registro

            for (i = 1; i <= leerRegistro(registros, op2 & 0x1F, ver); i++){ // en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                escribirRegistro(registros, op1&0x1F, (uint32_t)leerRegistro(registros, op1 & 0x1F, ver)>>1, ver);
                clonsinsigno >>= 1;
            }
            registros[CC] = 0; // limpio CC
            if ((clonsinsigno & 0XFFFFFFFF) != 0){
                registros[CC] |= (uint32_t)1 << 29;
            }
            registros[CC] |= (uint32_t)(registros[op1 & 0x1F] < 0) << 31;  // NEGATIVO?
            registros[CC] |= (uint32_t)(registros[op1 & 0x1F] == 0) << 30; // CERO?
        }
        else{ // segundo op inmediato

            for (i = 1; i <= (op2 & 0xFFFFFF); i++){ // en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                escribirRegistro(registros, op1&0x1F, (uint32_t)leerRegistro(registros, op1 & 0x1F, ver)>>1, ver);
                clonsinsigno >>= 1;
            }
            registros[CC] = 0; // limpio CC
            if ((clonsinsigno & 0XFFFFFFFF) != 0){
                registros[CC] |= (uint32_t)1 << 29;
            }
            registros[CC] |= (uint32_t)(registros[op1 & 0x1F] < 0) << 31;  // NEGATIVO?
            registros[CC] |= (uint32_t)(registros[op1 & 0x1F] == 0) << 30; // CERO?
        }
    }
}
void SAR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){
    int i, direfisopa, direfisopb, bitdesigno;
    uint64_t clonsinsigno;
    clonsinsigno = 0; // en SHR no hay overflow

    if (op1 >> 24 == 3){ // primer op de memoria
        if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){ // pregunto si el codigo de segmento es valido
            cargarLAR(op1, registros, tabla); // guardo la dir logica
            // en la parte del MAR cuantos bytes:
            registros[MAR] = 4 << 16; // creo q son 4 bytes porq leemos numeros(?)
            // traducir a dir fisica, guardarla en la parte baja del MBR, reviso si no se cae del segmento:
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op1, registros, tabla)){
                // guardar en el MBR el valor:
                registros[MBR] = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op1 >> 5) & 0x3);
                direfisopa = registros[MAR] & 0xFFFF;
                bitdesigno = memoria[direfisopa] >> 31;

                int valorA = registros[MBR];
                if (op2 >> 24 == 3){ // segundo op de memoria

                    if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){                                     // pregunto si el codigo de segmento es valido
                        cargarLAR(op2, registros, tabla); // guardo la dir logica
                        // en la parte del MAR cuantos bytes:
                        registros[MAR] = 4 << 16; // creo q son 4 bytes porq leemos numeros(?)
                        // traducir a dir fisica, guardarla en la parte baja del MBR, reviso si no se cae del segmento:
                        registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
                        if (validoDirFisica(op2, registros, tabla)){
                            // guardar en el MBR el valor:
                            registros[MBR] = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op2 >> 5) & 0x3);

                            for (i = 1; i <= +registros[MBR]; i++){ // en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                                valorA >>= 1;
                                valorA |= bitdesigno << 31;
                                clonsinsigno >>= 1;
                            }
                            escribirMemoria32(memoria, direfisopa, valorA, ver, (op1 >> 5) & 0x3);
                            registros[CC] = 0; // limpio CC
                            if ((clonsinsigno & 0XFFFFFFFF) != 0)
                            {
                                registros[CC] |= (uint32_t)1 << 29;
                            }

                            registros[CC] |= (uint32_t)(leerMemoria32(memoria, direfisopa, ver, (op1 >> 5) & 0x3) < 0) << 31;  // NEGATIVO?
                            registros[CC] |= (uint32_t)(leerMemoria32(memoria, direfisopa, ver, (op1 >> 5) & 0x3) == 0) << 30; // CERO?
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
                else if (op2 >> 24 == 1){ // segundo op de registro

                    for (i = 1; i <= leerRegistro(registros, op2 & 0x1F, ver); i++){ // en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                        valorA >>= 1;
                        valorA |= bitdesigno << 31;
                        clonsinsigno >>= 1;
                    }
                    escribirMemoria32(memoria, registros[MAR] & 0xFFFF, valorA, ver, (op1 >> 5) & 0x3);
                    registros[CC] = 0; // limpio CC
                    if ((clonsinsigno & 0XFFFFFFFF) != 0){
                        registros[CC] |= (uint32_t)1 << 29;
                    }
                    registros[CC] |= (uint32_t)(leerMemoria32(memoria, direfisopa, ver, (op1 >> 5) & 0x3) < 0) << 31;  // NEGATIVO?
                    registros[CC] |= (uint32_t)(leerMemoria32(memoria, direfisopa, ver, (op1 >> 5) & 0x3) == 0) << 30; // CERO?
                }
                else{ // segundo op inmediato

                    for (i = 1; i <= (op2 & 0xFFFFFF); i++){ // en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                        valorA >>= 1;
                        valorA |= bitdesigno << 31;
                        clonsinsigno >>= 1;
                    }
                    escribirMemoria32(memoria, registros[MAR] & 0xFFFF, valorA, ver, (op1 >> 5) & 0x3);
                    registros[CC] = 0; // limpio CC
                    if ((clonsinsigno & 0XFFFFFFFF) != 0){
                        registros[CC] |= (uint32_t)1 << 29;
                    }
                    registros[CC] |= (uint32_t)(leerMemoria32(memoria, direfisopa, ver, (op1 >> 5) & 0x3) < 0) << 31;  // NEGATIVO?
                    registros[CC] |= (uint32_t)(leerMemoria32(memoria, direfisopa, ver, (op1 >> 5) & 0x3) == 0) << 30; // CERO?
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
    else{ // primer op de registro
        clonsinsigno = registros[op1 & 0X1F];
        bitdesigno = registros[op1 & 0X1F] >> 31;

        if (op2 >> 24 == 3){ // segundo op de memoria
            if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){ // pregunto si el codigo de segmento es valido
                cargarLAR(op2, registros, tabla); // guardo la dir logica
                // en la parte del MAR cuantos bytes:
                registros[MAR] = 4 << 16; // creo q son 4 bytes porq leemos numeros(?)
                // traducir a dir fisica, guardarla en la parte baja del MBR, reviso si no se cae del segmento:
                registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
                if (validoDirFisica(op2, registros, tabla)){
                    // guardar en el MBR el valor:
                    registros[MBR] = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op1 >> 5) & 0x3);
                    direfisopb = registros[MAR] & 0xFFFF;

                    // registros[op1 & 0x1F] (primer operando)

                    for (i = 1; i <= registros[MBR]; i++){ // en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                        
                        escribirRegistro(registros, op1&0x1F, (uint32_t)leerRegistro(registros, op1 & 0x1F, ver)>>1, ver);
                        uint32_t dest= (uint32_t)leerRegistro(registros, op1 & 0x1F, ver);
                        escribirRegistro(registros, op1&0x1F, dest|= bitdesigno << 31, ver);
                        clonsinsigno >>= 1;
                    }
                    registros[CC] = 0; // limpio CC
                    if ((clonsinsigno & 0XFFFFFFFF) != 0){
                        registros[CC] |= (uint32_t)1 << 29;
                    }
                    registros[CC] |= (uint32_t)(registros[op1 & 0x1F] < 0) << 31;  // NEGATIVO?
                    registros[CC] |= (uint32_t)(registros[op1 & 0x1F] == 0) << 30; // CERO?
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
        else if (op2 >> 24 == 1){ // segundo op de registro

            for (i = 1; i <= leerRegistro(registros, op2 & 0x1F, ver); i++){ // en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                escribirRegistro(registros, op1&0x1F, (uint32_t)leerRegistro(registros, op1 & 0x1F, ver)>>1, ver);
                        uint32_t dest= (uint32_t)leerRegistro(registros, op1 & 0x1F, ver);
                        escribirRegistro(registros, op1&0x1F, dest|= bitdesigno << 31, ver);
                clonsinsigno >>= 1;
            }
            registros[CC] = 0; // limpio CC
            if ((clonsinsigno & 0XFFFFFFFF) | 0){
                registros[CC] |= (uint32_t)1 << 29;
            }
            registros[CC] |= (uint32_t)(registros[op1 & 0x1F] < 0) << 31;  // NEGATIVO?
            registros[CC] |= (uint32_t)(registros[op1 & 0x1F] == 0) << 30; // CERO?
            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "SHL");
        }
        else{ // segundo op inmediato

            for (i = 1; i <= (op2 & 0xFFFFFF); i++){ // en cada iteracion pregunto por desbordamiento y Acarreo, ademas de irle haciendo el shift
                escribirRegistro(registros, op1&0x1F, (uint32_t)leerRegistro(registros, op1 & 0x1F, ver)>>1, ver);
                uint32_t dest= (uint32_t)leerRegistro(registros, op1 & 0x1F, ver);
                escribirRegistro(registros, op1&0x1F, dest|= bitdesigno << 31, ver);
                clonsinsigno >>= 1;
            }
            registros[CC] = 0; // limpio CC
            if ((clonsinsigno & 0XFFFFFFFF) != 0){
                registros[CC] |= (uint32_t)1 << 29;
            }
            registros[CC] |= (uint32_t)(registros[op1 & 0x1F] < 0) << 31;  // NEGATIVO?
            registros[CC] |= (uint32_t)(registros[op1 & 0x1F] == 0) << 30; // CERO?
            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "SHL");
        }
    }
}
void LDL(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){
    int32_t valor_fuente = 0;
    int32_t valor_destino = 0;

    // 1. Obtener fuente (op2)
    if (op2 >> 24 == 3){
        cargarLAR(op2, registros, tabla);
        registros[MAR] = (4 << 16) | (tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF));
        if (validoDirFisica(op2, registros, tabla)){
            valor_fuente = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op2 >> 5) & 0x3);
        }
        else{
            printf("FALLO DE SEGMENTO\n");
            registros[IP] = -1;
            return;
        }
    }
    else if (op2 >> 24 == 1){
        valor_fuente = leerRegistro(registros, op2 & 0x1F, ver);
    }
    else{
        valor_fuente = op2 & 0xFFFFFF;
    }

    // 2. Modificar destino (op1)
    if (op1 >> 24 == 3){
        cargarLAR(op1, registros, tabla);
        registros[MAR] = (4 << 16) | (tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF));
        if (validoDirFisica(op1, registros, tabla)){
            valor_destino = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op1 >> 5) & 0x3);

            // Conserva la parte alta (0xFFFF0000) e inserta la fuente en la parte baja
            valor_destino = (valor_destino & 0xFFFF0000) | (valor_fuente & 0xFFFF);

            escribirMemoria32(memoria, registros[MAR] & 0xFFFF, valor_destino, ver, (op1 >> 5) & 0x3);
            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "LDL");
        }
        else{
            printf("FALLO DE SEGMENTO\n");
            registros[IP] = -1;
            return;
        }
    }
    else if (op1 >> 24 == 1){
        valor_destino = leerRegistro(registros, op1 & 0x1F, ver);
        valor_destino = (valor_destino & 0xFFFF0000) | (valor_fuente & 0xFFFF);
        escribirRegistro(registros, op1&0x1F, valor_destino, ver);
        disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "LDL");
    }
}
void LDH(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){
    int32_t valor_fuente = 0;
    int32_t valor_destino = 0;

    // 1. Obtener fuente (op2)
    if (op2 >> 24 == 3){
        cargarLAR(op2, registros, tabla);
        registros[MAR] = (4 << 16) | (tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF));
        if (validoDirFisica(op2, registros, tabla)){
            valor_fuente = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op2 >> 5) & 0x3);
        }
        else{
            printf("FALLO DE SEGMENTO\n");
            registros[IP] = -1;
            return;
        }
    }
    else if (op2 >> 24 == 1){
        valor_fuente = leerRegistro(registros, op2 & 0x1F, ver);
    }
    else{
        valor_fuente = op2 & 0xFFFFFF;
    }

    // 2. Modificar destino (op1)
    if (op1 >> 24 == 3){
        cargarLAR(op1, registros, tabla);
        registros[MAR] = (4 << 16) | (tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF));
        if (validoDirFisica(op1, registros, tabla)){
            valor_destino = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op1 >> 5) & 0x3);

            // Conserva la parte baja (0x0000FFFF) e inserta la fuente en la parte alta
            valor_destino = (valor_destino & 0x0000FFFF) | ((valor_fuente & 0xFFFF) << 16);

            escribirMemoria32(memoria, registros[MAR] & 0xFFFF, valor_destino, ver, (op1 >> 5) & 0x3);
            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "LDH");
        }
        else{
            printf("FALLO DE SEGMENTO\n");
            registros[IP] = -1;
            return;
        }
    }
    else if (op1 >> 24 == 1){
        valor_destino = leerRegistro(registros, op1 & 0x1F, ver);
        valor_destino = (valor_destino & 0x0000FFFF) | ((valor_fuente & 0xFFFF) << 16);
        escribirRegistro(registros, op1&0x1F, valor_destino, ver);
        disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "LDH");
    }
}
void RND(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char *nomRegistro[32], int ver,int *enter, char *archivo_vmi){

    if (op1 >> 24 == 3){ // primer op de memoria
        if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){ // pregunto si el codigo de segmento es valido
            cargarLAR(op1, registros, tabla); // guardo la dir logica
            // en la parte del MAR cuantos bytes:
            registros[MAR] = 4 << 16; // creo q son 4 bytes porq leemos numeros(?)
            // traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
            registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
            if (validoDirFisica(op1, registros, tabla)){
                // guardar en el MbR el valor:
                registros[MBR] = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op1 >> 5) & 0x3);
                if (op2 >> 24 == 3){ // segundo op de memoria
                    int dirOpA = registros[MAR] & 0xFFFF;

                    if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){ // pregunto si el codigo de segmento es valido
                        cargarLAR(op2, registros, tabla); // guardo la dir logica
                        // en la parte del MAR cuantos bytes:
                        registros[MAR] = 4 << 16; // creo q son 4 bytes porq leemos numeros(?)
                        // traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
                        registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
                        if (validoDirFisica(op2, registros, tabla)){
                            // guardar en el MbR el valor:
                            registros[MBR] = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op2 >> 5) & 0x3);
                            escribirMemoria32(memoria, dirOpA, rand() % registros[MBR], ver, (op1 >> 5) & 0x3);
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
                }
                else if (op2 >> 24 == 1){ // segundo op de registro
                    escribirMemoria32(memoria, registros[MAR] & 0xFFFF, rand() % leerRegistro(registros, op2 & 0x1F, ver), ver, (op1 >> 5) & 0x3);
                    disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "RND");
                }
                else{ // segundo op inmediato
                    escribirMemoria32(memoria, registros[MAR] & 0xFFFF, rand() % (op2 & 0xFFFFFF), ver, (op1 >> 5) & 0x3);
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
    else{ // primer op de registro
        if (op2 >> 24 == 3){ // segundo op de memoria
            if (tabla[(registros[op1 & 0x1F]) >> 16][0] != -1){// pregunto si el codigo de segmento es valido
                cargarLAR(op2, registros, tabla); // guardo la dir logica
                // en la parte del MAR cuantos bytes:
                registros[MAR] = 4 << 16; // creo q son 4 bytes porq leemos numeros(?)
                // traducir a dir fisica, guardarla en la parte baja del MbR, reviso si no se cae del segmento:
                registros[MAR] |= tabla[registros[LAR] >> 16][0] + (registros[LAR] & 0xFFFF);
                if (validoDirFisica(op2, registros, tabla)){
                    // guardar en el MbR el valor:
                    registros[MBR] = leerMemoria32(memoria, registros[MAR] & 0xFFFF, ver, (op2 >> 5) & 0x3);
                    escribirRegistro(registros, op1&0x1F,rand() % registros[MBR], ver);
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
        }
        else if (op2 >> 24 == 1){ // segundo op de registro
            escribirRegistro(registros, op1&0x1F,  rand() %leerRegistro(registros, op2 & 0x1F, ver), ver);
            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "RND");
        }
        else{ // segundo op inmediato
            escribirRegistro(registros, op1&0x1F,  rand() %(op2 & 0xFFFFFF), ver);
            disassembler(flag, 2, op1, op2, nomRegistro, IPant, memoria, registros, "RND");
        }
    }
}