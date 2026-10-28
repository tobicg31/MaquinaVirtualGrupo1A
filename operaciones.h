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

void SYS(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void STOP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void JMP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void JP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void JN(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void JZ(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void JC(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void JV(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void JNP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void JNN(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void JNZ(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void NOT(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void PUSH(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void POP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void CALL(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void RET(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void MOV(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void ADD(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void SUB(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void MUL(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);

void DIV(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);

void CMP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void AND(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void OR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void XOR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void SWAP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void SHL(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void SHR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void SAR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void LDL(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void LDH(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void RND(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32], int ver, int *, FILE *);
void imprimir_binario(int valor, int tam_bytes);
void cambiarCC(int32_t valor, int carry, int overflow, int registros[REGISTROS]);
void disassembler(int flag, int tOp, int op1, int op2, char *nomRegistro[32],int IPant, char memoria[MEMORIA], int registros[MBR], char *funcion, char cadena[100], int ver);
void imprimirOperando(int ,int op, char* nomRegistro[32], char cadena[100], int ver);
void cargarLAR( int op,int registros[REGISTROS], short int tabla[8][2]);
int validoDirFisica(int op,int registros[REGISTROS], short int tabla[8][2]);
int32_t leerMemoria32(char memoria[MEMORIA], int dir, int ver, int mod);
void escribirMemoria32(char memoria[MEMORIA], int dir, int32_t valor, int ver, int mod);
void hacebreakpoint(char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],char *archivo_vmi);
int dirFisica(int dir, short int tabla[8][2], int bytes);
