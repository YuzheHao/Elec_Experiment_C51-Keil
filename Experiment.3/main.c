/*
	main.c
	加减计数器
*/


#include <reg51.h>
#include <absacc.h>
#include <ctype.h>


//定义按键
sbit KEY1 = P2^2;		//定义按键1，'+'按键，对应核心板上SW_2
sbit KEY2 = P2^0;		//定义按键2，'-'按键，对应核心板上SW_3

code unsigned char Tab[] =
	{//定义0123456789AbCdEF的数码管字型数据
		0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,
		0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71
	};

sbit PWM = P0^0;

//定义显示缓冲区（由定时中断程序自动扫描）
unsigned char DispBuf[8];

//定义PWM最大级数，也就是调节LED的亮度等级
#define PWM_MAX	8
					   
//定义PWM级数，分为0～PWM_MAX-1级
unsigned char PwmValue;

/*
函数：KeyScan()
功能：键盘扫描
返回：扫描到的键值
*/
unsigned char KeyScan()
{
	unsigned char k = '\0';
	if ( KEY1 == 0 ) k = '+';
	if ( KEY2 == 0 ) k = '-';
	return k;
}

/*
函数：T1INTSVC()
功能：定时器T1的中断服务函数
*/
void T1INTSVC() interrupt 3
{
	code unsigned char com[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
	static unsigned char n = 0;
	static unsigned char t = 0;
	TR1 = 0;
	TH1 = 0xFA;
	TL1 = 0x24;
	TR1 = 1;
	XBYTE[0x7800] = 0xFF;		//暂停显示
	XBYTE[0x7801] = ~DispBuf[n];	//更新扫描数据
	XBYTE[0x7800] = ~com[n];//	重新显示
	n++	;
	n &= 0x07;

	t++;
	if ( t >= PWM_MAX-1 ) t = 0;	//PWM波的周期为：PWM_MAX*1ms = 20ms
	if ( t < PwmValue )
	{
		PWM = 1;		//PWM波高电平时间(LED灭)：PwmValue*1ms
	}
	else
	{
		PWM = 0;		//PWM波低电平时间(LED亮)：（PWM_MAX-PwmValue）*1ms
	}
}


/*
函数：DispClear()
功能：清除数码管的所有显示
*/
void DispClear()
{
	unsigned char i;
	for ( i=0; i<8; i++ )
	{
		DispBuf[i] = 0x00;
	}
}



/*
函数：DispInit()
功能：数码管扫描显示初始化
*/
void DispInit()
{
	DispClear();	//初始为全灭
	EA = 0;
	TH1 = 0xFA;
	TL1 = 0x24;
	TR1 = 1;
	ET1 = 1;
	EA = 1;
}


/*
函数：Delay()
功能：延时0.01～2.56s
参数：
	t>0时，延时(t*0.01)s
	t=0时，延时2.56s
说明：
	晶振用18MHz
*/
void Delay(unsigned char t)
{
	do
	{
		TH0 = 0xC5;		//定时时间10ms
		TL0 = 0x68;
		TR0 = 1;
		while ( !TF0 );
		TR0 = 0;
		TF0 = 0;
	} while ( --t != 0 );
}


/*
函数：SysInit()
功能：系统初始化
*/
void SysInit()
{
	TMOD = 0x11;	//设置定时器T0、T1为16位定时器
	DispInit();		//数码管扫描显示初始化
}


void main()
{

	unsigned char k;		//定义键值变量
	unsigned char cnt = 14;	//定义计数值变量的初始值

	SysInit();				//系统初始化
	DispBuf[0] = Tab[cnt % 10];
	DispBuf[1] = Tab[cnt / 10];

	for (;;)
	{
		Delay(1);		//延时10ms
		k = KeyScan();	//键盘扫描
		switch ( k )		//判断键值，执行具体功能
		{
		case '+':
			if ( cnt < 18 ) cnt++;
			break;
		case '-':
			if ( cnt > 11 ) cnt--;
			break;
		default:
			break;
		}
		PwmValue = 18-cnt;
		DispBuf[0] = Tab[cnt % 10];
		DispBuf[1] = Tab[cnt / 10];

		for (;;)
		{
			Delay(1);		//延时50ms
			if ( KeyScan() == '\0' ) break;	//如果按键释放，退出循环
		}
	}
}

