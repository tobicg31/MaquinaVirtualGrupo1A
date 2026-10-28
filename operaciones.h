#include <stdint.h>
#define MEMORIA 16384
#define REGISTROS 32
#define IP 0
#define OPC 1
#define OP1 2
#define OP2 3
#define LAR 4
#define MAR 5
#define MBR 6
#define SP 7
#define BP 8
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
#define ES 28
#define SS 29
#define KS 30
#define PS 31

void SYS(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);
void STOP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char* nomRegistro[32], int ver, int *, char *);
void JMP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);
void JP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);
void JN(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);
void JZ(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);
void JC(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);
void JV(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);
void JNP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);
void JNN(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);
void JNZ(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);
void NOT(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);
void PUSH(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);
void POP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);
void CALL(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);
void RET(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);
void MOV(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);
void ADD(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);
void SUB(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);
void MUL(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);

void DIV(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);

void CMP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char* nomRegistro[32], int ver, int *, char *);
void AND(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);
void OR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);
void XOR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);
void SWAP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);
void SHL(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);
void SHR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);
void SAR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);
void LDL(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);
void LDH(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);
void RND(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, char *);
void imprimir_binario(int valor, int tam_bytes);
void cambiarCC(int32_t valor, int carry, int overflow, int registros[REGISTROS]);
void disassembler(int flag, int tOp, int op1, int op2, char *nomRegistro[32],int IPant, char memoria[MEMORIA], int registros[MBR], char *funcion, char cadena[100], int ver);
void Dissasembler2(char memoria[MEMORIA], short int tabla[8][2], int registros[REGISTROS], short int tamCodigo, short int tamConst, char *nomRegistro[32]);
void imprimirOperando(int ,int op, char* nomRegistro[32], char cadena[100], int ver);
void cargarLAR( int op,int registros[REGISTROS], short int tabla[8][2]);
int validoDirFisica(int op,int registros[REGISTROS], short int tabla[8][2]);
int32_t leerMemoria32(char memoria[MEMORIA], int dir, int ver, int mod);
void escribirMemoria32(char memoria[MEMORIA], int dir, int32_t valor, int ver, int mod);
void hacebreakpoint(char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],char *archivo_vmi);
int dirFisica(int dir, short int tabla[8][2], int bytes);
int32_t leerRegistro(int registros[REGISTROS], int regis, int ver);
void escribirRegistro(int registros[REGISTROS], int regis, int valor, int ver);
void DesensamblarEstatico(char memoria[MEMORIA], short int tamCodigo, char* nomRegistro[32], int ver);