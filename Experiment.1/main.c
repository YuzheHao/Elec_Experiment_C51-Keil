/*
	main.c
	控制交流蜂鸣器演奏简单的乐曲
*/


#include <reg51.h>


sbit BUZZER = P1^0;		//交流蜂鸣器由P1.3管脚控制

unsigned int LED[7];
unsigned int m;	

sbit LED1 = P0^0;	//定义LED由P0.0控制
sbit LED2 = P0^1;
sbit LED3 = P0^2;
sbit LED4 = P0^3;
sbit LED5 = P0^4;	
sbit LED6 = P0^5;	
sbit LED7 = P0^6;



//定义音名
//L1～L7表示低音，M1～M7表示中音，H1～H7表示高音
#define L1	0xF91F	//261.63（单位：Hz，下同）
#define L2	0xF9DF	//293.66
#define L3	0xFA8A	//329.63
#define L4	0xFAD9	//349.23（低音3到低音4为半音）
#define L5	0xFB68	//392.00
#define L6	0xFBE9	//440.00
#define L7	0xFC5B	//493.88
#define M1	0xFC8F	//523.25（低音7到中音1为半音）
#define M2	0xFCEF	//587.33
#define M3	0xFD45	//659.26
#define M4	0xFD6C	//698.46（中音3到中音4为半音）
#define M5	0xFDB4	//783.99
#define M6	0xFDF4	//880.00
#define M7	0xFE2D	//987.77
#define H1	0xFE48	//1046.5（中音7到高音1为半音）
#define H2	0xFE78	//1174.7
#define H3	0xFEA3	//1318.5
#define H4	0xFEB6	//1396.9（高音3到高音4为半音）
#define H5	0xFEDA	//1568.0
#define H6	0xFEFA	//1760.0
#define H7	0xFF17	//1975.5
#define OO	0x0000


//定义基本拍节
//T表示1拍，T/2表示半拍，T*2表示2两拍，等等
#define T	300


//定义音符结构
typedef struct
{
	unsigned int mFreq;		//频率对应的定时器初值
	unsigned int mDelay;	//延时值
	unsigned int mLED;
}CNote;


//定义定时器T1重装值
volatile char ReloadH;
volatile char ReloadL;


/*
函数：T1INTSVC()
功能：定时器T1中断服务函数
*/
void T1INTSVC() interrupt 3
{
	TR1 = 0;
	TH1 = ReloadH;
	TL1 = ReloadL;
	TR1 = 1;
	BUZZER = !BUZZER;
}


/*
函数：Delay()
功能：延时0.001～65.536s
参数：
	t>0时，延时(t*0.001)s
	t=0时，延时65.536s
*/
void Delay(unsigned int t)
{
	do
	{
		TH0 = 0xfa;
		TL0 = 0x24;
		TR0 = 1;
		while ( !TF0 );
		TR0 = 0;
		TF0 = 0;
	} while ( --t != 0 );
}


/*
函数：Sound()
功能：演奏一个音符
参数：
	*note：音符指针，指向要演奏的音符
*/
void Sound(CNote *note)
{
//利用定时器T1发出音符的频率
	if ( note->mFreq != 0 )
	{
		ReloadH = (unsigned char)(note->mFreq >> 8);
		ReloadL = (unsigned char)(note->mFreq);
		TH1 = 0xFF;
		TL1 = 0xF0;
		TR1 = 1;
	}
//拍节延时
	
	switch( note->mLED )
	{
		case 1:
			LED1=0;
			break;
		case 2:
			LED2=0;
			break;
		case 3:
			LED3=0;
			break;
		case 4:
			LED4=0;
			break;
		case 5:
			LED5=0;
			break;
		case 6:
			LED6=0;
			break;
		case 7:
			LED7=0;
			break;
	}


	Delay(note->mDelay);
	
//停止发声
	TR1 = 0;
	BUZZER = 1;

	LED1=1;
	LED2=1;
	LED3=1;
	LED4=1;
	LED5=1;
	LED6=1;
	LED7=1;

//短暂停顿
	Delay(700);
}


/*
函数：Play()
功能：演奏一段乐曲
参数：
	music[]：要演奏的乐曲
*/
void Play(CNote music[])
{
	unsigned int n = 0;
	for (;;)
	{
		if ( music[n].mDelay == 0 ) break;
		Sound(&(music[n]));
		n++;
	}
}


code CNote MusicTab[] =
{
//	{音名,节拍}
	{M7,T,7},
	{M6,T,6},
	{M5,T,5},
	{M4,T,4},
	{M3,T,3},
	{M2,T,2},
	{M1,T,1},
	{0,0}		//结束标志
};


										   

//主程序
void main()
{
	TMOD = 0x11;	//T0和T1都设置成16位定时器
	ET1 = 1;		//允许T1中断
	EA = 1;			//允许总中断

	for (;;)
	{
		Play(MusicTab);	//演奏

		Delay(1);	//
		
	}
}

