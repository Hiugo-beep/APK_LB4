#include <dos.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
int attr = 0x6E;
int count=0;
struct VIDEO
{
	unsigned char symb;
	unsigned char attr;
};
int melody[] = {
880,880,988,1046,988,880,1046,
988,659,
1,1047,1046,784,784,659,523,
880,740,587,440,587,740,
692,880,1047,1319,1175,
988,784,659,698,
784,880,988,1047,1175,
1319,1175,1046,988,
1047,880,659,1046,880,659,1047,880,
988,830,659,988,830,659,988,830,
1047,880,659,1046,880,659,1047,880,
1175,880,740,1175,880,740,1175,880,
698,880,1046,1319,1175,
988,784,659,297,
698,659,698,784,880,988,784,880,988,1047,
880,988,1047,1175,988,1047,
82,125,165,247,330,415,494,415,330,247,330,247,208,165,125,
82,0
};

int clock[] = {
450,75,75,150,150,150,150,
600,600,
1,150,150,150,225,225,150,
225,225,150,225,225,150,
300,150,300,300,450,
150,300,300,450,
150,300,150,150,150,
450,75,75,600,
150,150,150,150,150,150,150,150,
150,150,150,150,150,150,150,150,
150,150,150,150,150,150,150,150,
150,150,150,150,150,150,150,150,
300,150,300,150,450,
150,300,300,150,
75,75,75,75,75,75,75,75,75,75,75,75,75,75,75,75,
150,75,75,75,75,75,75,75,75,75,75,75,75,75,75,
600,0
};

void setFrequency(unsigned int freq){
    //Порт 42 – канал 2, связан с динамиком
if(freq == 1) delay(200);
else {
	long base = 1193180, kd;
	// Устанавливаем режим 3 для второго канала таймера, 10 11 011 0 – канал, старший+младший байты, режим, формат (двоичный)
	outp(0x43, 0xB6); //Включение динамика 10 11 011 0 – канал, старший+младший байты, режим, формат (двоичный)
	kd = base / freq; //Коэффициент деления
	outp(0x42, kd % 256); //младший бит
	kd /= 256; 
	outp(0x42, kd);  //старшимй бит
	}
}

void play()
{
	for(int i=0;melody[i]!=0 || clock[i]!=0;i++){
		setFrequency(melody[i]);
		// Включаем громкоговоритель. Сигнал от
		// канала 2 таймера теперь будет проходить
		// на вход громкоговорителя
		outp(0x61, inp(0x61)|3);//turn ON
		//Пауза
		delay(clock[i]);
		// Выключаем громкоговоритель.
		outp(0x61, inp(0x61) & 0xfc);//turn OFF
	}
}

void binary_notation(int x, int offset)
{
char temp;
	int i;
	VIDEO far* screen = (VIDEO far *)MK_FP(0xB800,offset);
	for(i=7;i>=0;i--) //поделить по битам
	{
		temp = x% 2;
		x /= 2;
		screen->symb=temp + '0';//заполнить экран
		screen->attr=attr;
		screen++;
	}
screen->symb = ' ';
screen++;
}


void print_l(long x, int offset)
{
int number=1;
char temp;
if (x>9) number=2;
if(x>99) number=3;
if(x>999) number=4;
if (x>9999) number=5;
	int i;
	VIDEO far* screen = (VIDEO far *)MK_FP(0xB800,offset);

char mass[5];
for(int k=0;k<5;k++)
mass[k]='0';
	for(i=1;x!=0;i++) //поделить по битам
	{
		temp = x% 10;
		x /= 10;
mass[number-i] = temp+'0';
	}
for(i=0;i<number;i++)
{               screen->symb=mass[i];//заполнить экран
		screen->attr=attr;
		screen++;
}
screen->symb = ' ';
screen++;
}


