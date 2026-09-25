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

void SYS(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void STOP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char* nomRegistro[32]);
void JMP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void JP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void JN(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void JZ(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void JC(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void JV(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void JNP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void JNN(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void JNZ(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void NOT(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void B(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void C(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void D(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void E(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void MOV(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void ADD(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void SUB(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void MUL(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void DIV(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void CMP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2], int IPant, char* nomRegistro[32]);
void AND(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void OR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void XOR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void SWAP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void SHL(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void SHR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void SAR(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void LDL(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void LDH(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void RND(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[8][2],int IPant, char* nomRegistro[32]);
void imprimir_binario(int valor, int tam_bytes);
void cambiarCC(int32_t valor, int carry, int overflow, int registros[REGISTROS]);
void disassembler(int flag, int tOp, int op1, int op2, char *nomRegistro[32],int IPant, char memoria[MEMORIA], int registros[MBR], char *funcion);
void imprimirOperando(int ,int op, char* nomRegistro[32], int registros[REGISTROS]);
void cargarLAR( int op,int registros[REGISTROS], short int tabla[8][2]);
int validoDirFisica(int op,int registros[REGISTROS], short int tabla[8][2]);
