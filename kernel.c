void kmain(void)
{
	const char *str = "Hello, my little dude";
 	char *screen = (char*)0xb8000; // Тут начинается видеопамять 
 	unsigned int i = 0;
 	unsigned int j = 0;
 
 	while(j < 80 * 25 * 2) {	// Магия очистки экрана 
		screen[j] = ' ';		// Символ, который вносим в участок
		screen[j+1] = 0x07; 	// Атрибуты символа
		j = j + 2;
	}
 	
	j = 0;
 	
	while(str[j] != '\0') {
 		screen[i] = str[j];		// То же, что и с очисткой, только заносим части строки
	 	screen[i+1] = 0x03;
 		++j;
 		i = i + 2;
 	}
 	
	return;
}

void clean()
{
		
}

void print()
{
	
}
