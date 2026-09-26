MOV     EBX,    1
shl     EBX,    31
MOV     [0],    EBX
MOV     [4],    CC
MOV     EDX,    DS
ADD     EDX,    0
LDH     ECX,    4
LDL     ECX,    1
MOV     EAX,    1
SYS     0x2
MOV     EDX,    DS
ADD     EDX,    4
LDH     ECX,    4
LDL     ECX,    1
MOV     EAX,    0x18
SYS     0x2

STOP