int hexadecimal_notation(long x, int offset){
	char temp;
	int i;
	VIDEO far* screen = (VIDEO far *)MK_FP(0xB800,offset);
	screen->symb = '0';
	screen++;
	screen->symb = 'x';
	screen++;
	char number[4];
	for(i=3;i>-1;i--) //поделить по битам
	{
		temp = x% 16;
		x /= 16;
		if(temp>=0 && temp <=9) {
			number[i]=temp + '0';
		}
		else {
		
			number[i]=temp + 55;//заполнить экран
		}
	}
	for(i=0;i<4;i++) {
		screen->symb=number[i];//заполнить экран
		screen->attr=attr;
		screen++;
	}
screen->symb = ' ';
screen++;
return 4;
}

void printWords(){ //слово состояния
    //Для каждого канала
	outp(0x43, 0xE2);   //40 11 10 001 0b    
	binary_notation(inp(0x40), 0);
	outp(0x43, 0xE4 );  //41  11 10 010 0b
	binary_notation(inp(0x41), 16+2);
	outp(0x43, 0xE8 );  //42  11 10 100 0b
	binary_notation(inp(0x42), 32+4);
}

void printKd() {
	int kdLow;
	int kdHigh;
	long  kd;
	int new_offset;
	long  max=0;
	long j =0;
	const long  nTimes = 65535;
	//outp(0x61, inp(0x61)|1);//turn ON
	for (j= 0; j < nTimes ; j++) {
		outp(0x43, 0x0);  //00 00 0000
		kdLow  = inp(0x40);
		kdHigh = inp(0x40);
		kd = kdHigh*256 + kdLow;
		if(max<kd)
			max=kd;
	}
	new_offset = hexadecimal_notation(max, 0);
	new_offset*=2;
	max=0;
	for (j = 0; j < nTimes; j++) {
		outp(0x43, 0x40);  //0
		kdLow  = inp(0x41);
		kdHigh = inp(0x41);
		kd = kdHigh*256 + kdLow;
		if(max<kd)
			max=kd;
	}
	new_offset = hexadecimal_notation(max, new_offset+6);
	new_offset *= 4;
	max = 0;
	outp(0x43, 0xB6);
	outp(0x61, inp(0x61) | 1);
	for (j= 0; j < nTimes; j++) {
		outp(0x43, 0x80);  //0      
		kdLow  = inp(0x42);
		kdHigh = inp(0x42);
		kd = kdHigh*256 + kdLow;
		if(max<kd)
			max=kd;
		}
	new_offset = hexadecimal_notation(max, new_offset+12);
	outp(0x61, inp(0x61) & 0xfc);
}

void getRandom(int number) {
	int kdLow;
	int kdHigh;
	int new_offset;

	long  kd;
		outp(0x43, 0xB6);
		kd = number; //Коэффициент деления
		outp(0x42, kd % 256); //младший бит
		kd /= 256; 
		outp(0x42, kd);  //старшимй бит
	        // Разрешаем работу канала
		outp(0x43, 0xB6);
		outp(0x61, inp(0x61) | 1);
		delay(number);
		
		outp(0x43, 0x80);  //0      
		kdLow  = inp(0x42);
		kdHigh = inp(0x42);
		kd = kdHigh*256 + kdLow; 
		kd+=count;
		if(count == 1) count = 0;
		else count = 1;
print_l(kd,0);


}

int main()
{
int i;
	long range;
	char selection;

        while (1) {
		printf("1: Play sound \n");
		printf("2: Print KD\n");
		printf("3: Print condition words\n");
		printf("4: Generate random numbers\n");
		printf("5: Exit(e)\n\n");
		selection = getch();

		switch (selection) {
			case '1':
				play();
				break;  
			case '2':
				system("cls");
				printKd();
				getch();
system("cls");
				break;
			case '3':
				system("cls");
				printWords();
				getch();
                                system("cls");
				break;
			case '4':
				system("cls");
				printf("Enter the above range of random: ");
				scanf("%ld", &range);
system("cls");
				getRandom(range);
getch();
system("cls");
				break;
			case '5':
				printf("Exiting...");
				return 0;
                         case 'e':
				printf("Exiting...");
				return 0;
			default:
				printf("Error key! Please try again\n");
				break;
		}
	}
//return 0;
}
