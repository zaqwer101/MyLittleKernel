kernel:
	make clean
	nasm -f elf32 kernel.asm -o kasm.o
	gcc -fno-stack-protector -m32 -c kernel.c -o kc.o 
	gcc -fno-stack-protector -m32 -c string.c -o string.o 
	ld -m elf_i386 -T link.ld -o kernel kasm.o kc.o string.o
clean:
	rm -rf kasm.o kc.o kernel.o kernel *.o *.out
