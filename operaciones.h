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

void STOP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8], int IPant, char* nomRegistro[32]);
void JMP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]);
void JP(int op1, int op2, int flag, char memoria[MEMORIA], int registros[REGISTROS], short int tabla[2][8],int IPant, char* nomRegistro[32]);
void JN(int, int, int, char, int, short int t,int IPant, char* nomRegistro[32]);
void JZ(int, int, int, char, int, short int t,int IPant, char* nomRegistro[32]);
void JC(int, int, int, char, int, short int t,int IPant, char* nomRegistro[32]);
void JV(int, int, int, char, int, short int t,int IPant, char* nomRegistro[32]);
void JNP(int, int, int, char, int, short int t,int IPant, char* nomRegistro[32]);
void JNN(int, int, int, char, int, short int t,int IPant, char* nomRegistro[32]);
void JNZ(int, int, int, char, int, short int t,int IPant, char* nomRegistro[32]);
void NOT(int, int, int, char, int, short int t,int IPant, char* nomRegistro[32]);
void B(int, int, int, char, int, short int t,int IPant, char* nomRegistro[32]);
void C(int, int, int, char, int, short int t,int IPant, char* nomRegistro[32]);
void D(int, int, int, char, int, short int t,int IPant, char* nomRegistro[32]);
void E(int, int, int, char, int, short int t,int IPant, char* nomRegistro[32]);
void MOV(int, int, int, char, int, short int t,int IPant, char* nomRegistro[32]);
void ADD(int, int, int, char, int, short int t,int IPant, char* nomRegistro[32]);
void SUB(int, int, int, char, int, short int t,int IPant, char* nomRegistro[32]);
void MUL(int, int, int, char, int, short int t,int IPant, char* nomRegistro[32]);
void DIV(int, int, int, char, int, short int t,int IPant, char* nomRegistro[32]);
void AND(int, int, int, char, int, short int t,int IPant, char* nomRegistro[32]);
void OR(int, int, int, char, int, short int t,int IPant, char* nomRegistro[32]);
void XOR(int, int, int, char, int, short int t,int IPant, char* nomRegistro[32]);
void SWAP(int, int, int, char, int, short int t,int IPant, char* nomRegistro[32]);
void SHL(int, int, int, char, int, short int t,int IPant, char* nomRegistro[32]);
void SHR(int, int, int, char, int, short int t,int IPant, char* nomRegistro[32]);
void SAR(int, int, int, char, int, short int t,int IPant, char* nomRegistro[32]);
void LDL(int, int, int, char, int, short int t,int IPant, char* nomRegistro[32]);
void LDH(int, int, int, char, int, short int t,int IPant, char* nomRegistro[32]);
void RND(int, int, int, char, int, short int t,int IPant, char* nomRegistro[32]);
