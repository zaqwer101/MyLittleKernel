section .text
        align 4
        dd 0x1BADB002            ;магические числа
        dd 0x00                  
        dd - (0x1BADB002 + 0x00) 
 
global start
extern kmain         ;
 
start:
  cli ;блокировка прерываний
  mov esp, stack_space ;указатель стека
  call kmain
  hlt ;остановка процессора
 
section .bss
resb 8192 ;8KB на стек
stack_space:
