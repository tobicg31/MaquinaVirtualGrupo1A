\\extra 1000
\\stack 50
main: mov edx,DS
      mov [edx],23
      push edx
      LDH ecx,04
      LDL ecx,01
      MOV eax,0x08
      mov edx,10
      pop edx
      sys 2
      ret