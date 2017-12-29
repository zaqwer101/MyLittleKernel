section .text
        align 4
        dd 0x1BADB002           										; магические числа
        dd 0x00                  
        dd - (0x1BADB002 + 0x00) 
 
global start															; определяем NASM-функции, к которым будем обращаться в СИ-коде
global keyboard_handler
global read_port
global write_port
global load_idt

extern kmain         													; загружаем функции из СИ-кода
extern keyboard_handler_main
 
read_port:
	mov edx, [esp + 4]
	in al, dx
	ret

write_port:
	mov   edx, [esp + 4]    
	mov   al, [esp + 4 + 4]  
	out   dx, al  
	ret

load_idt:
	mov edx, [esp + 4]
	lidt [edx]
	sti 																; включаем прерывания
	ret

keyboard_handler:                 
	call    keyboard_handler_main
	iretd
 
start:
	cli 																; блокировка прерываний
	mov esp, stack_space 												; указатель стека
	call kmain
	hlt 																; остановка процессора
 
section .bss
resb 8192 																; выделяем 8KB на стек
stack_space:
