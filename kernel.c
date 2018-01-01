// https://community.bistudio.com/wiki/DIK_KeyCodes

#include "keyboard_map.h"
#define LINES 25
#define COLUMNS_IN_LINE 80
#define BYTES_FOR_EACH_ELEMENT 2
#define SCREENSIZE BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE * LINES
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 0x08

#define F1_KEY_CODE 0x3B
#define ENTER_KEY_CODE 0x1C
#define BACKSPACE_KEY_CODE 0x0E

#define KERNEL_CONSOLE_COLOR 0x03

extern unsigned char keyboard_map[128];

/* подгружаем функции из NASM */
extern void keyboard_handler(void);
extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned char data);
extern void load_idt(unsigned long *idt_ptr);

unsigned int current_loc = 0;											// Текущая позиция курсора

char *screen = (char*)0xb8000; 											// Тут начинается видеопамять 

struct IDT_entry {
	unsigned short int offset_lowerbits;
	unsigned short int selector;
	unsigned char zero;
	unsigned char type_attr;
	unsigned short int offset_higherbits;
};

struct IDT_entry IDT[IDT_SIZE];

//Магия, навык использования которой у меня не прокачан.
void idt_init(void)
{
	unsigned long keyboard_address;
	unsigned long idt_address;
	unsigned long idt_ptr[2];

	keyboard_address = (unsigned long)keyboard_handler;
	IDT[0x21].offset_lowerbits = keyboard_address & 0xffff;
	IDT[0x21].selector = KERNEL_CODE_SEGMENT_OFFSET;
	IDT[0x21].zero = 0;
	IDT[0x21].type_attr = INTERRUPT_GATE;
	IDT[0x21].offset_higherbits = (keyboard_address & 0xffff0000) >> 16;
	
	write_port(0x20 , 0x11);
	write_port(0xA0 , 0x11);
	
	write_port(0x21 , 0x20);
	write_port(0xA1 , 0x28);
	
	write_port(0x21 , 0x00);
	write_port(0xA1 , 0x00);
	
	write_port(0x21 , 0x01);
	write_port(0xA1 , 0x01);
	
	write_port(0x21 , 0xff);
	write_port(0xA1 , 0xff);
	
	idt_address = (unsigned long)IDT ;
	idt_ptr[0] = (sizeof (struct IDT_entry) * IDT_SIZE) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16 ;

	load_idt(idt_ptr);
}

void kb_init(void)
{
	write_port(0x21 , 0xFD);
}

void kprint(const char *str, const char *attr)							// Вывод на экран 
{
	unsigned int i = 0;
	while (str[i] != '\0') {
		screen[current_loc++] = str[i++];
		screen[current_loc++] = attr;
	}
}

void newline(void)														// Новая строка
{
	unsigned int line_size = BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE;
	current_loc = current_loc + (line_size - current_loc % (line_size));
}

void clear_screen()
{
        current_loc = 0;
	unsigned int i = 0;
	while (i < SCREENSIZE) 
	{
		screen[i++] = ' ';
		screen[i++] = KERNEL_CONSOLE_COLOR;
	}
}

// Это уходит в ассемблер
void keyboard_handler_main()
{
	unsigned char status;
	char keycode;
	
	write_port(0x20, 0x20);
	
	status = read_port(KEYBOARD_STATUS_PORT);
	
	if(status & 0x01)													// Если младший бит == 1, то в буфере что-то есть 
	{
                char *string[256];
                unsigned int counter = 0;
		keycode = read_port(KEYBOARD_DATA_PORT);
		
		if(keycode < 0) return;
		
		switch(keycode)
		{
                        case BACKSPACE_KEY_CODE:
			{
				screen[current_loc - 2] = ' ';
				current_loc -= 2;
				return;
			}
			
			case ENTER_KEY_CODE:
			{
                                counter = 0;
                                if(string == "test")
                                {
                                    clear_screen();
                                    kprint("Passed!", KERNEL_CONSOLE_COLOR);
                                }

                                newline();
				return;
			}

                        case F1_KEY_CODE:
                        {
                            clear_screen();
                            return;
                        }
			
			default:
			{
                                string[counter] = keyboard_map[(unsigned char) keycode];
                                counter++;
				screen[current_loc++] = keyboard_map[(unsigned char) keycode];
				screen[current_loc++] = KERNEL_CONSOLE_COLOR;
			}
			
		}
		
		
	}
}

void kmain(void)
{	
        clear_screen();
	kprint("Hi there!", 0x01);
	newline();
	
	idt_init();
	kb_init();

	while(1);
}



